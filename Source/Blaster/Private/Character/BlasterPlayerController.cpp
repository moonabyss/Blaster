// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerController.h"

#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"

#include "Character/BlasterCharacter.h"
#include "Character/BlasterPlayerState.h"
#include "GameMode/BlasterGameMode.h"
#include "GameState/BlasterGameState.h"
#include "HUD/BlasterHUD.h"

const FText AnnouncementTitleText = FText::FromString("Match starts in:");
const FText AnnouncementInfoText = FText::FromString("Fly around: W A S D");
const FText CooldownTitleText = FText::FromString("Next match starts in:");

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
        SetTimers();
    }

    BlasterGameState = Cast<ABlasterGameState>(GetWorld()->GetGameState());

    ShowAnnouncement();
}

void ABlasterPlayerController::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    CheckTimeSync(DeltaTime);
}

void ABlasterPlayerController::ReceivedPlayer()
{
    Super::ReceivedPlayer();

    WarmupStartTime = GetWorld()->GetTimeSeconds();
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

void ABlasterPlayerController::SetTimers()
{
    if (auto GameMode = Cast<ABlasterGameMode>(GetWorld()->GetAuthGameMode()))
    {
        MatchDuration = GameMode->GetMatchTime();
        WarmupDuration = GameMode->GetWarmupTime();
        CooldownDuration = GameMode->GetCooldownTime();
    }
}

float ABlasterPlayerController::GetLeftWarmupTime()
{
    if (BlasterGameState)
    {
        return FMath::Max(0, CountdownTime - BlasterGameState->GetServerWorldTimeSeconds());
    }
    return FMath::Max(0, CountdownTime - GetServerTime());
}

float ABlasterPlayerController::GetLeftMatchTime()
{
    //return MatchDuration + MatchStartTime - GetServerTime();
    return FMath::Max(0, CountdownTime + MatchStartTime - GetServerTime());
}

float ABlasterPlayerController::GetLeftCooldownTime()
{
    return FMath::Max(0, CountdownTime + CooldownStartTime - GetServerTime());
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

void ABlasterPlayerController::SetMatchState(FName State)
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
        HandleWaitingToStart();
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

void ABlasterPlayerController::HandleWaitingToStart() 
{
    CountdownTime = WarmupDuration;
    WarmupStartTime = GetServerTime();
    SetInputMode(FInputModeGameOnly());
    bShowMouseCursor = false;
}

void ABlasterPlayerController::HandleMatchHasStarted()
{
    CountdownTime = MatchDuration;
    MatchStartTime = GetServerTime();
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        BlasterHUD->AddCharacterOverlay();
        BlasterHUD->RemoveAnnouncement();
    }
}

void ABlasterPlayerController::HandleCooldown()
{
    CountdownTime = CooldownDuration;
    CooldownStartTime = GetServerTime();
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        if (auto BlasterCharacter = Cast<ABlasterCharacter>(GetCharacter()))
        {
            BlasterCharacter->DisableInput(this);
            BlasterCharacter->GetCharacterMovement()->DisableMovement();
            BlasterCharacter->GetCharacterMovement()->StopMovementImmediately();
            BlasterCharacter->StopFire();
        }

        UpdateTopScoreText();

        BlasterHUD->RemoveCharacterOverlay();
        ShowCooldown();

        SetInputMode(FInputModeUIOnly());
        bShowMouseCursor = true;

    }
}

void ABlasterPlayerController::UpdateTopScoreText() 
{
    auto BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();

    if (BlasterGameState && BlasterPlayerState)
    {
        FString InfoText;
        if (BlasterGameState->TopScoringPlayers.Num() == 0)
        {
            InfoText = FString("There is no winner.");
        }
        else if (BlasterGameState->TopScoringPlayers.Num() == 1 && BlasterGameState->TopScoringPlayers[0] == BlasterPlayerState)
        {
            InfoText = FString("You are the winner!");
        }
        else if (BlasterGameState->TopScoringPlayers.Num() == 1)
        {
            InfoText = FString::Printf(TEXT("Winner:\n%s"), *BlasterGameState->TopScoringPlayers[0]->GetPlayerName());
        }
        else if (BlasterGameState->TopScoringPlayers.Num() > 1)
        {
            InfoText = FString("Players tied for the win:\n");
            for (auto TiedPlayer : BlasterGameState->TopScoringPlayers)
            {
                InfoText.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
            }
        }
        CooldownInfoText = FText::FromString(InfoText);
    }
}

void ABlasterPlayerController::ShowAnnouncement()
{
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        BlasterHUD->AddAnnouncement(AnnouncementTitleText, AnnouncementInfoText);
    }
}

void ABlasterPlayerController::ShowCooldown()
{
    if (auto BlasterHUD = Cast<ABlasterHUD>(GetHUD()))
    {
        BlasterHUD->AddAnnouncement(CooldownTitleText, CooldownInfoText);
    }
}
