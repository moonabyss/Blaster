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
    virtual void PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController);
    virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);

public:
    /** Duration of match for this game mode */
    int32 GetMatchTime() const { return MatchTime; }

private:
    UPROPERTY(Category = "Game Rules", EditDefaultsOnly, meta = (Units = "s"))
    int32 MatchTime;
};
