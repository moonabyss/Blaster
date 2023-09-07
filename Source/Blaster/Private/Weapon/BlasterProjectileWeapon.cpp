// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileWeapon.h"
#include "Engine/SkeletalMeshSocket.h"

#include "BlasterTypes/BlasterCoreTypes.h"
#include "Components/BlasterProjectileMoveComponent.h"
#include "Weapon/BlasterProjectile.h"

void ABlasterProjectileWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget)
{
    Super::Fire(BarelLocation, HitTarget);

    if (!HasAuthority()) return;
    if (!GetWorld() || !GetOwner() || !GetMesh() || !ProjectileClass) return;

    FVector Direction = (HitTarget - BarelLocation).GetSafeNormal();
    const FTransform SpawnTransform(Direction.Rotation(), BarelLocation);
    ABlasterProjectile* Projectile = GetWorld()->SpawnActorDeferred<ABlasterProjectile>(ProjectileClass, SpawnTransform);
    if (Projectile)
    {
        Projectile->SetShotDirection(Direction);
        Projectile->SetOwner(this);
        Projectile->SetInstigator(Cast<APawn>(GetOwner()));
        Projectile->SetLifeSpan(GetWeaponProps().Range / Projectile->GetInitialSpeed());
        Projectile->FinishSpawning(SpawnTransform);
    }
}
