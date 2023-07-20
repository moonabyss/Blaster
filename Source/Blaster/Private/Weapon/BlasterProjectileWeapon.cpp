// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

#include "Weapon/BlasterProjectile.h"
#include "BlasterCoreTypes.h"

void ABlasterProjectileWeapon::Fire(const FVector& HitTarget) 
{
    Super::Fire(HitTarget);

    if (!HasAuthority()) return;
    if (ProjectileClass)
    {
        const auto MuzzleFlashSocket = GetMesh()->GetSocketByName(MuzzleFlashSocketName);
        const auto SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetMesh());
        SpawnProjectile(SocketTransform.GetLocation(), HitTarget);
    }
}

void ABlasterProjectileWeapon::SpawnProjectile(const FVector& SocketLocation, const FVector& HitTarget)
{
    if (!GetWorld()) return;

    FVector ToTarget = HitTarget - SocketLocation;
    FRotator TargetRotation = ToTarget.Rotation();
    FActorSpawnParameters SpawnParams;
    SpawnParams.Owner = GetOwner();
    SpawnParams.Instigator = Cast<APawn>(GetOwner());
    GetWorld()->SpawnActor<ABlasterProjectile>(ProjectileClass, SocketLocation, TargetRotation, SpawnParams);
}
