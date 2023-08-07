// Blaster Multiplayer Game. All rights reserved.

#include "HUD/CharacterOverlay.h"

#include "GameFramework/PlayerState.h"

#include "BlasterUtils.h"
#include "Character/BlasterPlayerState.h"
#include "Components/BlasterHealthComponent.h"
#include "Components/BlasterWeaponComponent.h"
#include "Weapon/BlasterBaseWeapon.h"

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

float UCharacterOverlay::GetScore() const
{
    float Score = 0.0f;
    if (auto PlayerState = GetOwningPlayerState())
    {
        Score = PlayerState->GetScore();
    }

    return FMath::Floor(Score);
}

int32 UCharacterOverlay::GetKilled() const
{
    int32 Killed = 0;
    if (auto PlayerState = Cast<ABlasterPlayerState>(GetOwningPlayerState()))
    {
        Killed = PlayerState->GetKilled();
    }

    return Killed;
}

int32 UCharacterOverlay::GetDefeats() const
{
    int32 Defeats = 0;
    if (auto PlayerState = Cast<ABlasterPlayerState>(GetOwningPlayerState()))
    {
        Defeats = PlayerState->GetDefeats();
    }

    return Defeats;
}

int32 UCharacterOverlay::GetAmmoInCLip()
{
    if (!IsValid(WeaponComponent))
    {
        WeaponComponent = BlasterUtils::GetBlasterPlayerComponent<UBlasterWeaponComponent>(GetOwningPlayerPawn());
    }
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon()) return 0;

    return WeaponComponent->GetCurrentWeapon()->GetAmmoInCLip();
}

int32 UCharacterOverlay::GetCarriedAmmo()
{
    if (!IsValid(WeaponComponent))
    {
        WeaponComponent = BlasterUtils::GetBlasterPlayerComponent<UBlasterWeaponComponent>(GetOwningPlayerPawn());
    }
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon() || WeaponComponent->GetCurrentWeapon()->GetWeaponProps().WeaponType == EWeaponType::None) return 0;

    return WeaponComponent->GetCarriedAmmo();
}

bool UCharacterOverlay::ShowAmmoWidget() const
{
    return IsValid(WeaponComponent) && WeaponComponent->GetCurrentWeapon() && WeaponComponent->GetCurrentWeapon()->GetWeaponProps().WeaponType != EWeaponType::None;
}
