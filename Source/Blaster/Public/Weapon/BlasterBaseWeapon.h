// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterCoreTypes.h"

#include "BlasterBaseWeapon.generated.h"

class USphereComponent;
class UWidgetComponent;

UCLASS()
class BLASTER_API ABlasterBaseWeapon : public AActor
{
    GENERATED_BODY()

public:
    ABlasterBaseWeapon();
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

public:
    void ShowPickupWidget(bool bShowWidget);
    void SetWeaponState(EWeaponState State);
    EWeaponType GetWeaponType() const;

protected:
    UFUNCTION()
    virtual void OnSphereBeginOverlap(
        UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    USphereComponent* AreaSphere;

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere, ReplicatedUsing = OnRep_WeaponState)
    EWeaponState WeaponState{EWeaponState::EWS_Initial};

    UFUNCTION(Server, Reliable)
    void OnRep_WeaponState();

    UPROPERTY(Category = "Weapon Properties", EditDefaultsOnly)
    EWeaponType WeaponType{EWeaponType::EWT_Rifle};

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere)
    UWidgetComponent* PickupWidget;
};
