// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "Announcement.generated.h"

class ABlasterPlayerController;
class UTextBlock;

UCLASS()
class BLASTER_API UAnnouncement : public UUserWidget
{
    GENERATED_BODY()

public:
    void NativeOnInitialized() override;

public:
    void SetTitleText(FText Text);
    void SetInfoText(FText Text);

protected:
    UFUNCTION(Category = "UI", BlueprintPure)
    FText GetCountdown();

private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* TitleText;
    
    UPROPERTY(meta = (BindWidget))
    UTextBlock* InfoText;

    ABlasterPlayerController* BlasterPlayerController{nullptr};
};
