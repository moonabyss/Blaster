// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "BlasterTypes/BlasterWeaponTypes.h"

#include "BlasterProjectile.generated.h"

class UBoxComponent;
class UBlasterProjectileMoveComponent;
class UParticleSystem;
class USoundCue;

UCLASS()
class BLASTER_API ABlasterProjectile : public AActor
{
    GENERATED_BODY()

public:
    ABlasterProjectile();

protected:
    virtual void BeginPlay() override;

public:
    void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }
    float GetInitialSpeed() const;

protected:
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UBoxComponent* GetCollisionBox() { return CollisionBox; }
    UBlasterProjectileMoveComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayImpactFX(UParticleSystem* ImpactParticles, USoundCue* ImpactSound);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBoxComponent* CollisionBox;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBlasterProjectileMoveComponent* ProjectileMovementComponent;

    FVector ShotDirection{FVector()};
};
