// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterPlayerState.h"
#include "Character/BlasterCharacter.h"
#include "Character/BlasterPlayerController.h"

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
    SetScore(GetScore() + ScoreAmount);
}

void ABlasterPlayerState::OnRep_Score()
{
    Super::OnRep_Score();
}
