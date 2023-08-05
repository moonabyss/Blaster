// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"

#include "BlasterPlayerState.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

UCLASS()
class BLASTER_API ABlasterPlayerState : public APlayerState
{
    GENERATED_BODY()

public:
    virtual void OnRep_Score() override;

public:
    void AddToScore(float ScoreAmount);
};
