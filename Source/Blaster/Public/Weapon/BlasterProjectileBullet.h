// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterProjectile.h"

#include "BlasterProjectileBullet.generated.h"

class UParticleSystemComponent;

UCLASS()
class BLASTER_API ABlasterProjectileBullet : public ABlasterProjectile
{
    GENERATED_BODY()

protected:
    virtual void BeginPlay() override;

protected:
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

private:
    UPROPERTY(Category = "Projectile Properties", EditDefaultsOnly)
    FBulletProps BulletProps{FBulletProps()};

    UParticleSystemComponent* TracerComponent{nullptr};
};
