// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.2.2
// Modified:     2026-07-22
//
// BNB_ForagingConfig.c - root JSON schema + loader for
// $profile:BNBForaging\foraging.json. Server-side load-once; writes defaults if
// the file is missing so a bare server boots working. Clients use defaults.

class BNB_ForagingConfig
{
    static const int VERSION_EXPECTED = 1;

    int version = -1;
    // 1 = emit per-search [BNB_Foraging::Debug] lines to the RPT. Load/boot
    // lines are always logged regardless.
    int debug_log = 0;
    ref BNB_SearchFruitConfigBlock     search_fruit     = new BNB_SearchFruitConfigBlock;
    ref BNB_SearchMushroomsConfigBlock search_mushrooms = new BNB_SearchMushroomsConfigBlock;
    ref BNB_SearchBerriesConfigBlock   search_berries   = new BNB_SearchBerriesConfigBlock;

    private static ref BNB_ForagingConfig s_Config;

    static const string DIR  = "$profile:\\BNBForaging";
    static const string PATH = "$profile:\\BNBForaging\\foraging.json";

    static BNB_ForagingConfig Get()
    {
        if (s_Config)
            return s_Config;

        s_Config = new BNB_ForagingConfig;
        // Clients keep built-in defaults (only the cosmetic action duration).
        if (!GetGame() || !GetGame().IsDedicatedServer())
            return s_Config;

        if (!FileExist(PATH))
        {
            if (!FileExist(DIR))
                MakeDirectory(DIR);
            s_Config.version = VERSION_EXPECTED;
            JsonFileLoader<BNB_ForagingConfig>.JsonSaveFile(PATH, s_Config);
            Print("[BNB_Foraging] foraging.json created with defaults");
            return s_Config;
        }

        BNB_ForagingConfig data = new BNB_ForagingConfig;
        string err;
        if (!JsonFileLoader<BNB_ForagingConfig>.LoadFile(PATH, data, err))
        {
            Print("[BNB_Foraging] foraging.json load FAILED (" + err + ") - using defaults");
            return s_Config;
        }
        if (data.version != VERSION_EXPECTED)
        {
            // Backup-then-adopt: loaded values are kept, the operator gets a
            // rollback copy of the pre-migration file.
            string bak = PATH + ".v" + data.version.ToString() + ".bak";
            CopyFile(PATH, bak);
            Print("[BNB_Foraging] foraging.json version " + data.version.ToString() + " -> " + VERSION_EXPECTED.ToString() + ", backup written to " + bak);
            data.version = VERSION_EXPECTED;
        }
        s_Config = data;
        // Re-save so keys added in a newer version appear with their defaults.
        JsonFileLoader<BNB_ForagingConfig>.JsonSaveFile(PATH, s_Config);
        Print("[BNB_Foraging] settings loaded from foraging.json");
        return s_Config;
    }

    static bool DebugEnabled()
    {
        return Get().debug_log != 0;
    }
}
