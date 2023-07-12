// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "BlasterCoreTypes.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterBaseWeapon.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class BLASTER_API ABlasterBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ABlasterBaseWeapon();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    virtual void OnSphereBeginOverlap(
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    USphereComponent* AreaSphere;

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere)
    EWeaponState WeaponState{EWeaponState::EWS_Initial};

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere)
    UWidgetComponent* PickupWidget;
};
