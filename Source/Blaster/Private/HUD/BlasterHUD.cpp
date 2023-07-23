// Blaster Multiplayer Game. All rights reserved.

#include "HUD/BlasterHUD.h"

void ABlasterHUD::DrawHUD()
{
    Super::DrawHUD();

    if (!GEngine) return;

    FVector2D ViewportSize;
    GEngine->GameViewport->GetViewportSize(ViewportSize);
    const FVector2D ViewportCenter(ViewportSize / 2.0f);

    if (Crosshairs.Center)
    {
        DrawCrosshair(Crosshairs.Center, ViewportCenter);
    }
    if (Crosshairs.Left)
    {
        DrawCrosshair(Crosshairs.Left, ViewportCenter);
    }
    if (Crosshairs.Right)
    {
        DrawCrosshair(Crosshairs.Right, ViewportCenter);
    }
    if (Crosshairs.Top)
    {
        DrawCrosshair(Crosshairs.Top, ViewportCenter);
    }
    if (Crosshairs.Bottom)
    {
        DrawCrosshair(Crosshairs.Bottom, ViewportCenter);
    }
}

void ABlasterHUD::DrawCrosshair(UTexture2D* Texture, const FVector2D& ViewportCenter) 
{
    const float TextureWidth = Texture->GetSizeX();
    const float TextureHeight = Texture->GetSizeY();
    const FVector2D TextuteDrawPoint(
        ViewportCenter.X - TextureWidth / 2.0f,
        ViewportCenter.Y - TextureHeight / 2.0f
    );

    DrawTexture(Texture, TextuteDrawPoint.X, TextuteDrawPoint.Y, TextureWidth, TextureHeight, 0.0f, 0.0f, 1.0f, 1.0f);
}
