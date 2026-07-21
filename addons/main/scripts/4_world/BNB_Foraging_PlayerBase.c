// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.0
// Modified:     2026-07-19
//
// BNB_Foraging_PlayerBase.c - attach the foraging cursor actions. Register
// (ActionConstructor) plus attach (here) are both required for a verb to appear.

modded class PlayerBase
{
    override void SetActions()
    {
        super.SetActions();
        AddAction(ActionSearchFruit);
        AddAction(ActionSearchMushrooms);
        AddAction(ActionSearchBerries);
    }
}
