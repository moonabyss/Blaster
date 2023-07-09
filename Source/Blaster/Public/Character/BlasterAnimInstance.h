// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Animation/AnimInstance.h"
#include "CoreMinimal.h"

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
    float Speed;

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsInAir;

    UPROPERTY(Category = "Character", BlueprintReadOnly)
    bool bIsAccelerating;

private:
    UPROPERTY()
    TObjectPtr<ABlasterCharacter> BlasterCharacter;
};
