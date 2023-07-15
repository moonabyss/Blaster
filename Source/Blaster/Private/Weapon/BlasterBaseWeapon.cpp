// Blaster Multiplayer Game. All rights reserved.

#include "Weapon/BlasterBaseWeapon.h"
#include "Character/BlasterCharacter.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Net/UnrealNetwork.h"

ABlasterBaseWeapon::ABlasterBaseWeapon()
{
    PrimaryActorTick.bCanEverTick = false;
    bReplicates = true;

    WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>("WeaponMesh");
    check(WeaponMesh);
    SetRootComponent(WeaponMesh);
    WeaponMesh->SetCollisionResponseToAllChannels(ECR_Block);
    WeaponMesh->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
    WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    AreaSphere = CreateDefaultSubobject<USphereComponent>("AreaSphere");
    check(AreaSphere);
    AreaSphere->SetupAttachment(RootComponent);
    AreaSphere->InitSphereRadius(200.0f);
    AreaSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
    AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    PickupWidget = CreateDefaultSubobject<UWidgetComponent>("PickupWidget");
    check(PickupWidget);
    PickupWidget->SetupAttachment(RootComponent);
    PickupWidget->SetWidgetSpace(EWidgetSpace::Screen);
    PickupWidget->SetDrawAtDesiredSize(true);
    PickupWidget->SetVisibility(false);
}

void ABlasterBaseWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ABlasterBaseWeapon, WeaponState, COND_OwnerOnly);
}

void ABlasterBaseWeapon::BeginPlay()
{
    Super::BeginPlay();

    if (HasAuthority())
    {
        AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
        AreaSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
        AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnSphereBeginOverlap);
        AreaSphere->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnSphereEndOverlap);
    }
}

void ABlasterBaseWeapon::OnSphereBeginOverlap(
    UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    if (auto Player = Cast<ABlasterCharacter>(OtherActor))
    {
        Player->SetOverlappedWeapon(this);
    }
}

void ABlasterBaseWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    if (auto Player = Cast<ABlasterCharacter>(OtherActor))
    {
        Player->UnsetOverlappedWeapon(this);
    }
}

void ABlasterBaseWeapon::ShowPickupWidget(bool bShowWidget)
{
    if (IsValid(PickupWidget))
    {
        PickupWidget->SetVisibility(bShowWidget);
    }
}

void ABlasterBaseWeapon::SetWeaponState(EWeaponState State)
{
    if (IsValid(PickupWidget))
    {
        PickupWidget->SetVisibility(false);
    }

    if (HasAuthority())
    {
        check(AreaSphere);

        switch (State)
        {
            case EWeaponState::EWS_Initial:
            {
                AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
                break;
            }
            case EWeaponState::EWS_Equipped:
            {
                AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
                break;
            }
            case EWeaponState::EWS_Dropped: break;
        }
        WeaponState = State;
    }
}

void ABlasterBaseWeapon::OnRep_WeaponState_Implementation()
{
    if (IsValid(PickupWidget))
    {
        PickupWidget->SetVisibility(false);
    }
}

FWeaponProps ABlasterBaseWeapon::GetWeaponProps() const
{
    return WeaponProperies;
}
