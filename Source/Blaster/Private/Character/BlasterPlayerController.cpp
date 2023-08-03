// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"

void ABlasterPlayerController::OnPossess(APawn* aPawn) 
{
    Super::OnPossess(aPawn);

    OnNewPawn.Broadcast(aPawn);
}
