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
    return FMath::Max(0, MatchDuration - GetWorld()->GetTimeSeconds());
}
