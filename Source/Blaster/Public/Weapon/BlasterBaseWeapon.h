// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterTypes/BlasterWeaponTypes.h"

#include "BlasterBaseWeapon.generated.h"

class UAnimationAsset;
class USphereComponent;
class UWidgetComponent;
class ABlasterWeaponShell;
class UParticleSystem;
class USoundCue;

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
    virtual void Fire(const FVector& BarelLocation, const FVector& HitTarget, float SpreadAngle){};
    void DecrementAmmo();
    bool TryAddAmmoToClip(int32 AmmoCount);

    // Getters
    FWeaponProps GetWeaponProps() const { return WeaponProperies; };
    USkeletalMeshComponent* GetMesh() const { return WeaponMesh; };
    int32 GetAmmoInCLip() const { return AmmoInClip; }
    int32 GetClipCapacity() const { return WeaponProperies.ClipCapacity; }
    bool IsClipFull() const { return AmmoInClip == WeaponProperies.ClipCapacity; }
    bool IsClipEmpty() const { return AmmoInClip == 0; }

protected:
    UFUNCTION()
    virtual void OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    FVector ShotDirectionWithSpread(const FVector& Direction, float SpreadAngle) const;

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_PlayWeaponFireAnimation();

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SpawnImpactFX(UParticleSystem* ImpactParticles, USoundCue* ImpactSound, const FTransform& ImpactTransform);

    UFUNCTION(NetMulticast, Reliable)
    void Multicast_SpawnBeamFX(UParticleSystem* ImpactParticles, const FVector& BeamStart, const FVector& BeamEnd);

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

    UPROPERTY(Category = "Weapon Properties", EditAnywhere, ReplicatedUsing = OnRep_AmmoInClip)
    int32 AmmoInClip{0};

    UFUNCTION()
    void OnRep_AmmoInClip();
};
