// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.1.0
// Modified:     2026-07-20
//
// BNB_SearchMushroomsHelpers.c - forest-stump classification + mushroom spawn
// helpers. A felled/forest stump reports its bare base class (TreeHard /
// TreeSoft); a standing tree carries a _t_<species> suffix, so an exact base
// match selects stumps only. Spawns oyster + wood-ear (wood-decay) mushrooms.

class BNB_SearchMushroomsHelpers
{
    static ref TStringArray s_StumpClasses;
    static ref TStringArray s_EdibleMushrooms;

    static void Init()
    {
        if (!s_StumpClasses) s_StumpClasses = new TStringArray;
        if (s_StumpClasses.Count() == 0)
        {
            s_StumpClasses.Insert("treehard");
            s_StumpClasses.Insert("treesoft");
        }

        if (!s_EdibleMushrooms) s_EdibleMushrooms = new TStringArray;
        if (s_EdibleMushrooms.Count() == 0)
        {
            s_EdibleMushrooms.Insert("PleurotusMushroom");    // Oyster
            s_EdibleMushrooms.Insert("AuriculariaMushroom");  // Wood Ear
        }
    }

    // A stump is the bare base class (TreeHard / TreeSoft), case-insensitive
    // exact match. Standing trees ("TreeHard_t_<species>") never match.
    static bool IsForageStump(Object obj)
    {
        if (!obj) return false;
        if (obj.IsInherited(ItemBase)) return false;
        if (obj.IsInherited(Man)) return false;

        Init();

        string tLower = obj.GetType();
        tLower.ToLower();
        return s_StumpClasses.Find(tLower) != -1;
    }

    static string PickMushroomClass()
    {
        Init();
        int n = s_EdibleMushrooms.Count();
        if (n == 0) return "";
        int i = Math.RandomInt(0, n);
        return s_EdibleMushrooms.Get(i);
    }

    // Returns 1 if the first find went into the player's hands, else 0.
    static int SpawnMushroomsInFront(PlayerBase player, int count)
    {
        int toHands = 0;
        if (!GetGame() || !player) return toHands;

        BNB_SearchMushroomsConfigBlock b = BNB_SearchMushroomsSettings.Get();

        for (int i = 0; i < count; i++)
        {
            string className = PickMushroomClass();
            if (className == "") continue;

            bool hands;
            Object o = BNB_SharedForageHelpers.SpawnForageItem(player, className, i == 0, hands);
            if (hands) toHands++;

            // Foraged mushrooms spawn worn/damaged (never pristine); quantity
            // tracks health fraction and nutrition scales with quantity consumed.
            ItemBase ib = ItemBase.Cast(o);
            if (ib)
            {
                float hp01 = Math.RandomFloat(0.51, 0.95);
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
