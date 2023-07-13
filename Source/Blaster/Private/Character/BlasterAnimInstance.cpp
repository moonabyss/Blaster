// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterAnimInstance.h"
#include "BlasterUtils.h"
#include "Character/BlasterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

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

    FVector Velocity = BlasterCharacter->GetVelocity().GetSafeNormal2D();
    Speed = Velocity.Size();

    bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();

    bIsAccelerating = !FMath::IsNearlyZero(BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size());

    bIsWeaponEquipped = BlasterCharacter && BlasterCharacter->IsWeaponEquipped();

    bIsCrouched = BlasterCharacter->bIsCrouched;
}

ABlasterCharacter* UBlasterAnimInstance::GetCharacter() const
{
    return BlasterCharacter;
}
