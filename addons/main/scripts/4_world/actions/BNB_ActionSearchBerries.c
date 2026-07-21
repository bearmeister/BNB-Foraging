// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.1.4
// Modified:     2026-07-22
//
// BNB_ActionSearchBerries.c - continuous "Search for berries" action on berry
// bushes (rosaCanina -> CaninaBerry, sambucusNigra -> SambucusBerry). Cursor-
// targeted, no item. Server rolls count + food stage and spawns berries in front
// of the player; per-bush cooldown gates re-use (client gate hides until expiry).

class ActionSearchBerriesCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        float dur = 10.0;
        BNB_SearchBerriesConfigBlock b = BNB_SearchBerriesSettings.Get();
        if (b)
        {
            dur = b.search_duration_seconds;
            if (dur <= 0) dur = 10.0;
        }
        m_ActionData.m_ActionComponent = new CAContinuousTime(dur);
    }
}

class ActionSearchBerries : ActionContinuousBase
{
    void ActionSearchBerries()
    {
        m_CallbackClass = ActionSearchBerriesCB;

        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEIDX_ERECT | DayZPlayerConstants.STANCEIDX_CROUCH;

        m_Text = "Search for berries";
    }

    override bool HasProgress() { return true; }

    override typename GetInputType()
    {
        return ContinuousDefaultActionInput;
    }

    override void CreateConditionComponents()
    {
        m_ConditionItem   = new CCINone;
        m_ConditionTarget = new CCTCursor();
    }

    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        if (!target) return false;
        Object obj = target.GetObject();
        if (!obj) return false;

        if (BNB_SharedForageHelpers.BlockedByFullHands(player)) return false;

        if (obj.IsInherited(ItemBase)) return false;
        if (obj.IsInherited(Man)) return false;

        string berryClass;
        if (!BNB_SearchBerriesHelpers.IsBerryBush(obj, berryClass))
            return false;

        string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);

        if (GetGame() && GetGame().IsServer())
        {
            int expiry = BNB_SharedForageCooldownRegistry.GetCooldown(key);
            if (expiry > 0)
            {
                int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
                if (now < expiry)
                    return false;
            }
        }

        if (GetGame() && !GetGame().IsServer())
        {
            int expiryC = BNB_SharedForageCooldownRegistry.GetCooldown(key);
            if (expiryC > 0)
            {
                int nowC = BNB_SharedForageCooldownRegistry.NowEpochSec();
                if (nowC < expiryC)
                    return false;
            }
            m_Text = "Search for berries";
        }

        return true;
    }

    override void OnFinishProgressServer(ActionData action_data)
    {
        super.OnFinishProgressServer(action_data);
        if (!GetGame() || !GetGame().IsServer()) return;

        PlayerBase player = action_data.m_Player;
        if (!player) return;

        Object obj = action_data.m_Target.GetObject();
        if (!obj) return;

        string berryClass;
        if (!BNB_SearchBerriesHelpers.IsBerryBush(obj, berryClass))
            return;

        BNB_SearchBerriesConfigBlock b = BNB_SearchBerriesSettings.Get();

        if (b && b.bush_cooldown_minutes > 0)
        {
            string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);
            int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
            int expiry = now + b.bush_cooldown_minutes * 60;
            BNB_SharedForageCooldownRegistry.SetAndPersist(key, expiry);
        }

        int cutN = 0;
        if (b && BNB_SharedForageHelpers.RollBareHandCut(player, b.cut_chance_no_gloves, b.glove_damage))
            cutN = 1;

        float chance = 0.0;
        if (b) chance = b.berry_find_chance;
        if (Math.RandomFloat01() >= chance)
        {
            if (BNB_ForagingConfig.DebugEnabled())
            {
                string missMsg = "SearchBerries player=" + BNB_SharedForageHelpers.PlayerName(player);
                missMsg = missMsg + " target=" + obj.GetType() + " yield=0";
                missMsg = missMsg + " cut=" + cutN;
                BNB_SharedForageHelpers.DebugLog(missMsg);
            }
            return;
        }

        int minC = 1;
        int maxC = 1;
        if (b)
        {
            minC = b.berry_min_count;
            maxC = b.berry_max_count;
        }
        int count = Math.RandomIntInclusive(minC, maxC);

        int toHands = 0;
        if (count > 0)
            toHands = BNB_SearchBerriesHelpers.SpawnBerriesInFront(player, berryClass, count);
        if (BNB_ForagingConfig.DebugEnabled())
        {
            string hitMsg = "SearchBerries player=" + BNB_SharedForageHelpers.PlayerName(player);
            hitMsg = hitMsg + " target=" + obj.GetType() + " item=" + berryClass;
            hitMsg = hitMsg + " yield=" + count + " hands=" + toHands + " ground=" + (count - toHands);
            hitMsg = hitMsg + " cut=" + cutN;
            BNB_SharedForageHelpers.DebugLog(hitMsg);
        }
    }

    override void OnFinishProgressClient(ActionData action_data)
    {
        super.OnFinishProgressClient(action_data);
        if (!GetGame() || GetGame().IsServer()) return;

        PlayerBase player = action_data.m_Player;
        if (!player) return;

        Object obj = action_data.m_Target.GetObject();
        if (!obj) return;

        BNB_SearchBerriesConfigBlock b = BNB_SearchBerriesSettings.Get();
        if (!b) return;

        if (b.bush_cooldown_minutes > 0)
        {
            string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);
            int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
            int expiry = now + b.bush_cooldown_minutes * 60;
            BNB_SharedForageCooldownRegistry.SetCooldown(key, expiry);
        }
    }

    override bool HasTarget() { return true; }
}
