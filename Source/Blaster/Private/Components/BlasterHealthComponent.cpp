// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterHealthComponent.h"
#include "Net/UnrealNetwork.h"

#include "Character/BlasterCharacter.h"

DEFINE_LOG_CATEGORY_STATIC(LogBlasterHealthComponent, All, All);

UBlasterHealthComponent::UBlasterHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME(UBlasterHealthComponent, Health);
}

void UBlasterHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    check(Character);

    SetHealth(MaxHealth);
}

void UBlasterHealthComponent::SetCharacter(ABlasterCharacter* BlasterCharacter)
{
    Character = BlasterCharacter;
}

void UBlasterHealthComponent::SetHealth(float InHealth) 
{
    if (!Character && !Character->HasAuthority()) return;

    Health = InHealth;
}

void UBlasterHealthComponent::OnRep_Health(float PrevHealth) 
{
    UE_LOG(LogBlasterHealthComponent, Display, TEXT("Health: %f"), Health);
    HealthChangedDelegate.Broadcast(Health);
}
