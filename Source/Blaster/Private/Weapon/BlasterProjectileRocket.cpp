// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileRocket.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Sound/SoundCue.h"

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

    if (HasAuthority())
    {
        TrailFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(RocketProps.TrailFX, GetRootComponent(), FName(), GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition, false);
        LoopSoundComponent = UGameplayStatics::SpawnSoundAttached(RocketProps.LoopSound, GetRootComponent(), FName(), GetActorLocation(), EAttachLocation::KeepWorldPosition, true);
    }
}

void ABlasterProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (GetInstigator() && GetInstigator()->GetController())
    {
        UGameplayStatics::ApplyRadialDamageWithFalloff(this, RocketProps.Damage, RocketProps.MinimumDamage, GetActorLocation(), RocketProps.DamageInnerRadius, RocketProps.DamageOuterRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigator()->GetController());
    }

    PlayImpactFX(RocketProps.RocketImpactParticles, RocketProps.RocketImpactSound);
    GetProjectileMovementComponent()->StopMovementImmediately();
    RocketMesh->SetVisibility(false);
    GetCollisionBox()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    if (TrailFXComponent && TrailFXComponent->GetSystemInstanceController())
    {
        TrailFXComponent->GetSystemInstanceController()->Deactivate();
    }
    if (LoopSoundComponent)
    {
        LoopSoundComponent->Stop();
    }

    GetWorldTimerManager().SetTimer(DestroyTimer, this, &ThisClass::DestroyTimerFinished, DestroyDelay);
}

void ABlasterProjectileRocket::DestroyTimerFinished()
{
    Destroy();
}
