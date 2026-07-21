// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.1.0
// Modified:     2026-07-20
//
// BNB_SearchBerriesHelpers.c - berry-bush classification + berry spawn. Bushes
// carry the Latin genus in their class name (BushSoft/BushHard_b_<genus>_...):
// rosaCanina -> CaninaBerry, sambucusNigra -> SambucusBerry (both vanilla items).
// Fills the gap left by vanilla's deprecated, unregistered ActionPickBerry.

class BNB_SearchBerriesHelpers
{
    // Returns true + the vanilla berry class for a known berry bush.
    static bool IsBerryBush(Object obj, out string berryClass)
    {
        berryClass = "";
        if (!obj) return false;
        if (obj.IsInherited(ItemBase)) return false;
        if (obj.IsInherited(Man)) return false;

        string t = obj.GetType();
        if (t == "") return false;
        t.ToLower();

        if (t.Contains("rosacanina"))
        {
            berryClass = "CaninaBerry";
            return true;
        }
        if (t.Contains("sambucusnigra"))
        {
            berryClass = "SambucusBerry";
            return true;
        }
        return false;
    }

    // Returns 1 if the first find went into the player's hands, else 0.
    static int SpawnBerriesInFront(PlayerBase player, string className, int count)
    {
        int toHands = 0;
        if (!GetGame() || !player || className == "") return toHands;

        BNB_SearchBerriesConfigBlock b = BNB_SearchBerriesSettings.Get();

        for (int i = 0; i < count; i++)
        {
            bool hands;
            Object o = BNB_SharedForageHelpers.SpawnForageItem(player, className, i == 0, hands);
            if (hands) toHands++;

            // Foraged berries spawn worn-to-pristine; quantity tracks health
            // fraction and nutrition scales with quantity consumed.
            ItemBase ib = ItemBase.Cast(o);
            if (ib)
            {
                float hp01 = Math.RandomFloat(0.7, 1.0);
                ib.SetHealth01("", "", hp01);
                ib.SetQuantityNormalized(hp01, false);
            }

            if (!b) continue;
            int stage = BNB_SharedForageHelpers.RollFoodStage(b.food_stage_enable, b.food_dried_chance, b.food_rotten_chance);
            if (stage >= 0)
                BNB_SharedForageHelpers.ApplyFoodStage(Edible_Base.Cast(o), stage);
        }

        return toHands;
    }
}
