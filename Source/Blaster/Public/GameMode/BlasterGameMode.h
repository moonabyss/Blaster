// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

namespace MatchState
{
extern BLASTER_API const FName Cooldown;  // Match duration has been reached. Display winner and begin cooldown timer.
}

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    ABlasterGameMode();
    virtual void Tick(float DeltaSeconds) override;

protected:
    virtual void BeginPlay() override;
    virtual void OnMatchStateSet() override;

public:
    virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
    virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

    /** Duration of match for this game mode */
    float GetMatchTime() const { return MatchTime; }
    float GetWarmupTime() const { return WarmupTime; }
    float GetCooldownTime() const { return CooldownTime; }

private:
    UPROPERTY(Category = "Game Rules", EditDefaultsOnly)
    float WarmupTime{10.0f};

    UPROPERTY(Category = "Game Rules", EditDefaultsOnly)
    float MatchTime{120.0f};

    UPROPERTY(Category = "Game Rules", EditDefaultsOnly)
    float CooldownTime{10.0f};

    float CountdownTime{0.0f};

    float LevelStartingTime{0.0f};
};
