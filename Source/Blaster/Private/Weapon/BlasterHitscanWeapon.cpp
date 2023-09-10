// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterHitscanWeapon.h"
#include "Interfaces/Hitable.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void ABlasterHitscanWeapon::Fire(const FVector& BarelLocation, const FVector& HitTarget)
{
    if (!GetWorld() || !GetOwner() || !GetOwner()->GetInstigator()) return;

    const FVector End = BarelLocation + (HitTarget - BarelLocation).GetSafeNormal() * GetWeaponProps().Range;
    FVector BeamEnd{End};

    FHitResult HitResult;
    FCollisionQueryParams QueryParams;
    QueryParams.AddIgnoredActor(this);
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
    Multicast_SpawnBeamFX(BulletProps.BulletTracer, BarelLocation, BeamEnd);

    Super::Fire(BarelLocation, HitTarget);
}

void ABlasterHitscanWeapon::Multicast_SpawnImpactFX_Implementation(UParticleSystem* ImpactParticles, USoundCue* ImpactSound, const FTransform& ImpactTransform)
{
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, ImpactTransform);
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, ImpactTransform.GetLocation());
}

void ABlasterHitscanWeapon::Multicast_SpawnBeamFX_Implementation(UParticleSystem* ImpactParticles, const FVector& BeamStart, const FVector& BeamEnd)
{
    if (auto* Beam = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, BeamStart, FRotator()))
    {
        Beam->SetVectorParameter(FName("Target"), BeamEnd);
    }
}
