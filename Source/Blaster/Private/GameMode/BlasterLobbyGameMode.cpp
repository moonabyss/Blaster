// Blaster Multiplayer Game. All rights reserved.

#include "GameMode/BlasterLobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ABlasterLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    int32 NumberOfPlayers = GameState->PlayerArray.Num();
    if (NumberOfPlayers == 2)
    {
        if (UWorld* World = GetWorld())
        {
            bUseSeamlessTravel = true;
            World->ServerTravel(FString("/Game/Maps/BlasterGame"));
        }
    }
}
