// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "BlasterLobbyGameMode.generated.h"

UCLASS()
class BLASTER_API ABlasterLobbyGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    virtual void PostLogin(APlayerController* NewPlayer) override;
};
