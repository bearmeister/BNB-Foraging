// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.1.4
// Modified:     2026-07-21
//
// BNB_SearchMushroomsSettings.c - search-mushrooms config block + loader. Reads the
// search_mushrooms block from BNB_ForagingConfig ($profile:BNBForaging\foraging.json),
// prefetched at MissionServer::OnInit. Clients use built-in defaults.

class BNB_SearchMushroomsConfigBlock
{
    float mushroom_find_chance    = 0.50;
    float search_duration_seconds = 10.0;
    int   stump_cooldown_minutes  = 60;
    int   mushroom_min_count      = 1;
    int   mushroom_max_count      = 3;
    int   food_stage_enable       = 1;
    float food_dried_chance       = 0.0;
    float food_rotten_chance      = 0.10;
    float cut_chance_no_gloves    = 0.05;
    float glove_damage            = 4.0;
}

class BNB_SearchMushroomsSettings
{
    private static ref BNB_SearchMushroomsConfigBlock s_Block;

    static BNB_SearchMushroomsConfigBlock Get()
    {
        if (!s_Block)
            Load();
        return s_Block;
    }

    private static void Load()
    {
        if (!GetGame().IsDedicatedServer())
        {
            s_Block = new BNB_SearchMushroomsConfigBlock;
            return;
        }
        s_Block = BNB_ForagingConfig.Get().search_mushrooms;
        Clamp(s_Block);
        Print("[BNB_SearchMushrooms] settings loaded (search=" + s_Block.search_duration_seconds + "s cooldown=" + s_Block.stump_cooldown_minutes + "m)");
    }

    private static void Clamp(BNB_SearchMushroomsConfigBlock b)
    {
        b.mushroom_find_chance = Math.Clamp(b.mushroom_find_chance, 0.0, 1.0);
        if (b.search_duration_seconds < 0.5)  b.search_duration_seconds = 0.5;
        if (b.search_duration_seconds > 60.0) b.search_duration_seconds = 60.0;
        if (b.stump_cooldown_minutes < 0)     b.stump_cooldown_minutes  = 0;
        if (b.stump_cooldown_minutes > 10080) b.stump_cooldown_minutes  = 10080;
        if (b.mushroom_min_count < 0)  b.mushroom_min_count = 0;
        if (b.mushroom_max_count < 0)  b.mushroom_max_count = 0;
        if (b.mushroom_min_count > 50) b.mushroom_min_count = 50;
        if (b.mushroom_max_count > 50) b.mushroom_max_count = 50;
        if (b.mushroom_min_count > b.mushroom_max_count)
        {
            int t = b.mushroom_min_count;
            b.mushroom_min_count = b.mushroom_max_count;
            b.mushroom_max_count = t;
        }
        b.food_dried_chance  = Math.Clamp(b.food_dried_chance,  0.0, 1.0);
        b.food_rotten_chance = Math.Clamp(b.food_rotten_chance, 0.0, 1.0);
        b.cut_chance_no_gloves = Math.Clamp(b.cut_chance_no_gloves, 0.0, 1.0);
        b.glove_damage         = Math.Clamp(b.glove_damage,         0.0, 100.0);
    }
}
