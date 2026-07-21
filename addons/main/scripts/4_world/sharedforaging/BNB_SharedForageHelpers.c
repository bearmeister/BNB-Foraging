// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.3.3
// Modified:     2026-07-22
//
// BNB_SharedForageHelpers.c - food-stage roll + apply, the shared hands-first
// spawn, and the bare-hand cut roll, used by every foraging verb. Chances are
// passed in so each feature keeps its own config block.

class BNB_SharedForageHelpers
{
    // First find goes to empty hands, the rest scatter on the ground so they do
    // not stack. toHands reports which path was taken.
    static Object SpawnForageItem(PlayerBase player, string className, bool allowHands, out bool toHands)
    {
        toHands = false;
        if (!GetGame() || !player || className == "")
            return null;

        HumanInventory hi = player.GetHumanInventory();
        if (allowHands && hi && !hi.GetEntityInHands())
        {
            EntityAI held = hi.CreateInHands(className);
            if (held)
            {
                toHands = true;
                return Object.Cast(held);
            }
        }

        return Object.Cast(player.SpawnEntityOnGroundRaycastDispersed(className, 1.0, UAItemsSpreadRadius.NARROW));
    }

    // Bare-hand forage cut: an intact worn glove wears + protects; a ruined or
    // absent glove rolls a cut - fixed shock (grunt + flinch) + a forearm bleed.
    static bool RollBareHandCut(PlayerBase player, float cutChance, float gloveDamage)
    {
        if (!player || cutChance <= 0) return false;

        ItemBase gloves = ItemBase.Cast(player.FindAttachmentBySlotName("Gloves"));
        if (gloves && !gloves.IsDamageDestroyed())
        {
            if (gloveDamage > 0) gloves.DecreaseHealth("", "", gloveDamage);
            return false;
        }

        if (Math.RandomFloat01() >= cutChance) return false;

        // Small shock (~5: coef 0.05 on FallDamageShock=100) so the player grunts +
        // flinches on a cut; too small to KO anyone but an already critically-shocked player.
        player.ProcessDirectDamage(DamageType.CUSTOM, player, "", "FallDamageShock", vector.Zero, 0.05);

        BleedingSourcesManagerServer bm = player.GetBleedingManagerServer();
        if (bm)
        {
            if (Math.RandomIntInclusive(0, 1) == 0)
            {
                if (!bm.AttemptAddBleedingSourceBySelection("LeftForeArmRoll"))
                    bm.AttemptAddBleedingSourceBySelection("RightForeArmRoll");
            }
            else
            {
                if (!bm.AttemptAddBleedingSourceBySelection("RightForeArmRoll"))
                    bm.AttemptAddBleedingSourceBySelection("LeftForeArmRoll");
            }
        }
        return true;
    }

    // Free-hands gate: true when search_requires_free_hands is on and the player is
    // holding something, so the Search option stays off the scroll wheel.
    static bool BlockedByFullHands(PlayerBase player)
    {
        if (!BNB_ForagingConfig.RequiresFreeHands()) return false;
        return player && player.GetHumanInventory() && player.GetHumanInventory().GetEntityInHands() != null;
    }

    // Debug sink for the foraging verbs; no-op unless foraging.json debug_log=1.
    static void DebugLog(string line)
    {
        if (BNB_ForagingConfig.DebugEnabled())
            Print("[BNB_Foraging::Debug] " + line);
    }

    // Identity is null for AI and during early join - never assume it resolves.
    static string PlayerName(PlayerBase player)
    {
        if (!player) return "?";
        PlayerIdentity id = player.GetIdentity();
        if (!id) return "?";
        return id.GetName();
    }

    // Returns a FoodStageType, or -1 to leave the item at its default stage.
    // enable is an int toggle (0/1) matching foraging.json food_stage_enable.
    static int RollFoodStage(int enable, float driedChance, float rottenChance)
    {
        if (!enable)
            return -1;

        float dried  = Math.Clamp(driedChance,  0.0, 1.0);
        float rotten = Math.Clamp(rottenChance, 0.0, 1.0);

        // Normalise so the two bands never overrun the 0..1 roll space.
        float sum = dried + rotten;
        if (sum > 1.0)
        {
            dried  = dried  / sum;
            rotten = rotten / sum;
        }

        float r = Math.RandomFloat01();
        if (r < dried)          return FoodStageType.DRIED;
        if (r < dried + rotten) return FoodStageType.ROTTEN;
        return -1;
    }

    static void ApplyFoodStage(Edible_Base edible, int stage)
    {
        if (!edible) return;
        if (stage < 0) return;
        FoodStage fs = edible.GetFoodStage();
        if (fs)
            fs.ChangeFoodStage(stage);
    }
}
