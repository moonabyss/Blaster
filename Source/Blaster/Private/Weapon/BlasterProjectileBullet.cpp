// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileBullet.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

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
    if (OtherActor == GetInstigator())
    {
        return;
    }

    UGameplayStatics::ApplyDamage(OtherActor, BulletProps.Damage, GetInstigator()->GetController(), this, UDamageType::StaticClass());

    if (IsValid(OtherActor) && OtherActor->Implements<UHitable>())
    {
        auto Victim = Cast<IHitable>(OtherActor);
        Victim->HitByProjectile();
    }

    PlayImpactFX(BulletProps.BulletImpactParticles, BulletProps.BulletImpactSound);

    Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
