// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterBaseWeapon.h"

#include "BlasterMultiscanWeapon.generated.h"

UCLASS()
class BLASTER_API ABlasterMultiscanWeapon : public ABlasterBaseWeapon
{
    GENERATED_BODY()

public:
    virtual void Fire(const FVector& BarelLocation, const FVector& HitTarget, float SpreadAngle);
};
