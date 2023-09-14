// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterMultiscanWeapon.h"
#include "Interfaces/Hitable.h"
#include "Kismet/GameplayStatics.h"

void ABlasterMultiscanWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget, float SpreadAngle)
{
    if (!GetWorld() || !GetOwner() || !GetOwner()->GetInstigator()) return;

    TMap<AActor*, int32> HittedActors;
    for (int32 i = 0; i < Pellets; ++i)
    {
        FHitResult HitResult;
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor(this);
        const FVector Direction = ShotDirectionWithSpread((HitTarget - BarelLocation).GetSafeNormal(), SpreadAngle);
        const FVector End = BarelLocation + Direction * GetWeaponProps().Range;
        FVector BeamEnd{End};
        GetWorld()->LineTraceSingleByChannel(HitResult, BarelLocation, End, ECC_Visibility, QueryParams);

        if (HitResult.bBlockingHit)
        {
            if (IsValid(HitResult.GetActor()) && HitResult.GetActor()->Implements<UHitable>())
            {
                AActor* HitActor = HitResult.GetActor();
                int32* HitCount = HittedActors.Find(HitActor);
                HitCount ? (*HitCount)++ : HittedActors.Add(HitActor, 1);
            }
            BeamEnd = HitResult.ImpactPoint;
            FTransform ImpactTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint);
            Multicast_SpawnImpactFX(BulletProps.BulletImpactParticles, /*BulletProps.BulletImpactSound*/ nullptr, ImpactTransform);
        }

        Multicast_SpawnBeamFX(BulletProps.BulletTracer, BarelLocation, BeamEnd);
    }

    for (auto& [HitActor, HitCount] : HittedActors)
    {
        const float Damage = BulletProps.Damage / Pellets * HitCount;
        UGameplayStatics::ApplyDamage(HitActor, Damage, GetOwner()->GetInstigator()->GetController(), this, UDamageType::StaticClass());
        auto Victim = Cast<IHitable>(HitActor);
        Victim->HitByProjectile();
    }
    Multicast_PlayWeaponFireAnimation();
}
