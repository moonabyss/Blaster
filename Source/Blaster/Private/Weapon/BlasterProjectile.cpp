// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectile.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

ABlasterProjectile::ABlasterProjectile()
{
    PrimaryActorTick.bCanEverTick = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
    SetRootComponent(CollisionBox);
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);

    ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovementComponent");
    ProjectileMovementComponent->bRotationFollowsVelocity = true;
    ProjectileMovementComponent->InitialSpeed = 15000.0f;
    ProjectileMovementComponent->MaxSpeed = 15000.0f;
}

void ABlasterProjectile::BeginPlay()
{
    Super::BeginPlay();
}

void ABlasterProjectile::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}
