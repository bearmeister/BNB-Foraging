// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.0
// Modified:     2026-07-19
//
// BNB_Foraging_MissionServer.c - server OnInit boot-load: prefetch foraging
// settings and load the persisted per-object cooldowns from disk.

modded class MissionServer
{
    override void OnInit()
    {
        super.OnInit();
        BNB_SearchFruitSettings.Get();
        BNB_SearchMushroomsSettings.Get();
        BNB_SearchBerriesSettings.Get();
        BNB_SharedForageCooldownRegistry.LoadFromDisk();
        Print("[BNB_Foraging] MissionServer.OnInit settings + cooldowns loaded");
    }
}
