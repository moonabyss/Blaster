// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "TimerManager.h"

#include "BlasterTypes/BlasterCoreTypes.h"
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
    DOREPLIFETIME_CONDITION(UBlasterWeaponComponent, CarriedAmmo, COND_OwnerOnly);
    DOREPLIFETIME(UBlasterWeaponComponent, CombatState);
}

void UBlasterWeaponComponent::BeginPlay()
{
    Super::BeginPlay();

    Character = Cast<ABlasterCharacter>(GetOwner());
    check(Character);

    if (auto PC = Character->GetController<APlayerController>())
    {
        DefaultFOV = PC->PlayerCameraManager->DefaultFOV;
        CurrentFOV = DefaultFOV;
    }

    InitializeAmmoMap();
}

void UBlasterWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

    if (Character && Character->IsLocallyControlled() && CurrentWeapon)
    {
        FHitResult HitResult;
        TraceUnderCrosshairs(HitResult, false, CurrentWeapon->GetWeaponProps().Range);
        HitTargetNoSpread = HitResult.ImpactPoint;
        DetectObstacle(HitResult);

        InterpFOV(DeltaTime);
        SetHUDCrosshairs(DeltaTime);
    }
}

bool UBlasterWeaponComponent::EquipWeapon(ABlasterBaseWeapon* WeaponToEquip)
{
    if (!IsValid(WeaponToEquip) || !Character) return false;

    if (IsValid(CurrentWeapon))
    {
        DropWeapon();
    }

    CurrentWeapon = WeaponToEquip;
    CurrentWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
    CurrentWeapon->SetOwner(Character);
    AttachWeaponToSocket(CurrentWeapon, Character->GetMesh(), WeaponRightHandSocket);
    CarriedAmmo = CarriedAmmoMap.FindOrAdd(CurrentWeapon->GetWeaponProps().WeaponType);

    if (CurrentWeapon->GetWeaponProps().EquipSound)
    {
        UGameplayStatics::PlaySoundAtLocation(CurrentWeapon, CurrentWeapon->GetWeaponProps().EquipSound, CurrentWeapon->GetActorLocation());
    }

    if (CurrentWeapon->IsClipEmpty())
    {
        Reload();
    }

    WeaponEquipped.Broadcast();
    return true;
}

void UBlasterWeaponComponent::DropWeapon()
{
    if (!IsValid(CurrentWeapon)) return;
    if (CombatState != ECombatState::ECS_Unoccupied) return;

    CurrentWeapon->SetWeaponState(EWeaponState::EWS_Dropped);
    DetachWeapon(CurrentWeapon);
    CurrentWeapon->SetOwner(nullptr);
    CurrentWeapon = nullptr;

    WeaponUnequipped.Broadcast();
}

void UBlasterWeaponComponent::OnRep_CurrentWeapon(ABlasterBaseWeapon* LastWeapon)
{
    if (IsValid(CurrentWeapon) && !LastWeapon)  // new weapon equipped
    {
        UGameplayStatics::PlaySoundAtLocation(CurrentWeapon, CurrentWeapon->GetWeaponProps().EquipSound, CurrentWeapon->GetActorLocation());

        WeaponEquipped.Broadcast();
    }
    else if (IsValid(CurrentWeapon))  // Weapon changed
    {
        UGameplayStatics::PlaySoundAtLocation(CurrentWeapon, CurrentWeapon->GetWeaponProps().EquipSound, CurrentWeapon->GetActorLocation());

        WeaponEquipped.Broadcast();
    }
    else if (!IsValid(CurrentWeapon) && IsValid(LastWeapon))  // weapon unequipped
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

    return bWantsAiming && !Character->GetCharacterMovement()->IsFalling() && CombatState != ECombatState::ECS_Reloading;
}

void UBlasterWeaponComponent::ServerSetWantsAiming_Implementation(bool bIsAiming)
{
    bWantsAiming = bIsAiming;
}

void UBlasterWeaponComponent::StartFire()
{
    bWantsFire = true;
    if (Character && !Character->HasAuthority()) ServerSetWantsFire(true);

    if (bWantsFire && CurrentWeapon && CurrentWeapon->IsClipEmpty())
    {
        Reload();
    }

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
    return Character && Character->IsAlive() &&                             //
           bCanFire &&                                                      //
           IsValid(CurrentWeapon) && CurrentWeapon->GetAmmoInCLip() > 0 &&  //
           CombatState == ECombatState::ECS_Unoccupied;
}

void UBlasterWeaponComponent::Fire()
{
    if (!CanShoot() || !IsValid(CurrentWeapon) || !CurrentWeapon->GetMesh()) return;

    if (Character->IsLocallyControlled())
    {
        const FVector BarelLocation = CurrentWeapon->GetMesh()->GetSocketLocation(MuzzleFlashSocketName);
        FHitResult HitResult;
        TraceUnderCrosshairs(HitResult, true, CurrentWeapon->GetWeaponProps().Range);
        Server_Fire(BarelLocation, HitResult.ImpactPoint);
        CrosshairsShootingFactor = FMath::Min(CurrentWeapon->GetWeaponProps().SpreadModifierShootingMax, CrosshairsShootingFactor + CurrentWeapon->GetWeaponProps().SpreadModifierPerShoot);

        StartFireTimer();
        bCanFire = false;
    }
}

void UBlasterWeaponComponent::Server_Fire_Implementation(const FVector_NetQuantize& BarelLocation, const FVector_NetQuantize& TraceHitTarget)
{
    if (!CanShoot() || !IsValid(CurrentWeapon)) return;

    CurrentWeapon->DecrementAmmo();
    Multicast_Fire(BarelLocation, TraceHitTarget);
    StartFireTimer();
}

void UBlasterWeaponComponent::Multicast_Fire_Implementation(const FVector_NetQuantize& BarelLocation, const FVector_NetQuantize& TraceHitTarget)
{
    if (!IsValid(CurrentWeapon)) return;

    PlayFireMontage();

    CurrentWeapon->Fire(BarelLocation, TraceHitTarget);
}

void UBlasterWeaponComponent::PlayFireMontage()
{
    if (!IsValid(Character) || !IsValid(Character->GetMesh())) return;

    const FName SectionName = IsAiming() ? WeaponAimMontageSectionName : WeaponHipMontageSectionName;
    Character->PlayAnimMontage(GetCurrentWeapon()->GetWeaponProps().BlasterFireMontage, 1.0f, SectionName);
}

void UBlasterWeaponComponent::StartFireTimer()
{
    if (!IsValid(CurrentWeapon) || !Character) return;

    Character->GetWorldTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, CurrentWeapon->GetWeaponProps().FireDelay, false);
}

void UBlasterWeaponComponent::FireTimerFinished()
{
    bCanFire = true;

    if (!CurrentWeapon) return;
    if (bWantsFire && CurrentWeapon->GetWeaponProps().bIsAutomatic)
    {
        Fire();
    }
    if (bWantsFire && CurrentWeapon->IsClipEmpty())
    {
        Reload();
    }
}

void UBlasterWeaponComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult, bool bWithSpread, float Range)
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

        if (IsValid(Character))
        {
            float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
            Start += CrosshairWorldDirection * (DistanceToCharacter + 100.0f);
        }

        FVector End;

        if (bWithSpread)
        {
            // Shooting
            const auto HalfRad = FMath::DegreesToRadians(CurrentSpreadAngle * 0.5f);
            const FVector ShootDirection = FMath::VRandCone(CrosshairWorldDirection, HalfRad);
            End = Start + ShootDirection * Range;
            GetWorld()->LineTraceSingleByChannel(TraceHitResult, Start, End, ECollisionChannel::ECC_Visibility);
        }
        else
        {
            // Aiming
            End = Start + CrosshairWorldDirection * Range;
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

void UBlasterWeaponComponent::DetectObstacle(const FHitResult& TraceHit)
{
    if (!CurrentWeapon->GetMesh()) return;

    if (!TraceHit.bBlockingHit)
    {
        Character->HideObstaclePoint();
        return;
    }

    FHitResult WeaponHitResult;
    const FVector WeaponBarel = CurrentWeapon->GetMesh()->GetSocketLocation(MuzzleFlashSocketName);
    FCollisionQueryParams CollisionParams;
    CollisionParams.AddIgnoredActor(CurrentWeapon);
    FCollisionResponseParams ResponseParams;
    ResponseParams.CollisionResponse = ECollisionResponse::ECR_Block;
    const bool bHit = GetWorld()->SweepSingleByChannel(WeaponHitResult, WeaponBarel, TraceHit.ImpactPoint, FQuat::Identity, ECC_Visibility, FCollisionShape::MakeSphere(2), CollisionParams, ResponseParams);
    if (bHit)
    {
        if (FVector::Distance(TraceHit.ImpactPoint, WeaponHitResult.ImpactPoint) > 300.0f &&            //
            FVector::Distance(WeaponHitResult.ImpactPoint, Character->GetActorLocation()) < 1000.0f &&  //
            !WeaponHitResult.GetActor()->Implements<UHitable>())
        {
            Character->ObstacleInFront(WeaponHitResult.ImpactPoint);
        }
        else
        {
            Character->HideObstaclePoint();
        }
    }
    else
    {
        Character->HideObstaclePoint();
    }
}

void UBlasterWeaponComponent::SetHUDCrosshairs(float DeltaTime)
{
    if (!Character || !Character->GetController()) return;

    if (!Controller)
    {
        Controller = Cast<ABlasterPlayerController>(Character->GetController());
        if (!Controller) return;
    }

    if (!HUD)
    {
        HUD = Cast<ABlasterHUD>(Controller->GetHUD());
        if (!HUD) return;
    }

    if (CurrentWeapon)
    {
        Crosshairs.CrosshairsData = CurrentWeapon->GetWeaponProps().Crosshairs.CrosshairsData;
        Crosshairs.SpreadAngle = CurrentWeapon->GetWeaponProps().DefaultSpreadInDegrees * CalculateCurrentSpreadModifier(DeltaTime);
        CurrentSpreadAngle = Crosshairs.SpreadAngle;
    }
    else
    {
        Crosshairs.CrosshairsData.Reset();
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

    CurrentFOV = FMath::FInterpTo(CurrentFOV, IsAiming() ? CurrentWeapon->GetWeaponProps().ZoomedFOV : DefaultFOV, DeltaTime, CurrentWeapon->GetWeaponProps().ZoomInterpSpeed);
    PC->PlayerCameraManager->SetFOV(CurrentFOV);
}

void UBlasterWeaponComponent::InitializeAmmoMap()
{
    for (int32 WeaponTypeIndex = 0; WeaponTypeIndex < (int32) EWeaponType::MAX; ++WeaponTypeIndex)
    {
        EWeaponType WeaponType = static_cast<EWeaponType>(WeaponTypeIndex);

        int32* AmmoPtr = LoadoutMap.Find(WeaponType);
        if (AmmoPtr)
        {
            CarriedAmmoMap.Add(WeaponType, *AmmoPtr);
        }
        else
        {
            CarriedAmmoMap.Add(WeaponType, 0);
        }
    }
}

void UBlasterWeaponComponent::Reload()
{
    if (!IsValid(CurrentWeapon)) return;

    if (!CurrentWeapon->IsClipFull() &&               //
        CarriedAmmo > 0 &&                            //
        CombatState == ECombatState::ECS_Unoccupied)  //
    {
        ServerReload();
    }
}

void UBlasterWeaponComponent::ServerReload_Implementation()
{
    if (CarriedAmmoMap.FindOrAdd(CurrentWeapon->GetWeaponProps().WeaponType) == 0) return;

    CombatState = ECombatState::ECS_Reloading;

    FTimerDelegate TimerDelegate;
    TimerDelegate.BindUFunction(this, "FinishReloading");
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, TimerDelegate, CurrentWeapon->GetWeaponProps().ReloadTime, false);

    HandleReload();
}

void UBlasterWeaponComponent::HandleReload()
{
    PlayReloadMontage();
}

void UBlasterWeaponComponent::PlayReloadMontage()
{

    if (!IsValid(Character) || !IsValid(Character->GetMesh())) return;

    Character->PlayAnimMontage(GetCurrentWeapon()->GetWeaponProps().BlasterReloadMontage);
}

void UBlasterWeaponComponent::OnRep_CombatState()
{
    switch (CombatState)
    {
        case ECombatState::ECS_Unoccupied:
        {
            break;
        }
        case ECombatState::ECS_Reloading:
        {
            HandleReload();
            break;
        }
    }
}

void UBlasterWeaponComponent::FinishReloading()
{
    CombatState = ECombatState::ECS_Unoccupied;

    ChargeClip();
}

void UBlasterWeaponComponent::ChargeClip()
{
    if (!CurrentWeapon) return;

    const int32 NeededAmmo = CurrentWeapon->GetWeaponProps().ClipCapacity - CurrentWeapon->GetAmmoInCLip();
    if (NeededAmmo <= CarriedAmmo)
    {
        if (CurrentWeapon->TryAddAmmoToClip(NeededAmmo))
        {
            CarriedAmmo = FMath::Max(CarriedAmmo - NeededAmmo, 0);
        }
    }
    else
    {
        if (CurrentWeapon->TryAddAmmoToClip(CarriedAmmo))
        {
            CarriedAmmo = 0;
        }
    }
    CarriedAmmoMap.FindOrAdd(CurrentWeapon->GetWeaponProps().WeaponType) = CarriedAmmo;
}
