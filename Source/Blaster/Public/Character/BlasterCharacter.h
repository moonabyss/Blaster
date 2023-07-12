// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BlasterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;
class ABlasterBaseWeapon;
class UBlasterWeaponComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABlasterCharacter();
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void SetOverlappedWeapon(ABlasterBaseWeapon* Weapon);
    virtual void UnsetOverlappedWeapon(ABlasterBaseWeapon* Weapon);

protected:
    virtual void BeginPlay() override;

    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void EquipPressed();

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UCameraComponent* FollowCamera;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UWidgetComponent* OverheadWidget;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBlasterWeaponComponent* WeaponComponent;

    UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
    ABlasterBaseWeapon* OverlappingWeapon{nullptr};

    UFUNCTION()
    void OnRep_OverlappingWeapon(ABlasterBaseWeapon* LastValue);

    void DisplayNetRole();
};
