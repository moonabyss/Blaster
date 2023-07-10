// Blaster Multiplayer Game. All rights reserved.

#include "Menu/BlasterMenuGameMode.h"
#include "Menu/BlasterMenuPlayerController.h"

ABlasterMenuGameMode::ABlasterMenuGameMode()
{
    PlayerControllerClass = ABlasterMenuPlayerController::StaticClass();
}
