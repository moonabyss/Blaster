// Blaster Multiplayer Game. All rights reserved.

#include "HUD/CharacterOverlay.h"

#include "GameFramework/PlayerState.h"

#include "BlasterUtils.h"
#include "Components/BlasterHealthComponent.h"

float UCharacterOverlay::GetHealth()
{
    if (!IsValid(HealthComponent))
    {
        HealthComponent = BlasterUtils::GetBlasterPlayerComponent<UBlasterHealthComponent>(GetOwningPlayerPawn());
    }
    if (!HealthComponent) return 0.0f;

    return HealthComponent->GetHealth();
}

float UCharacterOverlay::GetMaxHealth()
{
    if (!IsValid(HealthComponent))
    {
        HealthComponent = BlasterUtils::GetBlasterPlayerComponent<UBlasterHealthComponent>(GetOwningPlayerPawn());
    }
    if (!HealthComponent) return 0.0f;

    return HealthComponent->GetMaxHealth();
}

float UCharacterOverlay::GetHealthPercent()
{
    if (!IsValid(HealthComponent))
    {
        HealthComponent = BlasterUtils::GetBlasterPlayerComponent<UBlasterHealthComponent>(GetOwningPlayerPawn());
    }
    if (!HealthComponent) return 0.0f;

    return HealthComponent->GetHealthPercent();
}

float UCharacterOverlay::GetScore()
{
    float Score = 0.0f;
    if (auto PlayerState = GetOwningPlayerState())
    {
        Score = PlayerState->GetScore();
    }

    return FMath::Floor(Score);
}
