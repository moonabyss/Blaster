// Blaster Multiplayer Game. All rights reserved.

#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!GEngine) return;

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);
    const FVector2D ViewportCenter(ViewportSize / 2.0f);

    const auto PC = GetOwningPlayerController();
    float FOVAngle = PC->PlayerCameraManager->GetFOVAngle();
    const float Spread = ViewportSize.X / FOVAngle * Crosshairs.SpreadAngle;

    if (Crosshairs.Center)
    {
        FVector2D Spreads(0.0f);
        DrawCrosshair(Crosshairs.Center, ViewportCenter, Spreads);
    }
    if (Crosshairs.Left)
    {
        FVector2D Spreads(-Spread, 0.0f);
        DrawCrosshair(Crosshairs.Left, ViewportCenter, Spreads);
    }
    if (Crosshairs.Right)
    {
        FVector2D Spreads(Spread, 0.0f);
        DrawCrosshair(Crosshairs.Right, ViewportCenter, Spreads);
    }
    if (Crosshairs.Top)
    {
        FVector2D Spreads(0.0f, -Spread);
        DrawCrosshair(Crosshairs.Top, ViewportCenter, Spreads);
    }
    if (Crosshairs.Bottom)
    {
        FVector2D Spreads(0.0f, Spread);
        DrawCrosshair(Crosshairs.Bottom, ViewportCenter, Spreads);
    }
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter, const FVector2D& Spreads)
{
    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextuteDrawPoint(
        ViewportCenter.X - TextureWidth / 2.0f + Spreads.X,
        ViewportCenter.Y - TextureHeight / 2.0f + Spreads.Y
    );

    DrawTexture(Texture, TextuteDrawPoint.X, TextuteDrawPoint.Y, TextureWidth, TextureHeight, 0.0f, 0.0f, 1.0f, 1.0f);
}
