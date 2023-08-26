// Blaster Multiplayer Game. All rights reserved.

#include "GameState/BlasterGameState.h"

#include "Character/BlasterPlayerState.h"
#include "Net/UnrealNetwork.h"

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
    if (TopScoringPlayers.Num() == 0)
    {
        TopScoringPlayers.Add(ScoringPlayer);
        TopScore = ScoringPlayer->GetKilled();
    }
    else if (ScoringPlayer->GetKilled() == TopScore)
    {
        TopScoringPlayers.AddUnique(ScoringPlayer);
    }
    else if (ScoringPlayer->GetKilled() > TopScore)
    {
        TopScoringPlayers.Empty();
        TopScoringPlayers.Add(ScoringPlayer);
        TopScore = ScoringPlayer->GetKilled();
    }
}
