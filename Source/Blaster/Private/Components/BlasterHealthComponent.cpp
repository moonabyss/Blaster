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
    HealthChangedDelegate.Broadcast(Health, MaxHealth);

    if (AActor* ComponentOwner = GetOwner())
    {
        ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamageHandle);
    }
}

void UBlasterHealthComponent::SetCharacter(ABlasterCharacter* BlasterCharacter)
{
    Character = BlasterCharacter;
}

void UBlasterHealthComponent::SetHealth(float InHealth)
{
    if (!Character || !Character->HasAuthority()) return;

    Health = InHealth;
    HealthChangedDelegate.Broadcast(Health, MaxHealth);
}

void UBlasterHealthComponent::OnRep_Health(float PrevHealth) 
{
    HealthChangedDelegate.Broadcast(Health, MaxHealth);
}

void UBlasterHealthComponent::OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) 
{
    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);
    HealthChangedDelegate.Broadcast(Health, MaxHealth);
}

