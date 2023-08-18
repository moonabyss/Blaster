// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BlasterPlayerController.generated.h"

UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
    GENERATED_BODY()


public:
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* aPawn) override;

public:
    int32 GetLeftMatchTime();

private:
    UFUNCTION(Server, Reliable)
    void SetMatchTime();

    UPROPERTY(Replicated)
    int32 MatchDuration{0};
};
