// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "BlasterCoreTypes.h"

#include "BlasterWeaponComponent.generated.h"

class ABlasterBaseWeapon;
class ABlasterCharacter;
class ABlasterPlayerController;
class ABlasterHUD;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UBlasterWeaponComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBlasterWeaponComponent();
    virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

public:
    FOnWeaponEquippedDelegate WeaponEquipped;
    FOnWeaponUnequippedDelegate WeaponUnequipped;
    void SetCharacter(ABlasterCharacter* BlasterCharacter);
    void EquipWeapon(ABlasterBaseWeapon* WeaponToEquip);
    void DropWeapon();
    bool IsWeaponEquipped() const;
    ABlasterBaseWeapon* GetCurrentWeapon() const;
    void StartAiming();
    void StopAiming();
    bool IsAiming();
    void StartFire();
    void StopFire();
    void Fire();

    FVector GetHitTargetNoSpread() const { return HitTargetNoSpread; }
    // for currently equipped weapon
    int32 GetCarriedAmmo() { return CarriedAmmo; }

protected:
private:
    TObjectPtr<ABlasterCharacter> Character{nullptr};
    TObjectPtr<ABlasterPlayerController> Controller{nullptr};
    TObjectPtr<ABlasterHUD> HUD{nullptr};

    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentWeapon)
    ABlasterBaseWeapon* CurrentWeapon{nullptr};

    UFUNCTION()
    void OnRep_CurrentWeapon(ABlasterBaseWeapon* LastWeapon);

    UPROPERTY(EditDefaultsOnly)
    FName WeaponRightHandSocket{FName("Weapon_R")};

    void AttachWeaponToSocket(ABlasterBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
    void DetachWeapon(ABlasterBaseWeapon* Weapon);

    UPROPERTY(Replicated)
    bool bWantsAiming{false};

    UFUNCTION(Server, Reliable)
    void ServerSetWantsAiming(bool bIsAiming);

    UPROPERTY(Replicated)
    bool bWantsFire{false};

    UFUNCTION(Server, Reliable)
    void ServerSetWantsFire(bool bIsFiring);

    void PlayFireMontage();

    UFUNCTION(Server, Reliable)
    void ServerFire(const FVector_NetQuantize& TraceHitTarget);

    UFUNCTION(NetMulticast, Reliable)
    void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

    void TraceUnderCrosshairs(FHitResult& TraceHitResult, bool bWithSpread);

    void SetHUDCrosshairs(float DeltaTime);

    FCrosshairs Crosshairs{FCrosshairs()};
    float CalculateCurrentSpreadModifier(float DeltaTime);
    float CurrentSpreadAngle{0.0f};
    float CrosshairsVelocityFactor{0.0f};
    float CrosshairsInAirFactor{0.0f};
    float CrosshairsAimFactor{0.0f};
    float CrosshairsShootingFactor{0.0f};

    FVector HitTarget;
    FVector HitTargetNoSpread;

    UPROPERTY(Category = "Combat", EditDefaultsOnly)
    float DefaultFOV{90.0f};

    float CurrentFOV{90.0f};

    UPROPERTY(Category = "Combat", EditDefaultsOnly)
    float DefaultInterpSpeed{20.0f};

    void InterpFOV(float DeltaTime);

    UPROPERTY(Category = "UI|Crosshairs", EditDefaultsOnly)
    FColor CrosshairsDefaultColor{FColor::White};

    UPROPERTY(Category = "UI|Crosshairs", EditDefaultsOnly)
    FColor CrosshairsEnemyColor{FColor::Red};

    FTimerHandle FireTimer;

    void StartFireTimer();
    void FireTimerFinished();
    bool bCanFire{true};
    bool CanShoot() const;

    // Carried ammo for the currently equipped weapon
    UPROPERTY(Replicated)
    int32 CarriedAmmo{0};

    UPROPERTY(Category = "Combat", EditDefaultsOnly)
    TMap<EWeaponType, int32> LoadoutMap;

    UPROPERTY(Category = "Combat", VisibleAnywhere)
    TMap<EWeaponType, int32> CarriedAmmoMap;
    void InitializeAmmoMap();
};
