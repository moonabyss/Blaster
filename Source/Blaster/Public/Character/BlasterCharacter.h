// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BlasterCoreTypes.h"

#include "BlasterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UWidgetComponent;
class ABlasterBaseWeapon;
class UBlasterMovementComponent;
class UBlasterWeaponComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter
{
    GENERATED_BODY()

public:
    ABlasterCharacter(const FObjectInitializer& ObjInit);
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
    virtual void BeginPlay() override;

public:
    virtual void SetOverlappedWeapon(ABlasterBaseWeapon* Weapon);
    virtual void UnsetOverlappedWeapon(ABlasterBaseWeapon* Weapon);
    bool IsWeaponEquipped() const;
    ABlasterBaseWeapon* GetCurrentWeapon() const;
    bool IsAiming() const;
    float GetAimYaw() const;
    float GetAimPitch() const;

protected:
    void MoveForward(float Value);
    void MoveRight(float Value);
    void Turn(float Value);
    void LookUp(float Value);
    void EquipPressed();
    void CrouchPressed();
    void AimPressed();
    void AimReleased();
    void AimOffset(float DeltaTime);

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

    UFUNCTION(Server, Reliable)
    void ServerEquipPressed();

    UFUNCTION()
    void OnWeaponEquipped();

    UFUNCTION()
    void OnWeaponUnequipped();

    UFUNCTION()
    void OnAiming(bool bIsAiming);

    UPROPERTY(Replicated)
    float AO_Yaw{0.0f};
    UPROPERTY(Replicated)
    float AO_Pitch{0.0f};

    FRotator StartingAimRotation{FRotator()};
};
