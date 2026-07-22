// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.1
// Modified:     2026-07-22
//
// BNB_Foraging_ActionConstructor.c - register the foraging action typenames.
// Prefetching settings here fires the JSON load once so the first scroll-wheel
// render hits a populated config.

modded class ActionConstructor
{
    override void RegisterActions(TTypenameArray actions)
    {
        super.RegisterActions(actions);
        actions.Insert(BNB_ActionSearchFruit);
        actions.Insert(BNB_ActionSearchMushrooms);
        actions.Insert(BNB_ActionSearchBerries);
        BNB_SearchFruitSettings.Get();
        BNB_SearchMushroomsSettings.Get();
        BNB_SearchBerriesSettings.Get();
        Print("[BNB_Foraging] foraging actions registered + settings prefetched");
    }
}
