// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "Net/UnrealNetwork.h"

#include "BlasterCoreTypes.h"
#include "Character/BlasterAnimInstance.h"
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
    DOREPLIFETIME_CONDITION(UBlasterWeaponComponent, bWantsAiming, COND_SkipOwner);
    DOREPLIFETIME_CONDITION(UBlasterWeaponComponent, bWantsFire, COND_SkipOwner);
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
    if (Character && !Character->HasAuthority()) ServerSetWantsAiming(true);
}

void UBlasterWeaponComponent::StopAiming()
{
    bWantsAiming = false;
    if (Character && !Character->HasAuthority()) ServerSetWantsAiming(false);
}

bool UBlasterWeaponComponent::IsAiming()
{
    if (!Character) return false;

    return bWantsAiming && !Character->GetCharacterMovement()->IsFalling();
}

void UBlasterWeaponComponent::ServerSetWantsAiming_Implementation(bool bIsAiming) 
{
    bWantsAiming = bIsAiming;
}

void UBlasterWeaponComponent::StartFire() 
{
    bWantsFire = true;
    if (Character && !Character->HasAuthority()) ServerSetWantsFire(true);

    Fire();
}

void UBlasterWeaponComponent::StopFire() 
{
    bWantsFire = false;
    if (Character && !Character->HasAuthority()) ServerSetWantsFire(false);
}

void UBlasterWeaponComponent::ServerSetWantsFire_Implementation(bool bIsFiring) 
{
    bWantsFire = bIsFiring;
}

void UBlasterWeaponComponent::Fire() 
{
    if (!IsValid(CurrentWeapon)) return;

    PlayFireMontage();
    CurrentWeapon->Fire();
}

void UBlasterWeaponComponent::PlayFireMontage() 
{
    if (!IsValid(Character) || !IsValid(Character->GetMesh())) return;

    if (UAnimInstance* AnimInstance = Character->GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Play(GetCurrentWeapon()->GetWeaponProps().BlasterFireMontage);
        const FName SectionName = IsAiming() ? WeaponAimMontageSectionName : WeaponHipMontageSectionName;
        AnimInstance->Montage_JumpToSection(SectionName);
    }
}
