// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileGrenade.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABlasterProjectileGrenade::ABlasterProjectileGrenade()
{
    ProjectileMesh = CreateDefaultSubobject<UStaticMeshComponent>("Grenade Mesh");
    check(ProjectileMesh);
    ProjectileMesh->SetupAttachment(GetRootComponent());
    ProjectileMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    ProjectileMesh->SetIsReplicated(true);

    ProjectileMoveComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    check(ProjectileMoveComponent);
    ProjectileMoveComponent->bRotationFollowsVelocity = true;
    ProjectileMoveComponent->ProjectileGravityScale = 1.0f;
    ProjectileMoveComponent->InitialSpeed = 2000.0f;
    ProjectileMoveComponent->MaxSpeed = 2000.0f;
    ProjectileMoveComponent->SetIsReplicated(true);
    ProjectileMoveComponent->bShouldBounce = true;
    ProjectileMoveComponent->OnProjectileBounce.AddDynamic(this, &ThisClass::OnBounce);
}

float ABlasterProjectileGrenade::GetInitialSpeed() const
{
    return ProjectileMoveComponent->InitialSpeed;
}

void ABlasterProjectileGrenade::BeginPlay()
{
    AActor::BeginPlay();

    StartDestroyTimer();

    if (HasAuthority())
    {
        ProjectileMoveComponent->Velocity = ShotDirection * ProjectileMoveComponent->InitialSpeed;
        CollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);
    }

    SpawnFX(GrenadeProps.TrailFX, nullptr);
}

void ABlasterProjectileGrenade::OnBounce(const FHitResult& ImpactResult, const FVector& ImpactVelocity)
{
    UGameplayStatics::PlaySoundAtLocation(this, GrenadeProps.BounceSound, GetActorLocation());
}

void ABlasterProjectileGrenade::DestroyTimerFinished()
{
    Multicast_PlayImpactFX(GrenadeProps.GrenadeExplosionParticles, GrenadeProps.GrenadeExplosionSound);
    Multicast_StopTrailFX();

    if (GetInstigator() && GetInstigator()->GetController())
    {
        UGameplayStatics::ApplyRadialDamageWithFalloff(this, GrenadeProps.Damage, GrenadeProps.MinimumDamage, GetActorLocation(), GrenadeProps.DamageInnerRadius, GrenadeProps.DamageOuterRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigator()->GetController());
    }

    Destroy();
}
