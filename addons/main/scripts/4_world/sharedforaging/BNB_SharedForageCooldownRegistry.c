// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.3.0
// Modified:     2026-07-20
//
// BNB_SharedForageCooldownRegistry.c - per-object forage cooldowns, shared by
// all three search verbs; in-memory map over a $profile append-only log.

class BNB_SharedForageCooldownRegistry
{
    private static ref map<string, int> s_Cooldowns;
    // Lines physically on disk, incl. superseded duplicates. Drives compaction.
    private static int s_DiskLines;
    // False once a read failed: the map no longer mirrors disk, so compaction
    // (a truncating rewrite) would destroy entries it never read.
    private static bool s_LoadOk = true;

    static const string COOLDOWN_DIR  = "$profile:\\BNBForaging";
    static const string COOLDOWN_FILE = "$profile:\\BNBForaging\\forage_cooldowns.txt";
    // Compact once the file carries this multiple of the live entry count.
    static const int COMPACT_RATIO = 2;
    static const int COMPACT_FLOOR = 64;

    static void Ensure()
    {
        if (!s_Cooldowns) s_Cooldowns = new map<string, int>();
    }

    // Decimetre-resolution per-object key (class + position rounded to 0.1 m).
    // All set/check/query sites derive the key here so the shape stays uniform.
    static string MakeKey(Object obj)
    {
        vector p = obj.GetPosition();
        int rx = Math.Round(p[0] * 10);
        int ry = Math.Round(p[1] * 10);
        int rz = Math.Round(p[2] * 10);
        return obj.GetType() + "_" + rx + "_" + ry + "_" + rz;
    }

    // Restart-stable clock: seconds since a fixed civil baseline from the real
    // system UTC clock (immune to in-game settime; small int, decades headroom).
    static int NowEpochSec()
    {
        int y, m, d, hh, mm, ss;
        GetYearMonthDayUTC(y, m, d);
        GetHourMinuteSecondUTC(hh, mm, ss);
        int days = DaysFromCivil(y, m, d) - DaysFromCivil(2026, 1, 1);
        return ((days * 24 + hh) * 60 + mm) * 60 + ss;
    }

    // Days from 1970-01-01 for a civil date (Hinnant, integer-only). Only fed
    // real UTC dates (year >= 2026), so all intermediates are positive (floor).
    static int DaysFromCivil(int y, int m, int d)
    {
        int yy = y;
        if (m <= 2) yy = yy - 1;
        int era = yy / 400;
        int yoe = yy - era * 400;
        int mp = m + 9;
        if (m > 2) mp = m - 3;
        int doy = (153 * mp + 2) / 5 + d - 1;
        int doe = yoe * 365 + yoe / 4 - yoe / 100 + doy;
        return era * 146097 + doe - 719468;
    }

    static int GetCooldown(string key)
    {
        Ensure();
        int v;
        if (s_Cooldowns.Find(key, v)) return v;
        return 0;
    }

    static void SetCooldown(string key, int expiry)
    {
        Ensure();
        s_Cooldowns.Set(key, expiry);
    }

    // Server-side path: in-memory update + O(1) append. Cost is one short line
    // regardless of registry size; CompactIfNeeded bounds the file growth.
    static void SetAndPersist(string key, int expiry)
    {
        SetCooldown(key, expiry);
        AppendToDisk(key, expiry);
        CompactIfNeeded();
    }

    private static void AppendToDisk(string key, int expiry)
    {
        if (!FileExist(COOLDOWN_DIR))
            MakeDirectory(COOLDOWN_DIR);

        // APPEND is not guaranteed to create a missing file; fall back to WRITE.
        FileHandle fh = OpenFile(COOLDOWN_FILE, FileMode.APPEND);
        if (!fh)
            fh = OpenFile(COOLDOWN_FILE, FileMode.WRITE);
        if (!fh)
        {
            Print("[BNB_Foraging] WARNING cooldown append failed - not persisted");
            return;
        }

        FPrint(fh, key + ": " + expiry.ToString() + "\n");
        CloseFile(fh);
        s_DiskLines++;
    }

    // Rewrite only once duplicates dominate, so the O(n) cost is amortised.
    private static void CompactIfNeeded()
    {
        Ensure();
        if (!s_LoadOk) return;
        int live = s_Cooldowns.Count();
        int threshold = live * COMPACT_RATIO;
        if (threshold < COMPACT_FLOOR) threshold = COMPACT_FLOOR;
        if (s_DiskLines > threshold)
            SaveToDisk();
    }

    static void GetAllKeys(out array<string> outKeys)
    {
        Ensure();
        outKeys = s_Cooldowns.GetKeyArray();
    }

    static void Clear()
    {
        if (s_Cooldowns) s_Cooldowns.Clear();
        s_DiskLines = 0;
    }

    // Drop entries whose expiry is at or before now (bounds the file). Two-pass
    // so the map is never mutated during its own key-array walk.
    private static void PruneExpired()
    {
        Ensure();
        int now = NowEpochSec();
        array<string> keys = s_Cooldowns.GetKeyArray();
        array<string> toRemove = new array<string>();
        for (int i = 0; i < keys.Count(); i++)
        {
            int v;
            if (s_Cooldowns.Find(keys[i], v) && v <= now)
                toRemove.Insert(keys[i]);
        }
        for (int j = 0; j < toRemove.Count(); j++)
            s_Cooldowns.Remove(toRemove[j]);
    }

    static void LoadFromDisk()
    {
        Ensure();
        s_Cooldowns.Clear();
        s_DiskLines = 0;
        s_LoadOk = true;

        if (!FileExist(COOLDOWN_DIR))
            MakeDirectory(COOLDOWN_DIR);

        if (!FileExist(COOLDOWN_FILE))
            return;

        FileHandle fh = OpenFile(COOLDOWN_FILE, FileMode.READ);
        if (!fh)
        {
            // Unread entries still on disk: block the truncating rewrite.
            s_LoadOk = false;
            Print("[BNB_Foraging] WARNING cooldown read failed - compaction disabled this session");
            return;
        }

        // Last occurrence of a key wins. Terminate on -1, not 0: a zero-length
        // line returns 0 and would truncate the read.
        string line;
        while (FGets(fh, line) != -1)
        {
            s_DiskLines++;
            line = line.Trim();
            if (line.Length() == 0) continue;
            int colon = line.IndexOf(":");
            if (colon == -1) continue;
            string key = line.Substring(0, colon).Trim();
            string rhs = line.Substring(colon + 1, line.Length() - colon - 1).Trim();
            if (rhs.Length() > 0 && rhs.Substring(rhs.Length() - 1, 1) == ",")
                rhs = rhs.Substring(0, rhs.Length() - 1).Trim();
            int val = rhs.ToInt();
            s_Cooldowns.Set(key, val);
        }
        CloseFile(fh);

        PruneExpired();
        // Boot is the natural compaction point: drops expired + duplicate lines.
        SaveToDisk();
    }

    // Full rewrite = the compaction path. Not called per-search any more.
    static void SaveToDisk()
    {
        Ensure();
        PruneExpired();

        if (!FileExist(COOLDOWN_DIR))
            MakeDirectory(COOLDOWN_DIR);

        FileHandle fh = OpenFile(COOLDOWN_FILE, FileMode.WRITE);
        if (!fh)
        {
            Print("[BNB_Foraging] WARNING cooldown compaction failed - log left uncompacted");
            return;
        }

        array<string> keys = s_Cooldowns.GetKeyArray();
        for (int i = 0; i < keys.Count(); i++)
        {
            string k = keys[i];
            int v;
            s_Cooldowns.Find(k, v);
            FPrint(fh, k + ": " + v.ToString() + "\n");
        }
        CloseFile(fh);
        s_DiskLines = keys.Count();
    }
}
