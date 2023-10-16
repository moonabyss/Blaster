// Blaster Multiplayer Game. All rights reserved.

#include "GameMode/BlasterLobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

namespace
{
TMap<FString, FString> Levels{
    {"FreeForAll", "/Game/Maps/BlasterGame"}  //
};
}

void ABlasterLobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
    Super::PostLogin(NewPlayer);

    UWorld* World = GetWorld();
    if (!World) return;

    if (!GameState) return;

    int32 NumberOfPlayers = GameState->PlayerArray.Num();
    if (NumberOfPlayers == 2)
    {
        bUseSeamlessTravel = true;
        World->ServerTravel(Levels["FreeForAll"]);
    }
}
