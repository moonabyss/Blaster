// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "Announcement.generated.h"

class ABlasterPlayerController;

UCLASS()
class BLASTER_API UAnnouncement : public UUserWidget
{
    GENERATED_BODY()

public:
    void NativeOnInitialized() override;

protected:
    UFUNCTION(Category = "UI", BlueprintPure)
    FText GetCountdown();

private:
    ABlasterPlayerController* BlasterPlayerController{nullptr};
};
