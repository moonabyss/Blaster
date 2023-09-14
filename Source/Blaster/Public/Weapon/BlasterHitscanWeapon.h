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
    virtual void Fire(const FVector& BarelLocation, const FVector& HitTarget, float SpreadAngle) override;

private:
    UPROPERTY(Category = "Bullet Properties", EditDefaultsOnly)
    FBulletProps BulletProps{FBulletProps()};

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SpawnImpactFX(UParticleSystem* ImpactParticles, USoundCue* ImpactSound, const FTransform& ImpactTransform);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SpawnBeamFX(UParticleSystem* ImpactParticles, const FVector& BeamStart, const FVector& BeamEnd);
};
