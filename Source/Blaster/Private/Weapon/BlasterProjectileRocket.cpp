// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileRocket.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"

#include "Components/BlasterProjectileMoveComponent.h"

ABlasterProjectileRocket::ABlasterProjectileRocket()
{
    RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("Rocket Mesh");
    check(RocketMesh);
    RocketMesh->SetupAttachment(GetRootComponent());
    RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    RocketMesh->SetIsReplicated(true);
}

void ABlasterProjectileRocket::BeginPlay()
{
    Super::BeginPlay();

    TrailFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(RocketProps.TrailFX, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
    LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(RocketProps.LoopSound, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, true);
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
    RocketMesh->SetVisibility(false);
    GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetWorldTimerManager().SetTimer(DestroyTimer, this, &ThisClass::DestroyTimerFinished, DestroyDelay);

    Multicast_PlayImpactFX(RocketProps.RocketImpactParticles, RocketProps.RocketImpactSound);
    Multicast_StopTrailFX();
}

void ABlasterProjectileRocket::Multicast_StopTrailFX_Implementation()
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

void ABlasterProjectileRocket::DestroyTimerFinished()
{
    Destroy();
}
