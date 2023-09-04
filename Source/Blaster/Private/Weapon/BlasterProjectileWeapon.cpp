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
        DrawDebugSphere(GetWorld(), StartLocation, 24, 16, FColor::Orange, false, 30.0f);
    }
}

void ABlasterProjectileWeapon::MulticastSpawnProjectile_Implementation(const FVector& StartLocation, const FVector& HitTarget)
{
    if (!GetWorld() || !GetMesh()) return;

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
