// Blaster Multiplayer Game. All rights reserved.

#include "HUD/Announcement.h"

#include "Character/BlasterPlayerController.h"

void UAnnouncement::NativeOnInitialized()
{
    Super::NativeOnInitialized();

    BlasterPlayerController = Cast<ABlasterPlayerController>(GetOwningPlayer());
}

FText UAnnouncement::GetCountdown() 
{
    float CountdownTime = 0.0f;
    if (IsValid(BlasterPlayerController))
    {
        CountdownTime = BlasterPlayerController->GetLeftWarmupTime();
    }

    const int32 Minutes = FMath::FloorToInt(CountdownTime / 60.0f);
    const int32 Seconds = FMath::FloorToInt(CountdownTime - Minutes * 60);

    return FText::FromString(FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds));
}
