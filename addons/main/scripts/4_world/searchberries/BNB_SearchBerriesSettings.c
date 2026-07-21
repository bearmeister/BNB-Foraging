// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.1.2
// Modified:     2026-07-21
//
// BNB_SearchBerriesSettings.c - search-berries config block + loader. Reads the
// search_berries block from BNB_ForagingConfig ($profile:BNBForaging\foraging.json),
// prefetched at MissionServer::OnInit. Clients use built-in defaults.

class BNB_SearchBerriesConfigBlock
{
    float berry_find_chance       = 0.70;
    float search_duration_seconds = 10.0;
    int   bush_cooldown_minutes   = 30;
    int   berry_min_count         = 1;
    int   berry_max_count         = 3;
    int   food_stage_enable       = 1;
    float food_dried_chance       = 0.0;
    float food_rotten_chance      = 0.10;
    float cut_chance_no_gloves    = 0.15;
    float glove_damage            = 4.0;
}

class BNB_SearchBerriesSettings
{
    private static ref BNB_SearchBerriesConfigBlock s_Block;

    static BNB_SearchBerriesConfigBlock Get()
    {
        if (!s_Block)
            Load();
        return s_Block;
    }

    private static void Load()
    {
        if (!GetGame().IsDedicatedServer())
        {
            s_Block = new BNB_SearchBerriesConfigBlock;
            return;
        }
        s_Block = BNB_ForagingConfig.Get().search_berries;
        Clamp(s_Block);
        Print("[BNB_SearchBerries] settings loaded (search=" + s_Block.search_duration_seconds + "s cooldown=" + s_Block.bush_cooldown_minutes + "m)");
    }

    private static void Clamp(BNB_SearchBerriesConfigBlock b)
    {
        b.berry_find_chance = Math.Clamp(b.berry_find_chance, 0.0, 1.0);
        if (b.search_duration_seconds < 0.5)  b.search_duration_seconds = 0.5;
        if (b.search_duration_seconds > 60.0) b.search_duration_seconds = 60.0;
        if (b.bush_cooldown_minutes < 0)     b.bush_cooldown_minutes  = 0;
        if (b.bush_cooldown_minutes > 10080) b.bush_cooldown_minutes  = 10080;
        if (b.berry_min_count < 0)  b.berry_min_count = 0;
        if (b.berry_max_count < 0)  b.berry_max_count = 0;
        if (b.berry_min_count > 50) b.berry_min_count = 50;
        if (b.berry_max_count > 50) b.berry_max_count = 50;
        if (b.berry_min_count > b.berry_max_count)
        {
            int t = b.berry_min_count;
            b.berry_min_count = b.berry_max_count;
            b.berry_max_count = t;
        }
        b.food_dried_chance  = Math.Clamp(b.food_dried_chance,  0.0, 1.0);
        b.food_rotten_chance = Math.Clamp(b.food_rotten_chance, 0.0, 1.0);
        b.cut_chance_no_gloves = Math.Clamp(b.cut_chance_no_gloves, 0.0, 1.0);
        b.glove_damage         = Math.Clamp(b.glove_damage,         0.0, 100.0);
    }
}
