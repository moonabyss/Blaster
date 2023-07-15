// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterMovementComponent.h"
#include "Character/BlasterCharacter.h"
#include "Weapon/BlasterBaseWeapon.h"

float UBlasterMovementComponent::GetMaxSpeed() const
{
    float BaseSpeed = Super::GetMaxSpeed();

    const ABlasterCharacter* Player = Cast<ABlasterCharacter>(GetPawnOwner());
    if (!IsValid(Player)) return BaseSpeed;

    float AimModifier = 1.0f;
    if (Player->IsAiming() && IsValid(Player->GetCurrentWeapon()))
    {
        AimModifier = Player->GetCurrentWeapon()->GetWeaponProps().AimSpeedModifier;
    }

    return BaseSpeed * AimModifier;
}
