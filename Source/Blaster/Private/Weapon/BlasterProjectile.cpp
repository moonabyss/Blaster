// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectile.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystemInstanceController.h"
#include "Particles/ParticleSystem.h"
#include "Sound/SoundCue.h"

#include "Components/BlasterProjectileMoveComponent.h"
#include "Interfaces/Hitable.h"

ABlasterProjectile::ABlasterProjectile()
{
    PrimaryActorTick.bCanEverTick = true;
    bReplicates = true;

    CollisionBox = CreateDefaultSubobject<UBoxComponent>("CollisionBox");
    check(CollisionBox);
    SetRootComponent(CollisionBox);
    CollisionBox->SetBoxExtent(FVector(20.0f, 1.0f, 1.0f));
    CollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    CollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    CollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
    CollisionBox->SetCollisionResponseToChannel(ECC_SkeletalMesh, ECollisionResponse::ECR_Block);
}

void ABlasterProjectile::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        ProjectileMovementComponent->Velocity = ShotDirection * ProjectileMovementComponent->InitialSpeed;
        CollisionBox->IgnoreActorWhenMoving(GetInstigator(), true);
        CollisionBox->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);
    }
}

float ABlasterProjectile::GetInitialSpeed() const
{
    return ProjectileMovementComponent->InitialSpeed;
}

void ABlasterProjectile::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (OtherActor == GetInstigator())
    {
        return;
    }

    ProjectileMovementComponent->StopMovementImmediately();
    Destroy();
}

void ABlasterProjectile::Multicast_PlayImpactFX_Implementation(UParticleSystem* ImpactParticles, USoundCue* ImpactSound)
{
    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, GetActorTransform());
    UGameplayStatics::PlaySoundAtLocation(this, ImpactSound, GetActorLocation());
}

void ABlasterProjectile::Multicast_StopTrailFX_Implementation()
{
    if (TrailFXComponent && TrailFXComponent->GetSystemInstanceController())
    {
        TrailFXComponent->GetSystemInstanceController()->Deactivate();
    }
    if (LoopSoundComponent)
    {
        LoopSoundComponent->Stop();
    }
}

void ABlasterProjectile::StartDestroyTimer()
{
    GetWorldTimerManager().SetTimer(DestroyTimer, this, &ThisClass::DestroyTimerFinished, DestroyDelay);
}

void ABlasterProjectile::DestroyTimerFinished()
{
    Destroy();
}

void ABlasterProjectile::SpawnFX(UNiagaraSystem* VisualFX, USoundBase* LoopSound)
{
    TrailFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(VisualFX, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
    LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(LoopSound, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, true);
}
