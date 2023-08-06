// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerState.h"
#include "Character/BlasterCharacter.h"
#include "Character/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ABlasterPlayerState, Killed, COND_OwnerOnly);
    DOREPLIFETIME_CONDITION(ABlasterPlayerState, Defeats, COND_OwnerOnly);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
    SetScore(GetScore() + ScoreAmount);
}

void ABlasterPlayerState::OnRep_Score()
{
    Super::OnRep_Score();
}

void ABlasterPlayerState::AddToKilled(int NumOfKilled)
{
    Killed += NumOfKilled;
}

void ABlasterPlayerState::IncrementDefeats()
{
    ++Defeats;
}

void ABlasterPlayerState::OnRep_Killed() {}

void ABlasterPlayerState::OnRep_Defeats() {}
