// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "BlasterCoreTypes.h"
#include "BlasterUtils.h"
#include "Character/BlasterCharacter.h"
#include "Weapon/BlasterBaseWeapon.h"

#if !UE_BUILD_SHIPPING
#include "DrawDebugHelpers.h"
#endif  // !UE_BUILD_SHIPPING

DEFINE_LOG_CATEGORY_STATIC(LogBlasterAnimInstance, All, All);

void UBlasterAnimInstance::NativeInitializeAnimation()
{
    Super::NativeInitializeAnimation();

    APawn* Pawn = TryGetPawnOwner();
    if (!IsValid(Pawn)) return;

    BlasterCharacter = Cast<ABlasterCharacter>(Pawn);
    if (!IsValid(BlasterCharacter))
    {
        BLASTER_LOG_ERROR(LogBlasterAnimInstance, TEXT("UBlasterAnimInstance::NativeInitializeAnimation() UBlasterAnimInstance can only be used with ABlasterCharacter"));
        return;
    }
}

void UBlasterAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
    Super::NativeUpdateAnimation(DeltaSeconds);

    if (!IsValid(BlasterCharacter)) return;

    if (!IsValid(BlasterCharacter->GetCharacterMovement()))
    {
        BLASTER_LOG_ERROR(LogBlasterAnimInstance, TEXT("UBlasterAnimInstance::NativeUpdateAnimation() Invalid MovementComponent"));
        return;
    }

    SpeedLastFrame = Speed;
    AO_YawLastFrame = AO_Yaw;
    AO_PitchLastFrame = AO_Pitch;

    FVector Velocity = BlasterCharacter->GetVelocity().GetSafeNormal2D();
    Speed = Velocity.Size();

    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
    bIsAccelerating = !FMath::IsNearlyZero(BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size());
    bIsWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
    bIsCrouched = BlasterCharacter->bIsCrouched;
    bIsAiming = BlasterCharacter->IsAiming();
    TurningInPlace = BlasterCharacter->GetTurningInPlace();
    bRotateRootBone = BlasterCharacter->ShouldRotateRootBone();
    if (BlasterCharacter->GetCurrentWeapon())
    {
        AimSpeedModifier = BlasterCharacter->GetCurrentWeapon()->GetWeaponProps().AimSpeedModifier;
    }
    else
    {
        AimSpeedModifier = 1.0f;
    }
    bIsLocallyControlled = BlasterCharacter->IsLocallyControlled();

    // Offset Yaw for Strafing
    const auto AimRotation = BlasterCharacter->GetBaseAimRotation();
    const auto MovementRotation = UKismetMathLibrary::MakeRotFromX(Velocity);
    const auto DeltaRot = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation);
    if (FMath::IsNearlyZero(SpeedLastFrame))
    {
        DeltaRotation = DeltaRot;
    }
    else
    {
        DeltaRotation = FMath::RInterpTo(DeltaRotation, DeltaRot, DeltaSeconds, YawInterpSpeed);
    }
    YawOffset = DeltaRotation.Yaw;

    // Lean
    CharacterRotationLastFrame = CharacterRotation;
    CharacterRotation = BlasterCharacter->GetActorRotation();
    const auto Delta = UKismetMathLibrary::NormalizedDeltaRotator(CharacterRotation, CharacterRotationLastFrame);
    const auto Target = Delta.Yaw / DeltaSeconds;
    const auto Interp = FMath::FInterpTo(Lean, Target, DeltaSeconds, LeanInterpSpeed);
    Lean = FMath::Clamp(Interp, -90.0f, 90.f);

    // Aim Offset
    FRotator LastRotator = FRotator(AO_PitchLastFrame, AO_YawLastFrame, 0.0f);
    FRotator TargetRotator = FRotator(BlasterCharacter->GetAimPitch(), BlasterCharacter->GetAimYaw(), 0.0f);
    FRotator NextRotator = FMath::RInterpTo(LastRotator, TargetRotator, DeltaSeconds, AimInterpSpeed);
    AO_Yaw = NextRotator.Yaw;
    AO_Pitch = NextRotator.Pitch;

    // FABRIK
    if (bIsWeaponEquipped)
    {
        LeftHandTransform = BlasterCharacter->GetCurrentWeapon()->GetMesh()->GetSocketTransform(LeftHandSocketName, ERelativeTransformSpace::RTS_World);
        FVector OutPosition;
        FRotator OutRotation;
        BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("Hand_R"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
        LeftHandTransform.SetLocation(OutPosition);
        LeftHandTransform.SetRotation(FQuat(OutRotation));
    }

    // Correcting the weapon rotation to aim
    if (bIsWeaponEquipped && BlasterCharacter->IsLocallyControlled())
    {
        FTransform RightHandTransform = BlasterCharacter->GetCurrentWeapon()->GetMesh()->GetSocketTransform(FName("Hand_R"), ERelativeTransformSpace::RTS_World);
        const FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(                                                 //
            RightHandTransform.GetLocation(),                                                                                   //
            RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTargetNoSpread()));  //
        RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaSeconds, 10.0f);

        // FTransform MuzzleTipTransform = BlasterCharacter->GetCurrentWeapon()->GetMesh()->GetSocketTransform(MuzzleFlashSocketName, ERelativeTransformSpace::RTS_World);
        // FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

        // DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.0f, FColor::Yellow, false, -1.0f, (uint8)0U, 3.0f);
        // DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), BlasterCharacter->GetHitTarget(), FColor::Orange, false, -1.0f, (uint8) 0U, 3.0f);
    }
}
