// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.1.2
// Modified:     2026-07-21
//
// config.cpp - @BNB-Foraging client+server mod. Adds "Search for fruit" (trees),
// "Search for mushrooms" (stumps + logs) and "Search for berries" (bushes).
// Server-authoritative spawns + per-object cooldowns; config in
// $profile:BNBForaging\foraging.json.

class CfgPatches
{
    class BNB_Foraging
    {
        units[] = {};
        weapons[] = {};
        requiredVersion = 0.1;
        requiredAddons[] = { "DZ_Data", "DZ_Scripts" };
        version = "1.4.4";
    };
};

class CfgMods
{
    class BNB_Foraging
    {
        dir = "BNBForaging";
        name = "BNB-Foraging";
        author = "Bushy";
        type = "mod";
        hidePicture = 0;
        hideName = 0;
        tooltip = "Foraging: search trees, stumps and bushes for food";
        overview = "Adds three foraging actions - Search for fruit (apple/plum/pear trees), Search for mushrooms (stumps and fallen logs) and Search for berries (rose hip and elderberry bushes). The first find lands in your hands if they are free; anything more scatters on the ground where you searched. Server-authoritative with per-object cooldowns; tuned via $profile:BNBForaging\\foraging.json.";
        dependencies[] = { "Game", "World", "Mission" };
        class defs
        {
            class worldScriptModule
            {
                value = "";
                files[] = { "BNBForaging/scripts/4_world" };
            };
            class missionScriptModule
            {
                value = "";
                files[] = { "BNBForaging/scripts/5_mission" };
            };
        };
    };
};
