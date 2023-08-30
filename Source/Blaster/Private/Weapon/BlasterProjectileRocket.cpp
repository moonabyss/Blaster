// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterProjectileRocket.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

ABlasterProjectileRocket::ABlasterProjectileRocket()
{
    RocketMesh = CreateDefaultSubobject<UStaticMeshComponent>("Rocket Mesh");
    RocketMesh->SetupAttachment(GetRootComponent());
    RocketMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlasterProjectileRocket::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (GetInstigator() && GetInstigator()->GetController())
    {
        UGameplayStatics::ApplyRadialDamageWithFalloff(this, RocketProps.Damage, RocketProps.MinimumDamage, GetActorLocation(), RocketProps.DamageInnerRadius, RocketProps.DamageOuterRadius, 1.0f, UDamageType::StaticClass(), TArray<AActor*>(), this, GetInstigator()->GetController());
    }

    UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), RocketProps.RocketImpactParticles, GetActorTransform());
    UGameplayStatics::PlaySoundAtLocation(this, RocketProps.RocketImpactSound, GetActorLocation());

    Super::OnHit(HitComponent, OtherActor, OtherComp, NormalImpulse, Hit);
}
