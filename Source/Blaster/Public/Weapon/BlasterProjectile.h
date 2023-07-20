// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterProjectile.generated.h"

class UBoxComponent;
class UProjectileMovementComponent;
class UParticleSystem;
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

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBoxComponent* CollisionBox;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UProjectileMovementComponent* ProjectileMovementComponent;

    UPROPERTY(Category = "Bullet Properties", EditDefaultsOnly)
    float BulletSpeed{15000.0f};

    UPROPERTY(Category = "Bullet Properties", EditDefaultsOnly)
    UParticleSystem* BulletTracer;

    UParticleSystemComponent* TracerComponent;
};
