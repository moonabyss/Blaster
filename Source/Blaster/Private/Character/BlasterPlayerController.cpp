// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerController.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "GameMode/BlasterGameMode.h"
#include "HUD/BlasterHUD.h"

const FText AnnouncementText = FText::FromString("Fly around: W A S D");

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlasterPlayerController, MatchDuration);
    DOREPLIFETIME(ABlasterPlayerController, WarmupDuration);
    DOREPLIFETIME(ABlasterPlayerController, CooldownDuration);
    DOREPLIFETIME(ABlasterPlayerController, MatchState);
}

void ABlasterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        LevelStartTime = GetWorld()->GetTimeSeconds();
        if (auto GameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()))
        {
            MatchState = GameMode->GetMatchState();
        }
    }
    SetTimers();
    ShowAnnouncement(AnnouncementText);
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckTimeSync(DeltaTime);
}

void ABlasterPlayerController::ReceivedPlayer()
{
    Super::ReceivedPlayer();

    if (IsLocalController())
    {
        ServerRequestServerTime(GetWorld()->GetTimeSeconds());
    }
}

void ABlasterPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    OnNewPawn.Broadcast(aPawn);
}

void ABlasterPlayerController::SetTimers_Implementation()
{
    if (auto GameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()))
    {
        MatchDuration = GameMode->GetMatchTime();
        WarmupDuration = GameMode->GetWarmupTime();
        CooldownDuration = GameMode->GetCooldownTime();
    }
}

float ABlasterPlayerController::GetLeftMatchTime()
{
    return FMath::Max(0, MatchDuration + WarmupDuration - GetServerTime());
}

float ABlasterPlayerController::GetLeftWarmupTime()
{
    return FMath::Max(0, WarmupDuration - GetServerTime());
}

float ABlasterPlayerController::GetLeftCooldownTime()
{
    return FMath::Max(0, MatchDuration + WarmupDuration + CooldownDuration - GetServerTime());
}

float ABlasterPlayerController::GetTimerTime()
{
    if (MatchState == MatchState::WaitingToStart)
    {
        return GetLeftWarmupTime();
    }
    else if (MatchState == MatchState::InProgress)
    {
        return GetLeftMatchTime();
    }
    else if (MatchState == MatchState::Cooldown)
    {
        return GetLeftCooldownTime();
    }
    return 0.0f;
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
    ClientReportServerTime(TimeOfClientRequest, GetWorld()->GetTimeSeconds() - LevelStartTime);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeOfServerReceivedClient)
{
    const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
    const float CurrentServerTime = TimeOfServerReceivedClient + RoundTripTime * 0.5f;
    ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime() const
{
    if (HasAuthority())
    {
        return GetWorld()->GetTimeSeconds() - LevelStartTime;
    }

    return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
    TimeSyncRunningTime += DeltaTime;
    if (IsLocalController() && TimeSyncRunningTime > TimeSyncFrequency)
    {
        ServerRequestServerTime(GetWorld()->GetTimeSeconds());
        TimeSyncRunningTime = 0.0f;
    }
}

void ABlasterPlayerController::MatchStateSet(FName State)
{
    MatchState = State;

    HandleMatchState();
}

void ABlasterPlayerController::OnRep_MatchState()
{
    HandleMatchState();
}

void ABlasterPlayerController::HandleMatchState()
{
    if (MatchState == MatchState::WaitingToStart)
    {
    }
    else if (MatchState == MatchState::InProgress)
    {
        HandleMatchHasStarted();
    }
    else if (MatchState == MatchState::Cooldown)
    {
        HandleCooldown();
    }
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        BlasterHUD->AddCharacterOverlay();
        BlasterHUD->RemoveAnnouncement();
    }
}

void ABlasterPlayerController::HandleCooldown()
{
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        BlasterHUD->RemoveCharacterOverlay();
        ShowAnnouncement(FText());
    }
}

void ABlasterPlayerController::ShowAnnouncement(FText Text)
{
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        BlasterHUD->AddAnnouncement(Text);
    }
}
