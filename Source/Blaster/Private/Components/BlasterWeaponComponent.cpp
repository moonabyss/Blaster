// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "Weapon/BlasterBaseWeapon.h"
#include "BlasterCoreTypes.h"

UBlasterWeaponComponent::UBlasterWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBlasterWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UBlasterWeaponComponent::SetCharacter(ABlasterCharacter* BlasterCharacter)
{
    Character = BlasterCharacter;
}

void UBlasterWeaponComponent::EquipWeapon(ABlasterBaseWeapon* WeaponToEquip) 
{
    if (!IsValid(WeaponToEquip)) return;

    EquippedWeapon = WeaponToEquip;
    EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
    Character->GetMesh()
}
