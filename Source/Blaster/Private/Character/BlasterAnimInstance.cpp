// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterAnimInstance.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include "BlasterUtils.h"
#include "Character/BlasterCharacter.h"

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

    FVector Velocity = BlasterCharacter->GetVelocity().GetSafeNormal2D();
    Speed = Velocity.Size();

    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

    bIsAccelerating = !FMath::IsNearlyZero(BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size());

    bIsWeaponEquipped = BlasterCharacter && BlasterCharacter->IsWeaponEquipped();

    bIsCrouched = BlasterCharacter->bIsCrouched;

    bIsAiming = BlasterCharacter->IsAiming();

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

    AO_Yaw = BlasterCharacter->GetAimYaw();
    AO_Pitch = BlasterCharacter->GetAimPitch();
}
