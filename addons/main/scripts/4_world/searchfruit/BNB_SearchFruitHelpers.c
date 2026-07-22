// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.2.0
// Modified:     2026-07-22
//
// BNB_SearchFruitHelpers.c - fruit-tree classification + fruit spawn. Resolution
// order: exact vanilla classname, then CfgVehicles fruitType, then Latin genus.

enum eBNB_SearchFruitType
{
    NONE = 0,
    APPLE = 1,
    PLUM = 2,
    PEAR = 3
}

class BNB_SearchFruitHelpers
{
    // All three maps are lowercase-keyed; lookups lowercase the probe first.
    static ref map<string, int> s_KnownTrees;
    static ref map<string, int> s_GenusFruit;
    static ref map<string, int> s_FruitNames;
    // Per-classname classification memo (NONE cached too) - ActionCondition runs
    // per frame, so the config/genus resolution must not.
    static ref map<string, int> s_ClassCache;

    static void Init()
    {
        if (s_GenusFruit)
            return;

        s_ClassCache = new map<string, int>();
        s_GenusFruit = new map<string, int>();
        s_GenusFruit.Set("malus",  eBNB_SearchFruitType.APPLE);
        s_GenusFruit.Set("prunus", eBNB_SearchFruitType.PLUM);
        s_GenusFruit.Set("pyrus",  eBNB_SearchFruitType.PEAR);

        s_FruitNames = new map<string, int>();
        s_FruitNames.Set("apple", eBNB_SearchFruitType.APPLE);
        s_FruitNames.Set("plum",  eBNB_SearchFruitType.PLUM);
        s_FruitNames.Set("pear",  eBNB_SearchFruitType.PEAR);

        // Vanilla Chernarus fruit-tree classes (Bohemia game data) - fast path
        // that skips the config read and the genus scan.
        s_KnownTrees = new map<string, int>();
        s_KnownTrees.Set("treesoft_t_malusdomestica_1s",        eBNB_SearchFruitType.APPLE);
        s_KnownTrees.Set("treesoft_t_malusdomestica_2s",        eBNB_SearchFruitType.APPLE);
        s_KnownTrees.Set("treesoft_t_malusdomestica_3s",        eBNB_SearchFruitType.APPLE);
        s_KnownTrees.Set("treesoft_t_malusdomestica_2s_summer", eBNB_SearchFruitType.APPLE);
        s_KnownTrees.Set("treesoft_t_malusdomestica_3s_summer", eBNB_SearchFruitType.APPLE);
        s_KnownTrees.Set("treehard_t_prunusdomestica_2s",       eBNB_SearchFruitType.PLUM);
        s_KnownTrees.Set("treehard_t_pyruscommunis_3s",         eBNB_SearchFruitType.PEAR);
        s_KnownTrees.Set("treehard_t_pyruscommunis_3sb",        eBNB_SearchFruitType.PEAR);
        s_KnownTrees.Set("treehard_t_pyruscommunis_3s_summer",  eBNB_SearchFruitType.PEAR);
        s_KnownTrees.Set("treesoft_t_pyruscommunis_2s",         eBNB_SearchFruitType.PEAR);
        s_KnownTrees.Set("treesoft_t_pyruscommunis_2s_summer",  eBNB_SearchFruitType.PEAR);
    }

    static bool IsFruitTree(Object obj, out int fruitType)
    {
        fruitType = eBNB_SearchFruitType.NONE;
        if (!obj) return false;
        if (obj.IsInherited(ItemBase)) return false;
        if (obj.IsInherited(Man)) return false;

        string typeName = obj.GetType();
        if (typeName == "") return false;

        Init();

        string probe = typeName;
        probe.ToLower();

        if (s_ClassCache.Find(probe, fruitType))
            return fruitType != eBNB_SearchFruitType.NONE;

        fruitType = ClassifyFruitTree(typeName, probe);
        s_ClassCache.Set(probe, fruitType);
        return fruitType != eBNB_SearchFruitType.NONE;
    }

    // One-time resolution per classname; result is memoised by IsFruitTree.
    private static int ClassifyFruitTree(string typeName, string probe)
    {
        int ft;
        if (s_KnownTrees.Find(probe, ft))
            return ft;
        if (FruitFromConfig(typeName, ft))
            return ft;
        // Genus scan is the forward-compat path; gate it so only trees qualify.
        if (probe.IndexOf("tree") == -1)
            return eBNB_SearchFruitType.NONE;
        if (FruitFromGenus(probe, ft))
            return ft;
        return eBNB_SearchFruitType.NONE;
    }

    // Some trees declare a fruitType string in CfgVehicles; map it to the enum.
    private static bool FruitFromConfig(string typeName, out int fruitType)
    {
        if (!GetGame()) return false;

        string cfgPath = "CfgVehicles " + typeName + " fruitType";
        if (!GetGame().ConfigIsExisting(cfgPath)) return false;

        string raw;
        GetGame().ConfigGetText(cfgPath, raw);
        raw.ToLower();
        return s_FruitNames.Find(raw, fruitType);
    }

    private static bool FruitFromGenus(string probe, out int fruitType)
    {
        array<string> genus = s_GenusFruit.GetKeyArray();
        for (int i = 0; i < genus.Count(); i++)
        {
            if (probe.Contains(genus[i]))
                return s_GenusFruit.Find(genus[i], fruitType);
        }
        return false;
    }

    // Returns 1 if the first find went into the player's hands, else 0.
    static int SpawnFruitInFront(PlayerBase player, string className, int count)
    {
        int toHands = 0;
        if (!GetGame() || !player) return toHands;

        BNB_SearchFruitConfigBlock b = BNB_SearchFruitSettings.Get();

        for (int i = 0; i < count; i++)
        {
            bool hands;
            Object o = BNB_SharedForageHelpers.SpawnForageItem(player, className, i == 0, hands);
            if (hands) toHands++;

            // Foraged fruit spawns worn-to-pristine; quantity tracks health
            // fraction (pristine = 100%) and nutrition scales with quantity.
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
