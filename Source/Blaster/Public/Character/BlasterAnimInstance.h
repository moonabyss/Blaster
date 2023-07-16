// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"

#include "BlasterCoreTypes.h"

#include "BlasterAnimInstance.generated.h"

class ABlasterCharacter;

UCLASS()
class BLASTER_API UBlasterAnimInstance : public UAnimInstance
{
    GENERATED_BODY()

public:
    virtual void NativeInitializeAnimation() override;
    virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
    UPROPERTY(Category = "Character", BlueprintReadOnly)
    float Speed{0.0f};
    float SpeedLastFrame{0.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsInAir{false};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsAccelerating{false};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsWeaponEquipped{false};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsCrouched{false};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsAiming{false};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    float AimSpeedModifier{1.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    float YawOffset{0.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    float Lean{0.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    float AO_Yaw{0.0f};
    float AO_YawLastFrame{0.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    float AO_Pitch{0.0f};
    float AO_PitchLastFrame{0.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    TObjectPtr<ABlasterCharacter> BlasterCharacter;

    FRotator CharacterRotationLastFrame{FRotator()};
    FRotator CharacterRotation{FRotator()};
    FRotator DeltaRotation{FRotator()};

    UPROPERTY(Category = "Movement", EditAnywhere)
    float YawInterpSpeed{6.0f};

    UPROPERTY(Category = "Movement", EditAnywhere)
    float LeanInterpSpeed{3.0f};
    
    UPROPERTY(Category = "Movement", EditAnywhere)
    float AimInterpSpeed{10.0f};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    FTransform LeftHandTransform{FTransform()};

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    ETurningInPlace TurningInPlace{ETurningInPlace::ETIP_NotTurning};
};
