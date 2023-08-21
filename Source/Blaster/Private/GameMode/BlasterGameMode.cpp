// Blaster Multiplayer Game. All rights reserved.

#include "GameMode/BlasterGameMode.h"
#include "GameFramework/Controller.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"

#include "Character/BlasterCharacter.h"
#include "Character/BlasterPlayerController.h"
#include "Character/BlasterPlayerState.h"

namespace MatchState
{
    const FName Cooldown = FName("Cooldown");
}

ABlasterGameMode::ABlasterGameMode()
{
    bDelayedStart = true;
}

void ABlasterGameMode::BeginPlay()
{
    Super::BeginPlay();

    LevelStartingTime = GetWorld()->GetTimeSeconds();
}

void ABlasterGameMode::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    if (MatchState == MatchState::WaitingToStart)
    {
        CountdownTime = WarmupTime + LevelStartingTime - GetWorld()->GetTimeSeconds();
        if (CountdownTime <= 0.0f)
        {
            StartMatch();
        }
    }
    else if (MatchState == MatchState::InProgress)
    {
        CountdownTime = WarmupTime + MatchTime + LevelStartingTime - GetWorld()->GetTimeSeconds();
        if (CountdownTime <= 0.0f)
        {
            SetMatchState(MatchState::Cooldown);
        }
    }
    else if (MatchState == MatchState::Cooldown)
    {
        CountdownTime = WarmupTime + MatchTime + CooldownTime + LevelStartingTime - GetWorld()->GetTimeSeconds();
        if (CountdownTime <= 0.0f)
        {
            RestartGame();
        }
    }
}

void ABlasterGameMode::OnMatchStateSet() {
    Super::OnMatchStateSet();

    for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
    {
        auto BlasterPlayer = Cast<ABlasterPlayerController>(*It);
        if (IsValid(BlasterPlayer))
        {
            BlasterPlayer->MatchStateSet(MatchState);
        }
    }
}

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
    if (IsValid(ElimmedCharacter))
    {
        ElimmedCharacter->Elim();
    }

    auto AttackerPlayerState = AttackerController ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
    auto VictimPlayerState = VictimController ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
    if (AttackerPlayerState && AttackerController != VictimController && MatchState == MatchState::InProgress)
    {
        AttackerPlayerState->AddToKilled(1);
    }
    if (VictimPlayerState && MatchState == MatchState::InProgress)
    {
        VictimPlayerState->IncrementDefeats();
    }
}

void ABlasterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
    if (ElimmedCharacter)
    {
        ElimmedCharacter->Reset();
        ElimmedCharacter->Destroy();
    }

    if (ElimmedController && MatchState == MatchState::InProgress && CountdownTime > 5.0f)
    {
        TArray<AActor*> PlayerStarts;
        UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
        const int32 Selection = FMath::RandRange(0, PlayerStarts.Num() - 1);
        RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
    }
}
