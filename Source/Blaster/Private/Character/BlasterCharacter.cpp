// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h"

#include "Components/BlasterWeaponComponent.h"
#include "HUD/OverheadWidget.h"
#include "Weapon/BlasterBaseWeapon.h"

ABlasterCharacter::ABlasterCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    check(CameraBoom);
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
    check(FollowCamera);
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
    check(OverheadWidget);
    OverheadWidget->SetupAttachment(GetRootComponent());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    OverheadWidget->SetDrawAtDesiredSize(true);

    WeaponComponent = CreateDefaultSubobject<UBlasterWeaponComponent>("WeaponComponent");
    check(WeaponComponent);
    WeaponComponent->SetCharacter(this);
    WeaponComponent->SetIsReplicated(true);
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::BeginPlay()
{
    Super::BeginPlay();
}

void ABlasterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);
    PlayerInputComponent->BindAction("Equip", EInputEvent::IE_Pressed, this, &ThisClass::EquipPressed);

    PlayerInputComponent->BindAxis("MoveForward", this, &ThisClass::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &ThisClass::MoveRight);
    PlayerInputComponent->BindAxis("Turn", this, &ThisClass::Turn);
    PlayerInputComponent->BindAxis("LookUp", this, &ThisClass::LookUp);
}

void ABlasterCharacter::MoveForward(float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value)) return;

    const FRotator YawRotation = FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
    AddMovementInput(Direction, Value);
}

void ABlasterCharacter::MoveRight(float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value)) return;

    const FRotator YawRotation = FRotator(0.0f, Controller->GetControlRotation().Yaw, 0.0f);
    const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
    AddMovementInput(Direction, Value);
}

void ABlasterCharacter::Turn(float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value)) return;

    AddControllerYawInput(Value);
}

void ABlasterCharacter::LookUp(float Value)
{
    if (!Controller || FMath::IsNearlyZero(Value)) return;

    AddControllerPitchInput(Value);
}

void ABlasterCharacter::EquipPressed()
{
    if (IsValid(WeaponComponent) && HasAuthority())
    {
        WeaponComponent->EquipWeapon(OverlappingWeapon);
    }
}

void ABlasterCharacter::DisplayNetRole()
{
    if (!OverheadWidget) return;

    if (auto Widget = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject()))
    {
        Widget->SetDisplayText(FString(UEnum::GetValueAsString<ENetRole>(GetRemoteRole())));
    }
}

void ABlasterCharacter::SetOverlappedWeapon(ABlasterBaseWeapon* Weapon)
{
    if (IsValid(Weapon))
    {
        OverlappingWeapon = Weapon;
        if (IsLocallyControlled())
        {
            Weapon->ShowPickupWidget(true);
        }
    }
}

void ABlasterCharacter::UnsetOverlappedWeapon(ABlasterBaseWeapon* Weapon)
{
    if (IsValid(Weapon))
    {
        OverlappingWeapon = nullptr;
        if (IsLocallyControlled())
        {
            Weapon->ShowPickupWidget(false);
        }
    }
}

void ABlasterCharacter::OnRep_OverlappingWeapon(ABlasterBaseWeapon* LastValue)
{
    if (IsValid(OverlappingWeapon))
    {
        OverlappingWeapon->ShowPickupWidget(true);
    }
    if (IsValid(LastValue))
    {
        LastValue->ShowPickupWidget(false);
    }
}
