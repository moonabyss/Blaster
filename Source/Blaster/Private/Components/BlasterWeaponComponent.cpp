// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "Net/UnrealNetwork.h"

#include "BlasterCoreTypes.h"
#include "Character/BlasterCharacter.h"
#include "Components/BlasterMovementComponent.h"
#include "Weapon/BlasterBaseWeapon.h"

UBlasterWeaponComponent::UBlasterWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterWeaponComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

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

    WeaponEquipped.Broadcast();
}

void UBlasterWeaponComponent::OnRep_CurrentWeapon(ABlasterBaseWeapon* LastWeapon)
{
    if (IsValid(CurrentWeapon) && !LastWeapon)
    {
        WeaponEquipped.Broadcast();
    }
    if (!IsValid(CurrentWeapon) && IsValid(LastWeapon)){
        WeaponUnequipped.Broadcast();
    }
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

ABlasterBaseWeapon* UBlasterWeaponComponent::GetCurrentWeapon() const 
{
    if (!IsValid(CurrentWeapon)) return nullptr;

    return CurrentWeapon;
}

void UBlasterWeaponComponent::StartAiming()
{
    if (!IsValid(CurrentWeapon)) return;
    
    bWantsAiming = true;
}

void UBlasterWeaponComponent::StopAiming()
{
    bWantsAiming = false;
}

bool UBlasterWeaponComponent::IsAiming() const
{
    if (!Character) return false;

    return bWantsAiming && !Character->GetCharacterMovement()->IsFalling();
}
