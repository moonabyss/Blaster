// Blaster Multiplayer Game. All rights reserved.

#include "Components/BlasterWeaponComponent.h"

UBlasterWeaponComponent::UBlasterWeaponComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void UBlasterWeaponComponent::BeginPlay()
{
    Super::BeginPlay();
}

void UBlasterWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
