// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "Interfaces/Hitable.h"

ABlasterProjectile::ABlasterProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
    SetRootComponent(CollisionBox);
    CollisionBox->SetBoxExtent(FVector(4.0f, 32.0f, 4.0f));
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
        ProjectileMovementComponent->Velocity = ShotDirection * ProjectileMovementComponent->InitialSpeed;
        CollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
        CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
    }
}

void ABlasterProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    ProjectileMovementComponent->StopMovementImmediately();
    Destroy();
}
