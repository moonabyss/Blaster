// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterHitscanWeapon.h"
#include "Interfaces/Hitable.h"
#include "Kismet/GameplayStatics.h"

void ABlasterHitscanWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget, float SpreadAngle)
{
    if (!GetWorld() || !GetOwner() || !GetOwner()->GetInstigator()) return;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);

    const FVector Direction = ShotDirectionWithSpread((HitTarget - BarelLocation).GetSafeNormal(), SpreadAngle);
    const FVector End = BarelLocation + Direction * GetWeaponProps().Range;
    FVector BeamEnd{End};

    GetWorld()->LineTraceSingleByChannel(HitResult, BarelLocation, End, ECC_Visibility, QueryParams);
    if (HitResult.bBlockingHit)
    {
        if (IsValid(HitResult.GetActor()))
        {
            if (HitResult.GetActor()->Implements<UHitable>())
            {
                UGameplayStatics::ApplyDamage(HitResult.GetActor(), BulletProps.Damage, GetOwner()->GetInstigator()->GetController(), this, UDamageType::StaticClass());
                auto Victim = Cast<IHitable>(HitResult.GetActor());
                Victim->HitByProjectile();
            }
        }

        FTransform ImpactTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint);
        Multicast_SpawnImpactFX(BulletProps.BulletImpactParticles, BulletProps.BulletImpactSound, ImpactTransform);
        BeamEnd = HitResult.ImpactPoint;
    }
    Multicast_PlayWeaponFireAnimation();
    Multicast_SpawnBeamFX(BulletProps.BulletTracer, BarelLocation, BeamEnd);
}
