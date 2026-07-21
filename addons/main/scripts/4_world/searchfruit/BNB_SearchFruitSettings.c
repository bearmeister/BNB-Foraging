// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.3.4
// Modified:     2026-07-21
//
// BNB_SearchFruitSettings.c - search-fruit config block + loader. Reads the
// search_fruit block from BNB_ForagingConfig ($profile:BNBForaging\foraging.json),
// prefetched at MissionServer::OnInit.

class BNB_SearchFruitConfigBlock
{
    float apple_drop_chance      = 0.30;
    float plum_drop_chance       = 0.30;
    float pear_drop_chance       = 0.30;
    float search_duration_seconds = 10.0;
    int   tree_cooldown_minutes  = 60;
    int   apple_min_count        = 1;
    int   apple_max_count        = 2;
    int   plum_min_count         = 1;
    int   plum_max_count         = 2;
    int   pear_min_count         = 1;
    int   pear_max_count         = 2;
    int   food_stage_enable      = 1;
    float food_dried_chance      = 0.0;
    float food_rotten_chance     = 0.10;
    float cut_chance_no_gloves   = 0.10;
    float glove_damage           = 4.0;
}

class BNB_SearchFruitSettings
{
    private static ref BNB_SearchFruitConfigBlock s_Block;

    static BNB_SearchFruitConfigBlock Get()
    {
        if (!s_Block)
            Load();
        return s_Block;
    }

    private static void Load()
    {
        if (!GetGame().IsDedicatedServer())
        {
            // Clients use built-in defaults for the cosmetic action duration.
            s_Block = new BNB_SearchFruitConfigBlock;
            return;
        }
        s_Block = BNB_ForagingConfig.Get().search_fruit;
        Clamp(s_Block);
        Print("[BNB_SearchFruit] settings loaded (search=" + s_Block.search_duration_seconds + "s cooldown=" + s_Block.tree_cooldown_minutes + "m)");
    }

    private static void Clamp(BNB_SearchFruitConfigBlock b)
    {
        b.apple_drop_chance  = Math.Clamp(b.apple_drop_chance,  0.0, 1.0);
        b.plum_drop_chance   = Math.Clamp(b.plum_drop_chance,   0.0, 1.0);
        b.pear_drop_chance   = Math.Clamp(b.pear_drop_chance,   0.0, 1.0);
        if (b.search_duration_seconds < 0.5)  b.search_duration_seconds = 0.5;
        if (b.search_duration_seconds > 60.0) b.search_duration_seconds = 60.0;
        if (b.tree_cooldown_minutes < 0)     b.tree_cooldown_minutes  = 0;
        if (b.tree_cooldown_minutes > 10080) b.tree_cooldown_minutes  = 10080;
        ClampPair(b.apple_min_count, b.apple_max_count);
        ClampPair(b.plum_min_count,  b.plum_max_count);
        ClampPair(b.pear_min_count,  b.pear_max_count);
        b.food_dried_chance  = Math.Clamp(b.food_dried_chance,  0.0, 1.0);
        b.food_rotten_chance = Math.Clamp(b.food_rotten_chance, 0.0, 1.0);
        b.cut_chance_no_gloves = Math.Clamp(b.cut_chance_no_gloves, 0.0, 1.0);
        b.glove_damage         = Math.Clamp(b.glove_damage,         0.0, 100.0);
    }

    private static void ClampPair(inout int minC, inout int maxC)
    {
        if (minC < 0)  minC = 0;
        if (maxC < 0)  maxC = 0;
        if (minC > 50) minC = 50;
        if (maxC > 50) maxC = 50;
        if (minC > maxC)
        {
            int t = minC;
            minC = maxC;
            maxC = t;
        }
    }

    static float ChanceFor(int fruitType)
    {
        BNB_SearchFruitConfigBlock b = Get();
        if (!b) return 0.0;
        switch (fruitType)
        {
            case eBNB_SearchFruitType.APPLE: return b.apple_drop_chance;
            case eBNB_SearchFruitType.PLUM:  return b.plum_drop_chance;
            case eBNB_SearchFruitType.PEAR:  return b.pear_drop_chance;
        }
        return 0.0;
    }

    static void CountRangeFor(int fruitType, out int minC, out int maxC)
    {
        BNB_SearchFruitConfigBlock b = Get();
        if (!b)
        {
            minC = 0; maxC = 0;
            return;
        }
        switch (fruitType)
        {
            case eBNB_SearchFruitType.APPLE: minC = b.apple_min_count; maxC = b.apple_max_count; return;
            case eBNB_SearchFruitType.PLUM:  minC = b.plum_min_count;  maxC = b.plum_max_count;  return;
            case eBNB_SearchFruitType.PEAR:  minC = b.pear_min_count;  maxC = b.pear_max_count;  return;
        }
        minC = 0; maxC = 0;
    }
}
