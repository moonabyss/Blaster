// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "Net/UnrealNetwork.h"

#include "BlasterCoreTypes.h"
#include "Character/BlasterCharacter.h"
#include "Weapon/BlasterBaseWeapon.h"

UBlasterWeaponComponent::UBlasterWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    DOREPLIFETIME(UBlasterWeaponComponent, CurrentWeapon);
    DOREPLIFETIME(UBlasterWeaponComponent, bWantsAiming);
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
    if (!IsValid(WeaponToEquip) || !Character) return;

    CurrentWeapon = WeaponToEquip;
    CurrentWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
    CurrentWeapon->SetOwner(Character);
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponRightHandSocket);
}

void UBlasterWeaponComponent::AttachWeaponToSocket(ABlasterBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName)
{
    if (!Weapon || !SceneComponent) return;

    FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
    Weapon->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

bool UBlasterWeaponComponent::IsWeaponEquipped() const
{
    return IsValid(CurrentWeapon);
}

EWeaponType UBlasterWeaponComponent::GetEquippedWeaponType() const
{
    if (!IsValid(CurrentWeapon)) return EWeaponType::EWT_MAX;

    return CurrentWeapon->GetWeaponType();
}

void UBlasterWeaponComponent::StartAiming()
{
    if (!IsValid(CurrentWeapon)) return;
    SetAiming(true);
}

void UBlasterWeaponComponent::StopAiming()
{
    SetAiming(false);
}

bool UBlasterWeaponComponent::IsAiming()
{
    return bWantsAiming;
}

void UBlasterWeaponComponent::SetAiming(bool bIsAiming)
{
    bWantsAiming = bIsAiming;
    if (!Character->HasAuthority())
    {
        ServerSetAiming(bIsAiming);
    }
}

void UBlasterWeaponComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
    bWantsAiming = bIsAiming;
}
