// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "BlasterCoreTypes.h"

#include "BlasterWeaponComponent.generated.h"

class ABlasterBaseWeapon;
class ABlasterCharacter;

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
    FOnWeaponAimingDelegate WeaponAiming;
    void SetCharacter(ABlasterCharacter* BlasterCharacter);
    void EquipWeapon(ABlasterBaseWeapon* WeaponToEquip);
    bool IsWeaponEquipped() const;
    ABlasterBaseWeapon* GetCurrentWeapon() const;
    void StartAiming();
    void StopAiming();
    bool IsAiming();

protected:
    void SetAiming(bool bIsAiming);

    UFUNCTION(Server, Reliable)
    void ServerSetAiming(bool bIsAiming);

private:
    TObjectPtr<ABlasterCharacter> Character{nullptr};
    
    UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_CurrentWeapon)
    ABlasterBaseWeapon* CurrentWeapon{nullptr};

    UFUNCTION()
    void OnRep_CurrentWeapon(ABlasterBaseWeapon* LastWeapon);

    UPROPERTY(EditDefaultsOnly)
    FName WeaponRightHandSocket{FName("Weapon_R")};

    void AttachWeaponToSocket(ABlasterBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);

    UPROPERTY(Replicated)
    bool bWantsAiming{false};
};