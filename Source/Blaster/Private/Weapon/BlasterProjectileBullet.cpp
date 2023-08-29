// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileBullet.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "Interfaces/Hitable.h"

void ABlasterProjectileBullet::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        TracerComponent = UGameplayStatics::SpawnEmitterAttached(BulletProps.BulletTracer, GetCollisionBox(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
    }
}

void ABlasterProjectileBullet::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    UGameplayStatics::ApplyDamage(OtherActor, BulletProps.Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

    if (IsValid(OtherActor) && OtherActor->Implements<UHitable>())
    {
        auto Victim = Cast<IHitable>(OtherActor);
        Victim->HitByProjectile();
    }

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletProps.BulletImpactParticles, GetActorTransform());
    UGameplayStatics::PlaySoundAtLocation(this, BulletProps.BulletImpactSound, GetActorLocation());

    Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
