// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "BlasterTypes/BlasterWeaponTypes.h"

#include "BlasterProjectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
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

protected:
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UBoxComponent* GetCollisionBox() { return CollisionBox; }
    UProjectileMovementComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

    void PlayImpactFX(UParticleSystem* ImpactParticles, USoundCue* ImpactSound);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBoxComponent* CollisionBox;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovementComponent;

    FVector ShotDirection{FVector()};
};
