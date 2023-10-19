// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterProjectile.h"

#include "BlasterProjectileRocket.generated.h"

class UNiagaraSystem;

UCLASS()
class BLASTER_API ABlasterProjectileRocket : public ABlasterProjectile
{
    GENERATED_BODY()

public:
    ABlasterProjectileRocket();

protected:
    virtual void BeginPlay() override;
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
    UPROPERTY(Category = "Projectile Properties", EditDefaultsOnly)
    FRocketProps RocketProps{FRocketProps()};
};
