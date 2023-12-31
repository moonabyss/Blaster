// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Blueprint/UserWidget.h"
#include "CoreMinimal.h"

#include "OverheadWidget.generated.h"

class UTextBlock;

UCLASS()
class BLASTER_API UOverheadWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    void SetDisplayText(FString TextToDisplay);

protected:
    virtual void NativeDestruct() override;

private:
    UPROPERTY(meta = (BindWidget))
    UTextBlock* DisplayText;
};
