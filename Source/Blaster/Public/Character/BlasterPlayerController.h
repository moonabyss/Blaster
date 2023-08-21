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
    float GetLeftMatchTime();
    float GetLeftWarmupTime();
    void MatchStateSet(FName State);

protected:
    // Request the current server time
    UFUNCTION(Server, Reliable)
    void ServerRequestServerTime(float TimeOfClientRequest);

    // Reports the current server time to the client
    UFUNCTION(Client, Reliable)
    void ClientReportServerTime(float TimeOfClientRequest, float TimeOfServerReceivedClient);

    float GetServerTime() const;

private:
    UPROPERTY(Replicated)
    int32 WarmupDuration{0};

    UFUNCTION(Server, Reliable)
    void SetTimers();

    float LevelStartTime{0.0f};

    UPROPERTY(Replicated)
    int32 MatchDuration{0};

    float ClientServerDelta{0.0f};

    UPROPERTY(Category = "Time", EditDefaultsOnly)
    float TimeSyncFrequency{5.0f};

    float TimeSyncRunningTime{0.0f};

    void CheckTimeSync(float DeltaTime);

    UPROPERTY(ReplicatedUsing = OnRep_MatchState)
    FName MatchState;

    UFUNCTION()
    void OnRep_MatchState();

    void HandleMatchState();
    void HandleMatchHasStarted();
    void HandleCooldown();

    void ShowAnnouncement();
};
