// Blaster Multiplayer Game. All rights reserved.

#include "Menu/BlasterMenuPlayerController.h"

void ABlasterMenuPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetInputMode(FInputModeUIOnly());
    bShowMouseCursor = true;
}
