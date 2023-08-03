// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "Components/ActorComponent.h"
#include "CoreMinimal.h"

#include "BlasterCoreTypes.h"

#include "BlasterHealthComponent.generated.h"

class ABlasterCharacter;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class BLASTER_API UBlasterHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UBlasterHealthComponent();
    void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

public:
    FOnHealthChangedDelegate HealthChangedDelegate;

    bool IsAlive() const { return Health > 0.0f; }

    float GetHealth() const { return Health; }

    void SetCharacter(ABlasterCharacter* BlasterCharacter);

private:
    UPROPERTY(Category = "Player Stats", EditAnywhere)
    float MaxHealth = 100.0f;

    UPROPERTY(Category = "Player Stats", VisibleAnywhere, ReplicatedUsing = OnRep_Health)
    float Health{0.0f};

    ABlasterCharacter* Character{nullptr};

    UFUNCTION()
    void OnRep_Health(float PrevHealth);

    UFUNCTION()
    void OnTakeAnyDamageHandle(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
