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
    virtual void Tick(float DeltaTime) override;
    virtual void ReceivedPlayer() override;

protected:
    virtual void BeginPlay() override;
    virtual void OnPossess(APawn* aPawn) override;

public:
    int32 GetLeftMatchTime();

protected:
    // Request the current server time
    UFUNCTION(Server, Reliable)
    void ServerRequestServerTime(float TimeOfClientRequest);

    // Reports the current server time to the client
    UFUNCTION(Client, Reliable)
    void ClientReportServerTime(float TimeOfClientRequest, float TimeOfServerReceivdClient);

    float GetServerTime() const;

private:
    UFUNCTION(Server, Reliable)
    void SetMatchTime();

    UPROPERTY(Replicated)
    int32 MatchDuration{0};

    float ClientServerDelta{0.0f};

    UPROPERTY(Category = "Time", EditDefaultsOnly)
    float TimeSyncFrequency{5.0f};

    float TimeSyncRunningTime{0.0f};

    void CheckTimeSync(float DeltaTime);
};
