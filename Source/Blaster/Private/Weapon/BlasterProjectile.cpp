// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "Interfaces/Hitable.h"

ABlasterProjectile::ABlasterProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
    SetRootComponent(CollisionBox);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->ProjectileGravityScale = 0.0f;
    ProjectileMovementComponent->InitialSpeed = 2000.0f;
    ProjectileMovementComponent->MaxSpeed = 2000.0f;
}

void ABlasterProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        if (IsValid(BulletProps.BulletTracer))
        {
            TracerComponent = UGameplayStatics::SpawnEmitterAttached(BulletProps.BulletTracer, CollisionBox, FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
        }

        ProjectileMovementComponent->Velocity = ShotDirection * ProjectileMovementComponent->InitialSpeed;
        CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
        CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
    }
}

void ABlasterProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABlasterProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ProjectileMovementComponent->StopMovementImmediately();
    SpawnParticles();
    SpawnSound();
    Destroy();
}

void ABlasterProjectile::SpawnParticles()
{
    if (!BulletProps.BulletImpactParticles) return;

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), BulletProps.BulletImpactParticles, GetActorTransform());
}

void ABlasterProjectile::SpawnSound()
{
    if (!BulletProps.BulletImpactSound) return;

    UGameplayStatics::PlaySoundAtLocation(this, BulletProps.BulletImpactSound, GetActorLocation());
}
