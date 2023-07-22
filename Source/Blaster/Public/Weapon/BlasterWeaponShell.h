// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "BlasterWeaponShell.generated.h"

UCLASS()
class BLASTER_API ABlasterWeaponShell : public AActor
{
    GENERATED_BODY()

public:
    ABlasterWeaponShell();

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    UStaticMeshComponent* ShellMesh;
};
