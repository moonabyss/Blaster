// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "CharacterOverlay.generated.h"

UCLASS()
class BLASTER_API UCharacterOverlay : public UUserWidget
{
    GENERATED_BODY()

public:
    UPROPERTY(meta = (BindWidget))
    UUserWidget* WBP_Health;
};
