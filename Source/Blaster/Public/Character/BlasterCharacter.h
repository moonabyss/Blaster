// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BlasterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABlasterCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

protected:
    virtual void BeginPlay() override;

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UCameraComponent* FollowCamera;
};
