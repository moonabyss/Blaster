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
    void SetCharacter(ABlasterCharacter* BlasterCharacter);
    void EquipWeapon(ABlasterBaseWeapon* WeaponToEquip);
    bool IsWeaponEquipped() const;
    EWeaponType GetEquippedWeaponType() const;

private:
    TObjectPtr<ABlasterCharacter> Character;
    
    UPROPERTY(VisibleAnywhere, Replicated)
    ABlasterBaseWeapon* CurrentWeapon;

    UPROPERTY(EditDefaultsOnly)
    FName WeaponRightHandSocket{FName("Weapon_R")};

    void AttachWeaponToSocket(ABlasterBaseWeapon* Weapon, USceneComponent* SceneComponent, const FName& SocketName);
};
