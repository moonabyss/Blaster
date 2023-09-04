// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "Weapon/BlasterProjectile.h"

void ABlasterProjectileWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget)
{
    Super::Fire(BarelLocation, HitTarget);

    if (!HasAuthority()) return;

    Multicast_SpawnProjectile(BarelLocation, HitTarget);
}

void ABlasterProjectileWeapon::Multicast_SpawnProjectile_Implementation(const FVector& StartLocation, const FVector& HitTarget)
{
    if (!GetWorld() || !GetOwner() || !ProjectileClass) return;

    FVector Direction = (HitTarget - StartLocation).GetSafeNormal();
    const FTransform SpawnTransform(Direction.Rotation(), StartLocation);
    ABlasterProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABlasterProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(this);
        Projectile->SetInstigator(Cast<APawn>(GetOwner()));
        Projectile->FinishSpawning(SpawnTransform);
    }
}
