// Blaster Multiplayer Game. All rights reserved.

#include "HUD/CharacterOverlay.h"

#include "GameFramework/PlayerState.h"

#include "BlasterUtils.h"
#include "Character/BlasterPlayerController.h"
#include "Character/BlasterPlayerState.h"
#include "Components/BlasterHealthComponent.h"
#include "Components/BlasterWeaponComponent.h"
#include "GameMode/BlasterGameMode.h"
#include "Weapon/BlasterBaseWeapon.h"

void UCharacterOverlay::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    BlasterPlayerController = Cast<ABlasterPlayerController>(GetOwningPlayer());
    BlasterPlayerState = Cast<ABlasterPlayerState>(GetOwningPlayerState());
}

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
    if (IsValid(BlasterPlayerState))
    {
        Killed = BlasterPlayerState->GetKilled();
    }

    return Killed;
}

int32 UCharacterOverlay::GetDefeats() const
{
    int32 Defeats = 0;
    if (IsValid(BlasterPlayerState))
    {
        Defeats = BlasterPlayerState->GetDefeats();
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

int32 UCharacterOverlay::GetClipCapacity()
{
    if (!IsValid(WeaponComponent))
    {
        WeaponComponent = BlasterUtils::GetBlasterPlayerComponent<UBlasterWeaponComponent>(GetOwningPlayerPawn());
    }
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon()) return 0;

    return WeaponComponent->GetCurrentWeapon()->GetClipCapacity();
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

FText UCharacterOverlay::GetMatchCountdown()
{
    float CountdownTime = 0.0f;
    if (IsValid(BlasterPlayerController))
    {
        CountdownTime = BlasterPlayerController->GetLeftMatchTime();
    }

    const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
    const int32 Seconds = FMath::FloorToInt(CountdownTime - Minutes * 60);

    return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}
