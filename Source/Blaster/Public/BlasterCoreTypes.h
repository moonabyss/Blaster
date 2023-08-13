// Blaster Multiplayer Game. All rights reserved.

#pragma once
#include "BlasterCoreTypes.generated.h"

#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1

// weapon
DECLARE_MULTICAST_DELEGATE(FOnWeaponEquippedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnWeaponUnequippedDelegate);

const FName LeftHandSocketName = "LeftHandSocket";
const FName MuzzleFlashSocketName = "MuzzleFlash";
const FName ShellSocketName = "AmmoEject";
const FName WeaponHipMontageSectionName = "Hip";
const FName WeaponAimMontageSectionName = "Aim";

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
    EWS_Initial UMETA(DisplayName = "Initial"),
    EWS_Equipped UMETA(DisplayName = "Equipped"),
    EWS_Dropped UMETA(DisplayName = "Dropped"),

    EWS_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
    None,
    EWT_Pistol UMETA(DisplayName = "Pistol"),
    EWT_Rifle UMETA(DisplayName = "Rifle"),

    MAX UMETA(Hidden)
};

USTRUCT(BlueprintType)
struct FCrosshairs
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    UTexture2D* Center;

    UPROPERTY(EditAnywhere)
    UTexture2D* Left;

    UPROPERTY(EditAnywhere)
    UTexture2D* Right;

    UPROPERTY(EditAnywhere)
    UTexture2D* Top;

    UPROPERTY(EditAnywhere)
    UTexture2D* Bottom;

    UPROPERTY(EditAnywhere)
    float SpreadAngle;

    UPROPERTY(EditAnywhere)
    FColor Color;

    FCrosshairs()
    {
        Center = nullptr;
        Left = nullptr;
        Right = nullptr;
        Top = nullptr;
        Bottom = nullptr;
        SpreadAngle = 0.0f;
        Color = FColor::White;
    }
};

USTRUCT(BlueprintType)
struct FWeaponProps
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    EWeaponType WeaponType;

    UPROPERTY(EditAnywhere)
    float AimSpeedModifier;

    UPROPERTY(EditAnywhere)
    UAnimMontage* BlasterFireMontage;

    UPROPERTY(EditAnywhere)
    UAnimationAsset* FireAnimation;

    UPROPERTY(EditAnywhere)
    FCrosshairs Crosshairs;

    UPROPERTY(EditAnywhere)
    float DefaultSpreadInDegrees;

    UPROPERTY(EditAnywhere)
    float SpreadModifierWalk;

    UPROPERTY(EditAnywhere)
    float ZoomedFOV;

    UPROPERTY(EditAnywhere)
    float ZoomInterpSpeed;

    UPROPERTY(EditAnywhere)
    float SpreadModifierZoom;

    UPROPERTY(EditAnywhere)
    float SpreadModifierPerShoot;

    UPROPERTY(EditAnywhere)
    float SpreadModifierShootingMax;

    UPROPERTY(EditAnywhere)
    bool bIsAutomatic;

    UPROPERTY(EditAnywhere)
    float FireDelay;

    UPROPERTY(EditAnywhere)
    int32 ClipCapacity;

    UPROPERTY(EditAnywhere)
    UAnimMontage* BlasterReloadMontage;

    UPROPERTY(EditAnywhere)
    float ReloadTime;

    UPROPERTY(EditAnywhere)
    USoundBase* EquipSound;

    FWeaponProps()
    {
        WeaponType = EWeaponType::None;
        AimSpeedModifier = 1.0f;
        BlasterFireMontage = nullptr;
        FireAnimation = nullptr;
        Crosshairs = FCrosshairs();
        DefaultSpreadInDegrees = 0.0f;
        SpreadModifierWalk = 1.0f;
        ZoomedFOV = 30.0f;
        ZoomInterpSpeed = 20.0f;
        SpreadModifierZoom = 0.0f;
        SpreadModifierPerShoot = 0.0f;
        SpreadModifierShootingMax = 0.0f;
        bIsAutomatic = false;
        FireDelay = 0.0f;
        ClipCapacity = 0;
        BlasterReloadMontage = nullptr;
        ReloadTime = 0.0f;
        EquipSound = nullptr;
    }
};

USTRUCT(BlueprintType)
struct FBulletProps
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere)
    UParticleSystem* BulletTracer;

    UPROPERTY(EditAnywhere)
    UParticleSystem* BulletImpactParticles;

    UPROPERTY(EditAnywhere)
    class USoundCue* BulletImpactSound;

    FBulletProps()
    {
        BulletTracer = nullptr;
        BulletImpactParticles = nullptr;
        BulletImpactSound = nullptr;
    }
};

// character
UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
    ETIP_Left UMETA(DisplayName = "Left"),
    ETIP_Right UMETA(DisplayName = "Right"),
    ETIP_NotTurning UMETA(DisplayName = "Not Turning"),

    ETIP_MAX UMETA(Hidden)
};

UENUM(BlueprintType)
enum class ECombatState : uint8
{
    ECS_Unoccupied UMETA(DisplayName = "Unoccupied"),
    ECS_Reloading UMETA(DisplayName = "Reloading"),

    MAX UMETA(Hidden)
};

// Health
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedDelegate, float NewHealth, float MaxHealth);
