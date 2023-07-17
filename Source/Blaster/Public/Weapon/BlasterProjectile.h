// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterProjectile.generated.h"

class UBoxComponent;

UCLASS()
class BLASTER_API ABlasterProjectile : public AActor
{
    GENERATED_BODY()

public:
    ABlasterProjectile();
    virtual void Tick(float DeltaTime) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBoxComponent* CollisionBox;
};
