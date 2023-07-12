// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlasterCoreTypes.h"

#include "BlasterBaseWeapon.generated.h"

class USphereComponent;

UCLASS()
class BLASTER_API ABlasterBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ABlasterBaseWeapon();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    USphereComponent* AreaSphere;

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere)
    EWeaponState WeaponState{EWeaponState::EWS_Initial};
};
