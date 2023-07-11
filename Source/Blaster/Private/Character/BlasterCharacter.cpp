// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "HUD/OverheadWidget.h"

ABlasterCharacter::ABlasterCharacter()
{
    PrimaryActorTick.bCanEverTick = true;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
    check(OverheadWidget);
    OverheadWidget->SetupAttachment(GetRootComponent());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    OverheadWidget->SetDrawAtDesiredSize(true);
}

void ABlasterCharacter::BeginPlay()
{
    Super::BeginPlay();

    if (auto Widget = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject()))
    {
        // Display network role
        // Widget->SetDisplayText(FString(UEnum::GetValueAsString<ENetRole>(GetRemoteRole())));
    }
}

void ABlasterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ACharacter::Jump);

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

void ABlasterCharacter::DisplayNetRole()
{
    if (!OverheadWidget) return;

    // OverheadWidget->SetDisplayText();
}
