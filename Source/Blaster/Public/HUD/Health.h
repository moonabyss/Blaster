// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "Health.generated.h"

class UProgressBar;
class UTextBlock;

UCLASS()
class BLASTER_API UHealth : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

public:
    UPROPERTY(meta = (BindWidget))
    UProgressBar* HealthBar;

    UPROPERTY(meta = (BindWidget))
    UTextBlock* HealthText;

private:
    void OnHealthChanged(float NewHealth, float MaxHealth);
};
