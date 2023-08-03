// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerController.h"
#include "HUD/BlasterHUD.h"

void ABlasterPlayerController::InitHUD() 
{
    UE_LOG(LogTemp, Warning, TEXT("InitHUD()"));
    if (auto BlasterHUD = Cast<ABlasterHUD>(MyHUD))
    {
        BlasterHUD->Init();
        UE_LOG(LogTemp, Warning, TEXT("InitHUD() cast"));
    }
}
