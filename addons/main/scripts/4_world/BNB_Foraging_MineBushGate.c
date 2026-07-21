// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.0
// Modified:     2026-07-20
//
// BNB_Foraging_MineBushGate.c - suppress vanilla bush-mining ("gather long stick",
// ActionMineBush) on berry bushes so "Search for berries" is the only option there.
// Every other bush keeps normal stick-mining. Returning false before super also
// fires on the server, so a client can't mine a berry bush either.

modded class ActionMineBush
{
    override bool ActionCondition(PlayerBase player, ActionTarget target, ItemBase item)
    {
        Object obj = target.GetObject();
        string berryClass;
        if (obj && BNB_SearchBerriesHelpers.IsBerryBush(obj, berryClass))
            return false;
        return super.ActionCondition(player, target, item);
    }
}
