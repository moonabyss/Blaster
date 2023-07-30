// Blaster Multiplayer Game. All rights reserved.

#include "HUD/Health.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "GameFramework/PlayerController.h"

#include "Character/BlasterCharacter.h"
#include "Components/BlasterHealthComponent.h"

bool UHealth::Initialize() 
{
    if (!Super::Initialize())
    {
        return false;
    }

    if (auto Pawn = GetOwningPlayerPawn())
    {
        auto Character = Cast<ABlasterCharacter>(Pawn);
        if (Character)
        {
            auto HealthComponent = Cast<UBlasterHealthComponent>(Character->GetComponentByClass(UBlasterHealthComponent::StaticClass()));
            if (HealthComponent)
            {
                HealthComponent->HealthChangedDelegate.AddUObject(this, &ThisClass::OnHealthChanged);
            }
        }
    }

    return true;
}

void UHealth::OnHealthChanged(float NewHealth, float MaxHealth)
{
    const auto Health = NewHealth / 100.0f;

    HealthBar->SetPercent(Health);
    HealthText->SetText(FText::FromString(FString::Printf(TEXT("%.0f / %.0f"), NewHealth, MaxHealth)));
}
