// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Weapon/BlasterProjectile.h"

#include "BlasterProjectileRocket.generated.h"

class UAudioComponent;
class UNiagaraComponent;
class UNiagaraSystem;
class UStaticMeshComponent;

UCLASS()
class BLASTER_API ABlasterProjectileRocket : public ABlasterProjectile
{
    GENERATED_BODY()

public:
    ABlasterProjectileRocket();

protected:
    virtual void BeginPlay() override;
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UStaticMeshComponent* RocketMesh;

    UPROPERTY(Category = "Projectile Properties", EditDefaultsOnly)
    FRocketProps RocketProps{FRocketProps()};

    FTimerHandle DestroyTimer;

    UPROPERTY(EditDefaultsOnly)
    float DestroyDelay{5.0};

    UFUNCTION()
    void DestroyTimerFinished();

    UPROPERTY()
    UNiagaraComponent* TrailFXComponent{nullptr};

    UPROPERTY()
    UAudioComponent* LoopSoundComponent{nullptr};
};
