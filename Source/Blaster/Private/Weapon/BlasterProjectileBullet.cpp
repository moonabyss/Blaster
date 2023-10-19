// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileBullet.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"

#include "Components/BlasterProjectileMoveComponent.h"
#include "Interfaces/Hitable.h"

ABlasterProjectileBullet::ABlasterProjectileBullet()
{
    ProjectileMovementComponent = CreateDefaultSubobject<UBlasterProjectileMoveComponent>("BlasterProjectileMovementComponent");
    check(ProjectileMovementComponent);
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->InitialSpeed = 2000.0f;
    ProjectileMovementComponent->MaxSpeed = 2000.0f;
    ProjectileMovementComponent->SetIsReplicated(true);
}

void ABlasterProjectileBullet::BeginPlay()
{
    Super::BeginPlay();

    UGameplayStatics::SpawnEmitterAttached(BulletProps.BulletTracer, GetCollisionBox(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
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

    Multicast_PlayImpactFX(BulletProps.BulletImpactParticles, BulletProps.BulletImpactSound);

    Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
