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

    void SetCharacter(ABlasterCharacter* BlasterCharacter);
    void SetHealth(float InHealth);

private:
    UPROPERTY(Category = "Player Stats", EditAnywhere)
    float MaxHealth = 100.0f;

    UPROPERTY(Category = "Player Stats", VisibleAnywhere, ReplicatedUsing = OnRep_Health)
    float Health{0.0f};

    ABlasterCharacter* Character{nullptr};

    UFUNCTION()
    void OnRep_Health(float PrevHealth);
};
