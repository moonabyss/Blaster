// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"

#include "BlasterGameMode.generated.h"

class ABlasterCharacter;
class ABlasterPlayerController;

UCLASS()
class BLASTER_API ABlasterGameMode : public AGameMode
{
    GENERATED_BODY()

public:
    ABlasterGameMode();
    virtual void Tick(float DeltaSeconds) override;
    virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
    virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

protected:
    virtual void BeginPlay() override;
    virtual void OnMatchStateSet() override;

public:
    /** Duration of match for this game mode */
    int32 GetMatchTime() const { return MatchTime; }

private:
    UPROPERTY(Category = "Game Rules", EditDefaultsOnly, meta = (Units = "s"))
    int32 MatchTime;

    UPROPERTY(Category = "Timers", EditDefaultsOnly)
    float WarmupTime{10.0f};

    UPROPERTY(Category = "Timers", EditDefaultsOnly)
    float CountdownTime{0.0f};

    float LevelStartingTime{0.0f};
};
