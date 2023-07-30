// Blaster Multiplayer Game. All rights reserved.

#include "HUD/BlasterHUD.h"
#include "GameFramework/PlayerController.h"

#include "HUD/CharacterOverlay.h"

void ABlasterHUD::BeginPlay() 
{
    Super::BeginPlay();

    AddCharacterOverlay();
}

void ABlasterHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!GEngine) return;

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);
    const FVector2D ViewportCenter(ViewportSize / 2.0f);

    const auto PC = GetOwningPlayerController();
    float FOVAngle = PC->PlayerCameraManager->GetFOVAngle();
    const float Spread = ViewportSize.X / FOVAngle * Crosshairs.SpreadAngle / 2.0f;
    CurrentSpread = FMath::FInterpTo(CurrentSpread, Spread, GetWorld()->DeltaTimeSeconds, 10.0f);

    if (Crosshairs.Center)
    {
        FVector2D Spreads(0.0f);
        DrawCrosshair(Crosshairs.Center, ViewportCenter, Spreads, Crosshairs.Color);
    }
    if (Crosshairs.Left)
    {
        FVector2D Spreads(-CurrentSpread, 0.0f);
        DrawCrosshair(Crosshairs.Left, ViewportCenter, Spreads, Crosshairs.Color);
    }
    if (Crosshairs.Right)
    {
        FVector2D Spreads(CurrentSpread, 0.0f);
        DrawCrosshair(Crosshairs.Right, ViewportCenter, Spreads, Crosshairs.Color);
    }
    if (Crosshairs.Top)
    {
        FVector2D Spreads(0.0f, -CurrentSpread);
        DrawCrosshair(Crosshairs.Top, ViewportCenter, Spreads, Crosshairs.Color);
    }
    if (Crosshairs.Bottom)
    {
        FVector2D Spreads(0.0f, CurrentSpread);
        DrawCrosshair(Crosshairs.Bottom, ViewportCenter, Spreads, Crosshairs.Color);
    }
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spreads, const FColor& SpreadColor)
{
    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextuteDrawPoint(ViewportCenter.X - TextureWidth / 2.0f + Spreads.X, ViewportCenter.Y - TextureHeight / 2.0f + Spreads.Y);

    DrawTexture(Texture, TextuteDrawPoint.X, TextuteDrawPoint.Y, TextureWidth, TextureHeight, 0.0f, 0.0f, 1.0f, 1.0f, SpreadColor);
}

void ABlasterHUD::AddCharacterOverlay() 
{
    if (!CharacterOverlayClass) return;

    if (auto PC = GetOwningPlayerController())
    {
        CharacterOverlay = CreateWidget<UCharacterOverlay>(PC, CharacterOverlayClass);
        CharacterOverlay->AddToViewport();
    }
}
