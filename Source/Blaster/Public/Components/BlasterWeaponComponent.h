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
    bool IsWeaponEquipped() const;
    ABlasterBaseWeapon* GetCurrentWeapon() const;
    void StartAiming();
    void StopAiming();
    bool IsAiming();
    void StartFire();
    void StopFire();
    void Fire();

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

    void TraceUnderCrosshairs(FHitResult& TraceHitResult);

    void SetHUDCrosshairs(float DeltaTime);

    float CalculateCurrentSpreadModifier(float DeltaTime);
    float CrosshairsVelocityFactor{0.0f};
    float CrosshairsInAirFactor{0.0f};
};
