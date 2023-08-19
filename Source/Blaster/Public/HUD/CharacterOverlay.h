// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "CharacterOverlay.generated.h"

class ABlasterPlayerController;
class ABlasterPlayerState;
class UBlasterHealthComponent;
class UBlasterWeaponComponent;

UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeOnInitialized() override;

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
    int32 GetCarriedAmmo();

    UFUNCTION(Category = "UI", BlueprintPure)
    bool ShowAmmoWidget() const;

    UFUNCTION(Category = "UI", BlueprintPure)
    FText GetMatchCountdown();

private:
    UBlasterHealthComponent* HealthComponent{nullptr};
    UBlasterWeaponComponent* WeaponComponent{nullptr};
    ABlasterPlayerState* BlasterPlayerState{nullptr};
    ABlasterPlayerController* BlasterPlayerController{nullptr};
};
