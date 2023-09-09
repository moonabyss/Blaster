// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterBaseWeapon.h"

#include "BlasterHitscanWeapon.generated.h"

class UParticleSystem;
class USoundCue;

UCLASS(Blueprintable)
class BLASTER_API ABlasterHitscanWeapon : public ABlasterBaseWeapon
{
    GENERATED_BODY()

public:
    virtual void Fire(const FVector& BarelLocation, const FVector& HitTarget) override;

private:
    UPROPERTY(Category = "Bullet Properties", EditDefaultsOnly)
    FBulletProps BulletProps{FBulletProps()};

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayImpactFX(UParticleSystem* ImpactParticles, USoundCue* ImpactSound, FTransform ImpactTransform);
};
