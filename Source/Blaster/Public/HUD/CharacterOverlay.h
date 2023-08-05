// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "CharacterOverlay.generated.h"

class UBlasterHealthComponent;

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
    float GetScore();

private:
    UBlasterHealthComponent* HealthComponent{nullptr};
};
