// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterBaseWeapon.h"

#include "BlasterProjectileWeapon.generated.h"

class ABlasterProjectile;

UCLASS(Blueprintable)
class BLASTER_API ABlasterProjectileWeapon : public ABlasterBaseWeapon
{
    GENERATED_BODY()

public:
    virtual void Fire(const FVector& HitTarget) override;

private:
    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ABlasterProjectile> ProjectileClass;

    void SpawnProjectile(const FVector& SocketLocation, const FVector& HitTarget);
};
