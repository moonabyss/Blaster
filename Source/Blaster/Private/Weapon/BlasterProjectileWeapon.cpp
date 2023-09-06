// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "Weapon/BlasterProjectile.h"

void ABlasterProjectileWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget)
{
    Super::Fire(BarelLocation, HitTarget);

    if (!HasAuthority()) return;

    // Multicast_SpawnProjectile(BarelLocation, HitTarget);

    if (!GetWorld() || !GetOwner() || !GetMesh() || !ProjectileClass) return;

    FVector Direction = (HitTarget - BarelLocation).GetSafeNormal();
    const FTransform SpawnTransform(Direction.Rotation(), BarelLocation);
    ABlasterProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABlasterProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(this);
        Projectile->SetInstigator(Cast<APawn>(GetOwner()));
        Projectile->FinishSpawning(SpawnTransform);
    }
}

void ABlasterProjectileWeapon::Multicast_SpawnProjectile_Implementation(const FVector& StartLocation, const FVector& HitTarget)
{
    if (!GetWorld() || !GetOwner() || !GetMesh() || !ProjectileClass) return;

    const FVector BarelLocation = GetMesh()->GetSocketLocation(MuzzleFlashSocketName);

    FVector Direction = (HitTarget - BarelLocation).GetSafeNormal();
    const FTransform SpawnTransform(Direction.Rotation(), BarelLocation);
    ABlasterProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABlasterProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(this);
        Projectile->SetInstigator(Cast<APawn>(GetOwner()));
        UE_LOG(LogTemp, Warning, TEXT("Instigator: %s"), *Cast<APawn>(GetOwner())->GetName());
        Projectile->FinishSpawning(SpawnTransform);
    }
}
