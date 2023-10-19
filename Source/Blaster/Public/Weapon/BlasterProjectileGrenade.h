// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterProjectile.h"

#include "BlasterProjectileGrenade.generated.h"

class UProjectileMovementComponent;

UCLASS()
class BLASTER_API ABlasterProjectileGrenade : public ABlasterProjectile
{
    GENERATED_BODY()

public:
    ABlasterProjectileGrenade();

    virtual float GetInitialSpeed() const override;

protected:
    virtual void BeginPlay() override;
    virtual void DestroyTimerFinished() override;

    UFUNCTION()
    void OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMoveComponent;

    UPROPERTY(Category = "Projectile Properties", EditDefaultsOnly)
    FGrenadeProps GrenadeProps;
};
