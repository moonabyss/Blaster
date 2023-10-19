// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileRocket.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

#include "Components/BlasterProjectileMoveComponent.h"

ABlasterProjectileRocket::ABlasterProjectileRocket()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Rocket Mesh");
    check(ProjectileMesh);
    ProjectileMesh->SetupAttachment(GetRootComponent());
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectileMesh->SetIsReplicated(true);

    ProjectileMovementComponent = CreateDefaultSubobject<UBlasterProjectileMoveComponent>("BlasterProjectileMovementComponent");
    check(ProjectileMovementComponent);
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->InitialSpeed = 2000.0f;
    ProjectileMovementComponent->MaxSpeed = 2000.0f;
    ProjectileMovementComponent->SetIsReplicated(true);
}

void ABlasterProjectileRocket::BeginPlay()
{
    Super::BeginPlay();

    SpawnFX(RocketProps.TrailFX, RocketProps.LoopSound);
}

void ABlasterProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (GetInstigator() && OtherActor == GetInstigator())
    {
        return;
    }

    if (GetInstigator() && GetInstigator()->GetController())
    {
        UGameplayStatics::ApplyRadialDamageWithFalloff(this, RocketProps.Damage, RocketProps.MinimumDamage, GetActorLocation(), RocketProps.DamageInnerRadius, RocketProps.DamageOuterRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigator()->GetController());
    }

    GetProjectileMovementComponent()->StopMovementImmediately();
    if (ProjectileMesh)
    {
        ProjectileMesh->SetVisibility(false);
    }
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    StartDestroyTimer();

    Multicast_PlayImpactFX(RocketProps.RocketImpactParticles, RocketProps.RocketImpactSound);
    Multicast_StopTrailFX();
}
