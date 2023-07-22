// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterWeaponShell.generated.h"

class USoundBase;

UCLASS()
class BLASTER_API ABlasterWeaponShell : public AActor
{
    GENERATED_BODY()

public:
    ABlasterWeaponShell();

protected:
    virtual void BeginPlay() override;
    UFUNCTION()
    virtual void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UStaticMeshComponent* ShellMesh;

    UPROPERTY(Category = "Weapon Shell", EditDefaultsOnly)
    float ShellEjectionImpulse{10.0f};

    UPROPERTY(Category = "Weapon Shell", EditDefaultsOnly)
    float LifeSpanTime{1.0f};

    UPROPERTY(Category = "Weapon Shell", EditDefaultsOnly)
    USoundBase* ShellSound;
};
