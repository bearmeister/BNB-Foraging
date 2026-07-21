// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.10
// Modified:     2026-07-22
//
// BNB_ActionSearchFruit.c - continuous "Search for fruit" action on fruit trees. Cursor-
// targeted, no item. Server rolls count + food stage and spawns fruit in front
// of the player; per-tree cooldown gates re-use (client gate hides until
// expiry).

class ActionSearchFruitCB : ActionContinuousBaseCB
{
    override void CreateActionComponent()
    {
        float dur = 10.0;
        BNB_SearchFruitConfigBlock b = BNB_SearchFruitSettings.Get();
        if (b)
        {
            dur = b.search_duration_seconds;
            if (dur <= 0) dur = 10.0;
        }
        m_ActionData.m_ActionComponent = new CAContinuousTime(dur);
    }
}

class ActionSearchFruit : ActionContinuousBase
{
    void ActionSearchFruit()
    {
        m_CallbackClass = ActionSearchFruitCB;

        m_CommandUID = DayZPlayerConstants.CMD_ACTIONFB_INTERACT;
        m_FullBody = true;
        m_StanceMask = DayZPlayerConstants.STANCEIDX_ERECT | DayZPlayerConstants.STANCEIDX_CROUCH;

        m_Text = "Search for fruit";
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

        int fruitType;
        if (!BNB_SearchFruitHelpers.IsFruitTree(obj, fruitType))
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
            m_Text = "Search for fruit";
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

        int fruitType;
        if (!BNB_SearchFruitHelpers.IsFruitTree(obj, fruitType))
            return;

        BNB_SearchFruitConfigBlock b = BNB_SearchFruitSettings.Get();

        if (b && b.tree_cooldown_minutes > 0)
        {
            string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);
            int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
            int expiry = now + b.tree_cooldown_minutes * 60;
            BNB_SharedForageCooldownRegistry.SetAndPersist(key, expiry);
        }

        int cutN = 0;
        if (b && BNB_SharedForageHelpers.RollBareHandCut(player, b.cut_chance_no_gloves, b.glove_damage))
            cutN = 1;

        float chance = BNB_SearchFruitSettings.ChanceFor(fruitType);
        if (Math.RandomFloat01() >= chance)
        {
            if (BNB_ForagingConfig.DebugEnabled())
            {
                string missMsg = "SearchFruit player=" + BNB_SharedForageHelpers.PlayerName(player);
                missMsg = missMsg + " target=" + obj.GetType() + " yield=0";
                missMsg = missMsg + " cut=" + cutN;
                BNB_SharedForageHelpers.DebugLog(missMsg);
            }
            return;
        }

        int minC, maxC;
        BNB_SearchFruitSettings.CountRangeFor(fruitType, minC, maxC);
        int count = Math.RandomIntInclusive(minC, maxC);

        string fruitClass = "";
        switch (fruitType)
        {
            case eBNB_SearchFruitType.APPLE: fruitClass = "Apple"; break;
            case eBNB_SearchFruitType.PLUM:  fruitClass = "Plum";  break;
            case eBNB_SearchFruitType.PEAR:  fruitClass = "Pear";  break;
            default: return;
        }

        int toHands = 0;
        if (count > 0)
            toHands = BNB_SearchFruitHelpers.SpawnFruitInFront(player, fruitClass, count);
        if (BNB_ForagingConfig.DebugEnabled())
        {
            string hitMsg = "SearchFruit player=" + BNB_SharedForageHelpers.PlayerName(player);
            hitMsg = hitMsg + " target=" + obj.GetType() + " item=" + fruitClass;
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

        BNB_SearchFruitConfigBlock b = BNB_SearchFruitSettings.Get();
        if (!b) return;

        if (b.tree_cooldown_minutes > 0)
        {
            string key = BNB_SharedForageCooldownRegistry.MakeKey(obj);
            int now = BNB_SharedForageCooldownRegistry.NowEpochSec();
            int expiry = now + b.tree_cooldown_minutes * 60;
            BNB_SharedForageCooldownRegistry.SetCooldown(key, expiry);
        }
    }

    override bool HasTarget() { return true; }
}
