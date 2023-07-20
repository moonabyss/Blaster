// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

#include "BlasterCoreTypes.h"
#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "Components/BlasterMovementComponent.h"
#include "Weapon/BlasterBaseWeapon.h"

#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif  // !UE_BUILD_SHIPPING

UBlasterWeaponComponent::UBlasterWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
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

    FHitResult HitResult;
    TraceUnderCrosshairs(HitResult);
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
    if (!IsValid(CurrentWeapon) && IsValid(LastWeapon))
    {
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

    ServerFire();
}

void UBlasterWeaponComponent::ServerFire_Implementation()
{
    MulticastFire();
}

void UBlasterWeaponComponent::MulticastFire_Implementation()
{
    if (!IsValid(CurrentWeapon)) return;

    PlayFireMontage();

    CurrentWeapon->Fire(HitTarget);
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

void UBlasterWeaponComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
    if (!GetWorld()) return;

    FVector2D ViewportSize;
    if (GEngine && GEngine->GameViewport)
    {
        GEngine->GameViewport->GetViewportSize(ViewportSize);
    }

    FVector2D CrosshairLocation(ViewportSize.X / 2.0f, ViewportSize.Y / 2.0f);
    FVector CrosshairWorldPosition;
    FVector CrosshairWorldDirection;
    const bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(UGameplayStatics::GetPlayerController(this, 0), CrosshairLocation, CrosshairWorldPosition, CrosshairWorldDirection);

    if (bScreenToWorld)
    {
        FVector Start = CrosshairWorldPosition;
        // TODO: variable for trace distance
        FVector End = Start + CrosshairWorldDirection * 80000.0f;
        GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

        if (TraceHitResult.bBlockingHit)
        {
            HitTarget = TraceHitResult.ImpactPoint;
#if !UE_BUILD_SHIPPING
            DrawDebugSphere(GetWorld(), HitTarget, 30.0f, 12, FColor::Yellow);
#endif // !UE_BUILD_SHIPPING
        }
        else
        {
            HitTarget = End;
        }
    }
}
