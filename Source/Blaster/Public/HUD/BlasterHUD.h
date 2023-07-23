// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"

#include "BlasterCoreTypes.h"

#include "BlasterHUD.generated.h"

UCLASS()
class BLASTER_API ABlasterHUD : public AHUD
{
    GENERATED_BODY()

public:
    virtual void DrawHUD() override;

    void SetCrosshairs(const FCrosshairs& InCrosshairs) { Crosshairs = InCrosshairs; };

private:
    FCrosshairs Crosshairs;
};
