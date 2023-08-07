// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "CharacterOverlay.generated.h"

class UBlasterHealthComponent;
class UBlasterWeaponComponent;

UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
    GENERATED_BODY()

protected:
    UFUNCTION(Category = "UI", BlueprintPure)
    float GetHealth();

    UFUNCTION(Category = "UI", BlueprintPure)
    float GetMaxHealth();

    UFUNCTION(Category = "UI", BlueprintPure)
    float GetHealthPercent();

    UFUNCTION(Category = "UI", BlueprintPure)
    float GetScore() const;

    UFUNCTION(Category = "UI", BlueprintPure)
    int32 GetKilled() const;

    UFUNCTION(Category = "UI", BlueprintPure)
    int32 GetDefeats() const;

    UFUNCTION(Category = "UI", BlueprintPure)
    int32 GetAmmoInCLip();

    UFUNCTION(Category = "UI", BlueprintPure)
    int32 GetClipCapacity();

    UFUNCTION(Category = "UI", BlueprintPure)
    bool ShowAmmoWidget() const;

private:
    UBlasterHealthComponent* HealthComponent{nullptr};
    UBlasterWeaponComponent* WeaponComponent{nullptr};
};
