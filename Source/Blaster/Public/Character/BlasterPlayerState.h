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
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
    void AddToScore(float ScoreAmount);
    void AddToKilled(int NumOfKilled);
    void IncrementDefeats();

    int32 GetKilled() const { return Killed; }
    int32 GetDefeats() const { return Defeats; }

private:
    UPROPERTY(ReplicatedUsing = OnRep_Killed)
    int32 Killed{0};

    UFUNCTION()
    void OnRep_Killed();

    UPROPERTY(ReplicatedUsing = OnRep_Defeats)
    int32 Defeats{0};

    UFUNCTION()
    void OnRep_Defeats();
};
