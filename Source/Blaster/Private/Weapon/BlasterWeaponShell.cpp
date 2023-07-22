// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterWeaponShell.h"

ABlasterWeaponShell::ABlasterWeaponShell()
{
    PrimaryActorTick.bCanEverTick = false;

    ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>("ShellMesh");
    SetRootComponent(ShellMesh);
}

void ABlasterWeaponShell::BeginPlay()
{
    Super::BeginPlay();
}
