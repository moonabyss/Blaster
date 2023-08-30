// Blaster Multiplayer Game. All rights reserved.

#pragma once
#include "BlasterCoreTypes.generated.h"

#define ECC_SkeletalMesh ECollisionChannel::ECC_GameTraceChannel1

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
