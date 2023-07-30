// Blaster Multiplayer Game. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"

#include "BlasterCoreTypes.h"
#include "Interfaces/Hitable.h"
#include "Interfaces/InteractWithCrosshairs.h"

#include "BlasterCharacter.generated.h"

class UAnimMontage;
class UCameraComponent;
class USphereComponent;
class USpringArmComponent;
class UWidgetComponent;
class ABlasterBaseWeapon;
class UBlasterHealthComponent;
class UBlasterMovementComponent;
class UBlasterWeaponComponent;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairs, public IHitable
{
    GENERATED_BODY()

public:
    ABlasterCharacter(const FObjectInitializer& ObjInit);
    virtual void Tick(float DeltaTime) override;
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
    virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
    virtual void OnRep_ReplicatedMovement() override;

protected:
    virtual void BeginPlay() override;
    virtual void Jump() override;

public:
    virtual void SetOverlappedWeapon(ABlasterBaseWeapon* Weapon);
    virtual void UnsetOverlappedWeapon(ABlasterBaseWeapon* Weapon);
    bool IsWeaponEquipped() const;
    ABlasterBaseWeapon* GetCurrentWeapon() const;
    bool IsAiming() const;
    float GetAimYaw() const;
    float GetAimPitch() const;
    ETurningInPlace GetTurningInPlace() const;
    FVector GetHitTargetNoSpread() const;
    virtual void HitByProjectile() override;
    bool ShouldRotateRootBone() { return bRotateRootBone; }

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
    void CalculateAO_Pitch();

    void FirePressed();
    void FireReleased();

private:
    UPROPERTY(Category = "Components", VisibleAnywhere)
    USpringArmComponent* CameraBoom;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UCameraComponent* FollowCamera;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    USphereComponent* CameraCollisionComponent;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UWidgetComponent* OverheadWidget;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBlasterWeaponComponent* WeaponComponent;

    UPROPERTY(Category = "Components", VisibleAnywhere)
    UBlasterHealthComponent* HealthComponent;

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

    float AO_Yaw{0.0f};

    float AO_Pitch{0.0f};

    float InterpAO_Yaw{0.0f};

    FRotator StartingAimRotation{FRotator()};

    ETurningInPlace TurningInPlace{ETurningInPlace::ETIP_NotTurning};
    void TurnInPlace(float DeltaTime);

    UFUNCTION()
    void OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    UFUNCTION()
    void OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

    void CheckCameraOverlap();

    UPROPERTY(Category = "Combat", EditDefaultsOnly)
    UAnimMontage* HitReactMontage;

    void PlayHitReactMontage();

    UFUNCTION(NetMulticast, Unreliable)
    void MulticastHit();

    void SimProxiesTurn();

    bool bRotateRootBone;

    float TimeSinceLastMovementReplication{0.0f};
};
