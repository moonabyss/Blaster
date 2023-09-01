// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "Weapon/BlasterProjectile.h"

void ABlasterProjectileWeapon::Fire(const FVector& HitTarget)
{
    Super::Fire(HitTarget);

    if (!HasAuthority()) return;
    if (ProjectileClass)
    {
        const auto StartLocation = GetMesh()->GetSocketLocation(MuzzleFlashSocketName);
        MulticastSpawnProjectile(StartLocation, HitTarget);
    }
}

void ABlasterProjectileWeapon::MulticastSpawnProjectile_Implementation(const FVector& StartLocation, const FVector& HitTarget)
{
    if (!GetWorld() || !GetMesh()) return;

    FVector Direction = (HitTarget - StartLocation).GetSafeNormal();
    const FVector SocketLocation = GetMesh()->GetSocketLocation(MuzzleFlashSocketName);
    const FTransform SpawnTransform(Direction.Rotation(), SocketLocation);
    ABlasterProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABlasterProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(GetOwner());
        Projectile->SetInstigator(Cast<APawn>(GetOwner()));
        Projectile->FinishSpawning(SpawnTransform);
    }
}
