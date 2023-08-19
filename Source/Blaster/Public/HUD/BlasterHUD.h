// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "BlasterCoreTypes.h"

#include "BlasterHUD.generated.h"

class UCharacterOverlay;

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

protected:
    virtual void BeginPlay() override;

public:
    void AddCharacterOverlay();
    void SetCrosshairs(const FCrosshairs& InCrosshairs) { Crosshairs = InCrosshairs; }

private:
    FCrosshairs Crosshairs;

    void DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spreads, const FColor& SpreadColor = FColor::White);
    float CurrentSpread{0.0f};

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<UUserWidget> CharacterOverlayClass;

    UPROPERTY()
    UCharacterOverlay* CharacterOverlay;
};
