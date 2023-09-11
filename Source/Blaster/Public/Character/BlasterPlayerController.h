// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"

#include "BlasterPlayerController.generated.h"

class ABlasterBaseWeapon;
class ABlasterGameState;
class ABlasterPlayerState;

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
    virtual void OnUnPossess() override;

public:
    float GetTimerTime();
    void SetMatchState(FName State);
    void SetCurrentWeapon() { CurrentWeapon = nullptr; }
    void SetCurrentWeapon(ABlasterBaseWeapon* Weapon) { CurrentWeapon = Weapon; }

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

    void SetTimers();

    float LevelStartTime{0.0f};

    UPROPERTY(Replicated)
    int32 MatchDuration{0};

    UPROPERTY(Replicated)
    int32 CooldownDuration{0};

    float ClientServerDelta{0.0f};

    UPROPERTY(Category = "Time", EditDefaultsOnly)
    float TimeSyncFrequency{5.0f};

    float TimeSyncRunningTime{0.0f};

    void CheckTimeSync(float DeltaTime);

    float CountdownTime{0.0f};
    float WarmupStartTime{0.0f};
    float MatchStartTime{0.0f};
    float CooldownStartTime{0.0f};

    UPROPERTY(ReplicatedUsing = OnRep_MatchState)
    FName MatchState;

    UFUNCTION()
    void OnRep_MatchState();

    void HandleMatchState();
    void HandleWaitingToStart();
    void HandleMatchHasStarted();
    void HandleCooldown();

    void ShowAnnouncement();
    void ShowCooldown();

    float GetLeftMatchTime();
    float GetLeftWarmupTime();
    float GetLeftCooldownTime();

    FText CooldownInfoText{FText()};

    void UpdateTopScoreText();

    ABlasterBaseWeapon* CurrentWeapon{nullptr};
};
