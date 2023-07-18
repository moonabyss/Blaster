// Blaster Multiplayer Game. All rights reserved.

#pragma once
#include "BlasterCoreTypes.generated.h"

// weapon
DECLARE_MULTICAST_DELEGATE(FOnWeaponEquippedDelegate);
DECLARE_MULTICAST_DELEGATE(FOnWeaponUnequippedDelegate);

const FName LeftHandSocketName = "LeftHandSocket";
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
    EWT_Pistol UMETA(DisplayName = "Pistol"),
    EWT_Rifle UMETA(DisplayName = "Rifle"),

    EWT_MAX UMETA(Hidden)
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

    FWeaponProps()
    {
        WeaponType = EWeaponType::EWT_MAX;
        AimSpeedModifier = 1.0f;
        BlasterFireMontage = nullptr;
        FireAnimation = nullptr;
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
