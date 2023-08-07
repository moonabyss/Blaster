// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterHealthComponent.h"
#include "Net/UnrealNetwork.h"

#include "Character/BlasterCharacter.h"
#include "Character/BlasterPlayerController.h"
#include "GameMode/BlasterGameMode.h"

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

    Health = MaxHealth;
    HealthChangedDelegate.Broadcast(Health, MaxHealth);

    if (AActor* ComponentOwner = GetOwner())
    {
        if (ComponentOwner->HasAuthority())
        {
            ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &ThisClass::OnTakeAnyDamageHandle);
        }
    }
}

void UBlasterHealthComponent::SetCharacter(ABlasterCharacter* BlasterCharacter)
{
    Character = BlasterCharacter;
}

void UBlasterHealthComponent::OnRep_Health(float PrevHealth)
{
    HealthChangedDelegate.Broadcast(Health, MaxHealth);
}

void UBlasterHealthComponent::OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    Health = FMath::Clamp(Health - Damage, 0.0f, MaxHealth);

    if (FMath::IsNearlyZero(Health))
    {
        Health = 0.0f;

        auto BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
        auto PlayerController = Cast<ABlasterPlayerController>(Character->Controller);
        auto AttackerController = Cast<ABlasterPlayerController>(InstigatedBy);
        if (BlasterGameMode && PlayerController && AttackerController)
        {
            BlasterGameMode->PlayerEliminated(Character, PlayerController, AttackerController);
        }

        // DIE
    }

    HealthChangedDelegate.Broadcast(Health, MaxHealth);
}
