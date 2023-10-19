// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "BlasterTypes/BlasterWeaponTypes.h"

#include "BlasterProjectile.generated.h"

class UAudioComponent;
class UBoxComponent;
class UBlasterProjectileMoveComponent;
class UNiagaraComponent;
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
    virtual float GetInitialSpeed() const;

protected:
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    UBoxComponent* GetCollisionBox() { return CollisionBox; }
    UBlasterProjectileMoveComponent* GetProjectileMovementComponent() { return ProjectileMovementComponent; }

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayImpactFX(UParticleSystem* ImpactParticles, USoundCue* ImpactSound);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_StopTrailFX();

    void SpawnFX(UNiagaraSystem* VisualFX, USoundBase* LoopSound);

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UStaticMeshComponent* ProjectileMesh;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBoxComponent* CollisionBox;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBlasterProjectileMoveComponent* ProjectileMovementComponent;

    void StartDestroyTimer();

    FTimerHandle DestroyTimer;

    UPROPERTY(EditDefaultsOnly)
    float DestroyDelay{5.0};

    UFUNCTION()
    virtual void DestroyTimerFinished();

    UPROPERTY()
    UNiagaraComponent* TrailFXComponent{nullptr};

    UPROPERTY()
    UAudioComponent* LoopSoundComponent{nullptr};

    FVector ShotDirection{FVector()};
};
