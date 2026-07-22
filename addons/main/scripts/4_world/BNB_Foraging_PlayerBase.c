// Organisation: Bullets'n'Bandages
// Author:       Bushy <contact@bushy.dev>
// Version:      v1.0.1
// Modified:     2026-07-22
//
// BNB_Foraging_PlayerBase.c - attach the foraging cursor actions. Register
// (ActionConstructor) plus attach (here) are both required for a verb to appear.

modded class PlayerBase
{
    override void SetActions(out TInputActionMap InputActionMap)
    {
        super.SetActions(InputActionMap);
        AddAction(BNB_ActionSearchFruit, InputActionMap);
        AddAction(BNB_ActionSearchMushrooms, InputActionMap);
        AddAction(BNB_ActionSearchBerries, InputActionMap);
    }
}
