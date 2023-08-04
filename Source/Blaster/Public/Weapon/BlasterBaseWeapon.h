// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterCoreTypes.h"

#include "BlasterBaseWeapon.generated.h"

class UAnimationAsset;
class USphereComponent;
class UWidgetComponent;
class ABlasterWeaponShell;

UCLASS(NotBlueprintable)
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
    virtual void Fire(const FVector& HitTarget);

    // Getters
    FWeaponProps GetWeaponProps() const { return WeaponProperies; };
    USkeletalMeshComponent* GetMesh() const { return WeaponMesh; };

protected:
    UFUNCTION()
    virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USkeletalMeshComponent* WeaponMesh;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    USphereComponent* AreaSphere;

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere, ReplicatedUsing = OnRep_WeaponState)
    EWeaponState WeaponState{EWeaponState::EWS_Initial};

    UFUNCTION()
    void OnRep_WeaponState();

    UPROPERTY(Category = "Weapon Properties", EditAnywhere)
    FWeaponProps WeaponProperies{FWeaponProps()};

    UPROPERTY(Category = "Weapon Properties", VisibleAnywhere)
    UWidgetComponent* PickupWidget;

    void PlayFireAnimation();
    void SpawnShell();

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ABlasterWeaponShell> ShellClass;
};
