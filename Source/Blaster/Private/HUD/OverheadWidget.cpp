// Blaster Multiplayer Game. All rights reserved.

#include "HUD/OverheadWidget.h"
#include "Components/TextBlock.h"

void UOverheadWidget::NativeDestruct()
{
    RemoveFromParent();
    Super::NativeDestruct();
}

void UOverheadWidget::SetDisplayText(FString TextToDisplay) {
    if (!DisplayText) return;

    DisplayText->SetText(FText::FromString(TextToDisplay));
}
