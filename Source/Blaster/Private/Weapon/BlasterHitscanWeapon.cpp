// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterHitscanWeapon.h"
#include "Interfaces/Hitable.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

void ABlasterHitscanWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget)
{
    if (!GetWorld() || !GetOwner() || !GetOwner()->GetInstigator()) return;

    const FVector End = (HitTarget - BarelLocation).GetSafeNormal() * GetWeaponProps().Range;

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
    GetWorld()->LineTraceSingleByChannel(HitResult, BarelLocation, End, ECC_Visibility, QueryParams);
    if (HitResult.bBlockingHit)
    {
        if (IsValid(HitResult.GetActor()))
        {
            UGameplayStatics::ApplyDamage(HitResult.GetActor(), BulletProps.Damage, GetOwner()->GetInstigator()->GetController(), this, UDamageType::StaticClass());

            if (HitResult.GetActor()->Implements<UHitable>())
            {
                auto Victim = Cast<IHitable>(HitResult.GetActor());
                Victim->HitByProjectile();
            }
        }

        FTransform ImpactTransform(HitResult.ImpactNormal.Rotation(), HitResult.ImpactPoint);
        Multicast_PlayImpactFX_Implementation(BulletProps.BulletImpactParticles, BulletProps.BulletImpactSound, ImpactTransform);
    }

    Super::Fire(BarelLocation, HitTarget);
}

void ABlasterHitscanWeapon::Multicast_PlayImpactFX_Implementation(UParticleSystem* ImpactParticles, USoundCue* ImpactSound, FTransform ImpactTransform)
{
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ImpactTransform);
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactTransform.GetLocation());
}
