// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.9
// Modified:     2026-07-21
//
// BNB_ActionSearchMushrooms.c - continuous "Search for mushrooms" action on forest tree
// stumps (bare TreeHard / TreeSoft). Cursor-targeted, no item. Server rolls
// count + food stage and spawns edible mushrooms in front of the player;
// per-stump cooldown gates re-use (client gate hides until expiry). Shares
// BNB_SharedForageCooldownRegistry's cooldown registry - keys are namespaced by class name
// and rounded position so stump + tree entries coexist.

class ActionSearchMushroomsCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        float dur = 10.0;
        BNB_SearchMushroomsConfigBlock b = BNB_SearchMushroomsSettings.Get();
        if (b)
        {
            dur = b.search_duration_seconds;
            if (dur <= 0) dur = 10.0;
        }
        m_ActionData.m_ActionComponent = new CAContinuousTime(dur);
    }
}

class ActionSearchMushrooms : ActionContinuousBase
{
    void ActionSearchMushrooms()
    {
        m_CallbackClass = ActionSearchMushroomsCB;

        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEIDX_ERECT | DayZPlayerConstants.STANCEIDX_CROUCH;

        m_Text = "Search for mushrooms";
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

        if (obj.IsInherited(ItemBase)) return false;
        if (obj.IsInherited(Man)) return false;

        if (!BNB_SearchMushroomsHelpers.IsForageStump(obj))
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
            m_Text = "Search for mushrooms";
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

        if (!BNB_SearchMushroomsHelpers.IsForageStump(obj))
            return;

        BNB_SearchMushroomsConfigBlock b = BNB_SearchMushroomsSettings.Get();

        if (b && b.stump_cooldown_minutes > 0)
        {
            string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);
            int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
            int expiry = now + b.stump_cooldown_minutes * 60;
            BNB_SharedForageCooldownRegistry.SetAndPersist(key, expiry);
        }

        int cutN = 0;
        if (b && BNB_SharedForageHelpers.RollBareHandCut(player, b.cut_chance_no_gloves, b.glove_damage))
            cutN = 1;

        float chance = 0.0;
        if (b) chance = b.mushroom_find_chance;
        if (Math.RandomFloat01() >= chance)
        {
            if (BNB_ForagingConfig.DebugEnabled())
            {
                string missMsg = "SearchMushrooms player=" + BNB_SharedForageHelpers.PlayerName(player);
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
            minC = b.mushroom_min_count;
            maxC = b.mushroom_max_count;
        }
        int count = Math.RandomIntInclusive(minC, maxC);

        int toHands = 0;
        if (count > 0)
            toHands = BNB_SearchMushroomsHelpers.SpawnMushroomsInFront(player, count);
        if (BNB_ForagingConfig.DebugEnabled())
        {
            string hitMsg = "SearchMushrooms player=" + BNB_SharedForageHelpers.PlayerName(player);
            hitMsg = hitMsg + " target=" + obj.GetType();
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

        BNB_SearchMushroomsConfigBlock b = BNB_SearchMushroomsSettings.Get();
        if (!b) return;

        if (b.stump_cooldown_minutes > 0)
        {
            string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);
            int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
            int expiry = now + b.stump_cooldown_minutes * 60;
            BNB_SharedForageCooldownRegistry.SetCooldown(key, expiry);
        }
    }

    override bool HasTarget() { return true; }
}
