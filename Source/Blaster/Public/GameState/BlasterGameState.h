// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"

#include "BlasterGameState.generated.h"

class ABlasterPlayerState;

UCLASS()
class BLASTER_API ABlasterGameState : public AGameState
{
    GENERATED_BODY()

public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    void UpdateTopScore(ABlasterPlayerState* ScoringPlayer);

    UPROPERTY(Replicated)
    TArray<ABlasterPlayerState*> TopScoringPlayers;

private:
    int32 TopScore{0};
};
