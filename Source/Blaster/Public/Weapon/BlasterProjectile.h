// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterCoreTypes.h"

#include "BlasterProjectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystemComponent;

UCLASS()
class BLASTER_API ABlasterProjectile : public AActor
{
    GENERATED_BODY()

public:
    ABlasterProjectile();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

public:
    void SetShotDirection(const FVector& Direction) { ShotDirection = Direction; }

protected:
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UPROPERTY(Category = "Bullet Properties", EditDefaultsOnly)
    float Damage{0.0f};

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBoxComponent* CollisionBox;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(Category = "Bullet Properties", EditDefaultsOnly)
    FBulletProps BulletProps{FBulletProps()};

    UParticleSystemComponent* TracerComponent;

    void SpawnParticles();

    void SpawnSound();

    FVector ShotDirection{FVector()};
};
