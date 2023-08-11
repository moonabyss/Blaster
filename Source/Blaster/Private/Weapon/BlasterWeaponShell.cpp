// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterWeaponShell.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"

ABlasterWeaponShell::ABlasterWeaponShell()
{
    PrimaryActorTick.bCanEverTick = false;

    ShellMesh = CreateDefaultSubobject<UStaticMeshComponent>("ShellMesh");
    SetRootComponent(ShellMesh);
    ShellMesh->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly);
    ShellMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    ShellMesh->SetSimulatePhysics(true);
    ShellMesh->SetEnableGravity(true);
    ShellMesh->SetNotifyRigidBodyCollision(true);
}

void ABlasterWeaponShell::BeginPlay()
{
    Super::BeginPlay();

    ShellMesh->OnComponentHit.AddDynamic(this, &ThisClass::OnHit);

    ShellMesh->AddImpulse(GetActorRightVector() * ShellEjectionImpulse);
    SetLifeSpan(LifeSpanTime);
}

void ABlasterWeaponShell::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (ShellSound)
    {
        ShellMesh->OnComponentHit.RemoveAll(this);
        UGameplayStatics::PlaySoundAtLocation(this, ShellSound, GetActorLocation());
    }
}
