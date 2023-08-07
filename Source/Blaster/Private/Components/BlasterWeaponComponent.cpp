// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include "BlasterCoreTypes.h"
#include "Character/BlasterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "Character/BlasterPlayerController.h"
#include "Components/BlasterMovementComponent.h"
#include "HUD/BlasterHUD.h"
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

    if (auto PC = Character->GetController<APlayerController>())
    {
        DefaultFOV = PC->PlayerCameraManager->DefaultFOV;
        CurrentFOV = DefaultFOV;
    }
}

void UBlasterWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Character && Character->IsLocallyControlled())
    {
        FHitResult HitResult;
        TraceUnderCrosshairs(HitResult, false);
        HitTargetNoSpread = HitResult.ImpactPoint;

        InterpFOV(DeltaTime);
        SetHUDCrosshairs(DeltaTime);
    }
}

void UBlasterWeaponComponent::SetCharacter(ABlasterCharacter* BlasterCharacter)
{
    Character = BlasterCharacter;
}

void UBlasterWeaponComponent::EquipWeapon(ABlasterBaseWeapon* WeaponToEquip)
{
    if (!IsValid(WeaponToEquip) || !Character) return;

    if (IsValid(CurrentWeapon))
    {
        DropWeapon();
    }

    CurrentWeapon = WeaponToEquip;
    CurrentWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
    CurrentWeapon->SetOwner(Character);
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponRightHandSocket);

    WeaponEquipped.Broadcast();
}

void UBlasterWeaponComponent::DropWeapon()
{
    if (!IsValid(CurrentWeapon)) return;

    CurrentWeapon->SetWeaponState(EWeaponState::EWS_Dropped);
    DetachWeapon(CurrentWeapon);
    CurrentWeapon->SetOwner(nullptr);
    CurrentWeapon = nullptr;

    WeaponUnequipped.Broadcast();
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
    Weapon->GetMesh()->AttachToComponent(SceneComponent, AttachmentRules, SocketName);
}

void UBlasterWeaponComponent::DetachWeapon(ABlasterBaseWeapon* Weapon)
{
    if (!Weapon) return;

    FDetachmentTransformRules DetachRule(EDetachmentRule::KeepWorld, true);
    Weapon->GetMesh()->DetachFromComponent(DetachRule);
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

bool UBlasterWeaponComponent::CanShoot() const
{
    return Character && Character->IsAlive() && bCanFire && IsValid(CurrentWeapon) && CurrentWeapon->GetAmmoInCLip() > 0;
}

void UBlasterWeaponComponent::Fire()
{
    if (!CanShoot() || !IsValid(CurrentWeapon)) return;

    if (Character->IsLocallyControlled())
    {
        FHitResult HitResult;
        TraceUnderCrosshairs(HitResult, true);
        ServerFire(HitResult.ImpactPoint);
        CrosshairsShootingFactor = FMath::Min(CurrentWeapon->GetWeaponProps().SpreadModifierShootingMax, CrosshairsShootingFactor + CurrentWeapon->GetWeaponProps().SpreadModifierPerShoot);

        StartFireTimer();
        bCanFire = false;
    }
}

void UBlasterWeaponComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
    CurrentWeapon->DecrementAmmo();
    MulticastFire(TraceHitTarget);
    StartFireTimer();
}

void UBlasterWeaponComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
    if (!IsValid(CurrentWeapon)) return;

    PlayFireMontage();

    CurrentWeapon->Fire(TraceHitTarget);
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

void UBlasterWeaponComponent::StartFireTimer()
{
    if (!IsValid(CurrentWeapon) || !Character) return;

    Character->GetWorldTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, CurrentWeapon->GetWeaponProps().FireDelay, false);
}

void UBlasterWeaponComponent::FireTimerFinished()
{
    if (!CurrentWeapon) return;

    bCanFire = true;
    if (bWantsFire && CurrentWeapon->GetWeaponProps().bIsAutomatic)
    {
        Fire();
    }
}

void UBlasterWeaponComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult, bool bWithSpread)
{
    if (!GetWorld() || !IsValid(CurrentWeapon)) return;

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

        if (IsValid(Character))
        {
            float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
            Start += CrosshairWorldDirection * (DistanceToCharacter + 100.0f);
        }
        // TODO: variable for trace distance
        FVector End;

        if (bWithSpread)
        {
            // Shooting
            const auto HalfRad = FMath::DegreesToRadians(CurrentSpreadAngle / 2.0f);
            const FVector ShootDirection = FMath::VRandCone(CrosshairWorldDirection, HalfRad);
            End = Start + ShootDirection * 80000.0f;
            GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
        }
        else
        {
            // Aiming
            End = Start + CrosshairWorldDirection * 80000.0f;
            GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);

            if (IsValid(TraceHitResult.GetActor()) && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairs>())
            {
                Crosshairs.Color = CrosshairsEnemyColor;
            }
            else
            {
                Crosshairs.Color = CrosshairsDefaultColor;
            }
        }

        if (!TraceHitResult.bBlockingHit)
        {
            TraceHitResult.ImpactPoint = End;
        }
    }
}

void UBlasterWeaponComponent::SetHUDCrosshairs(float DeltaTime)
{
    if (!Character || !Character->GetController()) return;

    if (!Controller)
    {
        Controller = Cast<ABlasterPlayerController>(Character->GetController());
    }
    if (!Controller) return;

    if (!HUD)
    {
        HUD = Cast<ABlasterHUD>(Controller->GetHUD());
    }
    if (!HUD) return;

    if (CurrentWeapon)
    {
        Crosshairs.Center = CurrentWeapon->GetWeaponProps().Crosshairs.Center;
        Crosshairs.Left = CurrentWeapon->GetWeaponProps().Crosshairs.Left;
        Crosshairs.Right = CurrentWeapon->GetWeaponProps().Crosshairs.Right;
        Crosshairs.Top = CurrentWeapon->GetWeaponProps().Crosshairs.Top;
        Crosshairs.Bottom = CurrentWeapon->GetWeaponProps().Crosshairs.Bottom;
        Crosshairs.SpreadAngle = CurrentWeapon->GetWeaponProps().DefaultSpreadInDegrees * CalculateCurrentSpreadModifier(DeltaTime);
        CurrentSpreadAngle = Crosshairs.SpreadAngle;
    }
    else
    {
        Crosshairs = FCrosshairs();
    }
    HUD->SetCrosshairs(Crosshairs);
}

float UBlasterWeaponComponent::CalculateCurrentSpreadModifier(float DeltaTime)
{
    float Result = 1.0f;

    // [0, 600] -> [0, 1]
    // CrosshairsVelocityFactor
    const FVector2D WalkSpeedRange(0, Character->GetCharacterMovement()->MaxWalkSpeed);
    const FVector2D VelocityMultiplierRange(0.0f, CurrentWeapon->GetWeaponProps().SpreadModifierWalk);
    CrosshairsVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplierRange, Character->GetVelocity().Size2D());

    // CrosshairsInAirFactor
    if (Character->GetCharacterMovement()->IsFalling())
    {
        CrosshairsInAirFactor = FMath::FInterpTo(CrosshairsInAirFactor, 2.0f, DeltaTime, 2.0f);
    }
    else
    {
        CrosshairsInAirFactor = FMath::FInterpTo(CrosshairsInAirFactor, 0.0f, DeltaTime, 20.0f);
    }

    // CrosshairsAimFactor
    if (IsAiming())
    {
        CrosshairsAimFactor = FMath::FInterpTo(CrosshairsAimFactor, CurrentWeapon->GetWeaponProps().SpreadModifierZoom, DeltaTime, 30.0f);
    }
    else
    {
        CrosshairsAimFactor = FMath::FInterpTo(CrosshairsAimFactor, 0.0f, DeltaTime, 30.0f);
    }

    // CrosshairsShootingFactor
    CrosshairsShootingFactor = FMath::FInterpConstantTo(CrosshairsShootingFactor, 0.0f, DeltaTime, 4.0f);

    const float SpreadFactor = FMath::Max(0.0f, Result + CrosshairsVelocityFactor + CrosshairsInAirFactor - CrosshairsAimFactor + CrosshairsShootingFactor);
    return SpreadFactor;
}

void UBlasterWeaponComponent::InterpFOV(float DeltaTime)
{
    const auto PC = Character->GetController<APlayerController>();
    if (!PC || !PC->PlayerCameraManager) return;

    if (!CurrentWeapon)
    {
        CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, DefaultInterpSpeed);
        PC->PlayerCameraManager->SetFOV(CurrentFOV);
        return;
    }

    if (IsAiming())
    {
        CurrentFOV = FMath::FInterpTo(CurrentFOV, CurrentWeapon->GetWeaponProps().ZoomedFOV, DeltaTime, CurrentWeapon->GetWeaponProps().ZoomInterpSpeed);
        PC->PlayerCameraManager->SetFOV(CurrentFOV);
    }
    else
    {
        CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, DeltaTime, CurrentWeapon->GetWeaponProps().ZoomInterpSpeed);
        PC->PlayerCameraManager->SetFOV(CurrentFOV);
    }
}
