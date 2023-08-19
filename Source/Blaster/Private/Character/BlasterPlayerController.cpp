// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerController.h"

#include "Engine/World.h"
#include "Net/UnrealNetwork.h"

#include "GameMode/BlasterGameMode.h"
#include "HUD/BlasterHUD.h"

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlasterPlayerController, MatchDuration);
    DOREPLIFETIME(ABlasterPlayerController, MatchState);
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

void ABlasterPlayerController::BeginPlay()
{
    Super::BeginPlay();

    SetMatchTime();
}

void ABlasterPlayerController::OnPossess(APawn* aPawn)
{
    Super::OnPossess(aPawn);

    OnNewPawn.Broadcast(aPawn);
}

void ABlasterPlayerController::SetMatchTime_Implementation()
{
    if (auto GameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()))
    {
        MatchDuration = GameMode->GetMatchTime();
    }
}

int32 ABlasterPlayerController::GetLeftMatchTime()
{
    return FMath::Max(0, MatchDuration - GetServerTime());
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
    ClientReportServerTime(TimeOfClientRequest, GetWorld()->GetTimeSeconds());
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeOfServerReceivdClient)
{
    const float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
    const float CurrentServerTime = TimeOfServerReceivdClient + RoundTripTime * 0.5f;
    ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime() const
{
    if (HasAuthority())
    {
        return GetWorld()->GetTimeSeconds();
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
    if (MatchState == MatchState::InProgress)
    {
        if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
        {
            BlasterHUD->AddCharacterOverlay();
        }
    }
}
