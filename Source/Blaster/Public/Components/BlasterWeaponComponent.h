// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

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

protected:
    virtual void BeginPlay() override;

public:
    void SetCharacter(ABlasterCharacter* BlasterCharacter);
    void EquipWeapon(ABlasterBaseWeapon* WeaponToEquip);

private:
    TObjectPtr<ABlasterCharacter> Character;
    
    UPROPERTY(VisibleAnywhere)
    ABlasterBaseWeapon* EquippedWeapon;

    UPROPERTY(EditDefaultsOnly)
    FName WeaponRightHandSocket{FName("Weapon_R")};
};
