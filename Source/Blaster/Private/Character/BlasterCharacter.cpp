// Blaster Multiplayer Game. All rights reserved.

#include "Character/BlasterCharacter.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"

#include "Character/BlasterPlayerController.h"
#include "Components/BlasterHealthComponent.h"
#include "Components/BlasterMovementComponent.h"
#include "Components/BlasterWeaponComponent.h"
#include "GameMode/BlasterGameMode.h"
#include "HUD/OverheadWidget.h"
#include "Weapon/BlasterBaseWeapon.h"

ABlasterCharacter::ABlasterCharacter(const FObjectInitializer& ObjInit)
    : Super(ObjInit.SetDefaultSubobjectClass<UBlasterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
    PrimaryActorTick.bCanEverTick = true;
    SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    CameraBoom = CreateDefaultSubobject<USpringArmComponent>("CameraBoom");
    check(CameraBoom);
    CameraBoom->SetupAttachment(GetMesh());
    CameraBoom->TargetArmLength = 600.0f;
    CameraBoom->bUsePawnControlRotation = true;

    FollowCamera = CreateDefaultSubobject<UCameraComponent>("Camera");
    check(FollowCamera);
    FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
    FollowCamera->bUsePawnControlRotation = false;

    CameraCollisionComponent = CreateDefaultSubobject<USphereComponent>("CameraCollisionComponent");
    CameraCollisionComponent->SetupAttachment(FollowCamera);
    CameraCollisionComponent->SetSphereRadius(200.0f);
    CameraCollisionComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldStatic);
    CameraCollisionComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Overlap);

    GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetCollisionObjectType(ECC_SkeletalMesh);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
    GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);

    OverheadWidget = CreateDefaultSubobject<UWidgetComponent>("OverheadWidget");
    OverheadWidget->SetupAttachment(GetRootComponent());
    OverheadWidget->SetWidgetSpace(EWidgetSpace::Screen);
    OverheadWidget->SetDrawAtDesiredSize(true);

    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
    GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
    GetCharacterMovement()->SetCrouchedHalfHeight(70.0f);
    GetCharacterMovement()->RotationRate = FRotator(0.0f, 0.0f, 720.0f);

    WeaponComponent = CreateDefaultSubobject<UBlasterWeaponComponent>("WeaponComponent");
    WeaponComponent->SetCharacter(this);
    WeaponComponent->SetIsReplicated(true);

    HealthComponent = CreateDefaultSubobject<UBlasterHealthComponent>("HealthComponent");
    HealthComponent->SetCharacter(this);
    HealthComponent->SetIsReplicated(true);

    if (auto PC = GetController<APlayerController>())
    {
        PC->SetAudioListenerOverride(RootComponent, FVector::ZeroVector, FRotator::ZeroRotator);
    }

    NetUpdateFrequency = 66.0f;
    MinNetUpdateFrequency = 33.0f;

    DissolveTimeline = CreateDefaultSubobject<UTimelineComponent>("DissolveTimeline");
}

void ABlasterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);

    DOREPLIFETIME_CONDITION(ABlasterCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void ABlasterCharacter::BeginPlay()
{
    Super::BeginPlay();

    check(OverheadWidget);
    check(WeaponComponent);
    check(HealthComponent);

    WeaponComponent->WeaponEquipped.AddUObject(this, &ThisClass::OnWeaponEquipped);
    WeaponComponent->WeaponUnequipped.AddUObject(this, &ThisClass::OnWeaponUnequipped);

    CameraCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCameraCollisionBeginOverlap);
    CameraCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnCameraCollisionEndOverlap);
}

void ABlasterCharacter::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (HasAuthority() || IsLocallyControlled())
    {
        AimOffset(DeltaTime);
    }
    else
    {
        TimeSinceLastMovementReplication += DeltaTime;
        if (TimeSinceLastMovementReplication > 0.25f)
        {
            OnRep_ReplicatedMovement();
        }
        CalculateAO_Pitch();
    }
}

void ABlasterCharacter::OnRep_ReplicatedMovement()
{
    Super::OnRep_ReplicatedMovement();

    SimProxiesTurn();
    TimeSinceLastMovementReplication = 0.0f;
}

void ABlasterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    PlayerInputComponent->BindAction("Jump", EInputEvent::IE_Pressed, this, &ThisClass::Jump);
    PlayerInputComponent->BindAction("Equip", EInputEvent::IE_Pressed, this, &ThisClass::EquipPressed);
    PlayerInputComponent->BindAction("Crouch", EInputEvent::IE_Pressed, this, &ThisClass::CrouchPressed);
    PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Pressed, this, &ThisClass::AimPressed);
    PlayerInputComponent->BindAction("Aim", EInputEvent::IE_Released, this, &ThisClass::AimReleased);
    PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Pressed, this, &ThisClass::FirePressed);
    PlayerInputComponent->BindAction("Fire", EInputEvent::IE_Released, this, &ThisClass::FireReleased);

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

void ABlasterCharacter::Jump()
{
    if (bIsCrouched)
    {
        UnCrouch();
    }
    else
    {
        Super::Jump();
    }
}

void ABlasterCharacter::EquipPressed()
{
    if (IsValid(WeaponComponent))
    {
        if (HasAuthority())
        {
            WeaponComponent->EquipWeapon(OverlappingWeapon);
            UnsetOverlappedWeapon(OverlappingWeapon);
        }
        else
        {
            ServerEquipPressed();
        }
    }
}

void ABlasterCharacter::ServerEquipPressed_Implementation()
{
    if (IsValid(WeaponComponent))
    {
        WeaponComponent->EquipWeapon(OverlappingWeapon);
        UnsetOverlappedWeapon(OverlappingWeapon);
    }
}

void ABlasterCharacter::CrouchPressed()
{
    if (GetCharacterMovement()->IsFalling()) return;

    if (bIsCrouched)
    {
        UnCrouch();
    }
    else
    {
        Crouch();
    }
}

void ABlasterCharacter::DisplayNetRole()
{
    if (!OverheadWidget) return;

    if (auto Widget = Cast<UOverheadWidget>(OverheadWidget->GetUserWidgetObject()))
    {
        Widget->SetDisplayText(UEnum::GetValueAsString<ENetRole>(GetRemoteRole()));
    }
}

void ABlasterCharacter::SetOverlappedWeapon(ABlasterBaseWeapon* Weapon)
{
    if (IsValid(Weapon))
    {
        if (IsLocallyControlled())
        {
            if (IsValid(OverlappingWeapon))
            {
                OverlappingWeapon->ShowPickupWidget(false);
            }
            Weapon->ShowPickupWidget(true);
        }

        OverlappingWeapon = Weapon;
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

bool ABlasterCharacter::IsWeaponEquipped() const
{
    return IsValid(WeaponComponent) && WeaponComponent->IsWeaponEquipped();
}

ABlasterBaseWeapon* ABlasterCharacter::GetCurrentWeapon() const
{
    if (!IsValid(WeaponComponent)) return nullptr;

    return WeaponComponent->GetCurrentWeapon();
}

void ABlasterCharacter::OnWeaponEquipped()
{
    bUseControllerRotationYaw = true;
    GetCharacterMovement()->bOrientRotationToMovement = false;
}

void ABlasterCharacter::OnWeaponUnequipped()
{
    bUseControllerRotationYaw = false;
    GetCharacterMovement()->bOrientRotationToMovement = true;
}

void ABlasterCharacter::AimPressed()
{
    if (!IsValid(WeaponComponent)) return;
    WeaponComponent->StartAiming();
}

void ABlasterCharacter::AimReleased()
{
    if (!IsValid(WeaponComponent)) return;
    WeaponComponent->StopAiming();
}

bool ABlasterCharacter::IsAiming() const
{
    return IsValid(WeaponComponent) && WeaponComponent->IsAiming();
}

void ABlasterCharacter::AimOffset(float DeltaTime)
{
    if (!IsValid(GetCurrentWeapon())) return;

    const float Speed = GetVelocity().Size2D();
    const bool bIsInAir = GetCharacterMovement()->IsFalling();

    if (FMath::IsNearlyZero(Speed) && !bIsInAir)  // standing still, not jumping
    {
        bRotateRootBone = true;
        FRotator CurrentAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
        FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
        AO_Yaw = DeltaAimRotation.Yaw;

        if (TurningInPlace == ETurningInPlace::ETIP_NotTurning)
        {
            InterpAO_Yaw = AO_Yaw;
        }

        bUseControllerRotationYaw = true;
        TurnInPlace(DeltaTime);
    }

    if (Speed > 0.0f || bIsInAir)  // running or jumping
    {
        bRotateRootBone = false;
        StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
        AO_Yaw = 0.0f;
        bUseControllerRotationYaw = true;

        TurningInPlace = ETurningInPlace::ETIP_NotTurning;
    }

    CalculateAO_Pitch();
}

void ABlasterCharacter::CalculateAO_Pitch()
{
    AO_Pitch = GetBaseAimRotation().Pitch;
    // fix for compressed data that come to the server
    if (AO_Pitch > 90.0f && !IsLocallyControlled())
    {
        // map pitch from [270, 360) to [-90, 0)
        const FVector2D InRange(270.0f, 360.0f);
        const FVector2D OutRange(-90.0f, 0.0f);
        AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
    }
}

void ABlasterCharacter::TurnInPlace(float DeltaTime)
{
    if (AO_Yaw > 90.0f)
    {
        TurningInPlace = ETurningInPlace::ETIP_Right;
    }
    else if (AO_Yaw < -90.0f)
    {
        TurningInPlace = ETurningInPlace::ETIP_Left;
    }

    if (TurningInPlace != ETurningInPlace::ETIP_NotTurning)
    {
        const float InterpSpeed = bIsCrouched ? 5.0f : 10.0f;
        InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.0f, DeltaTime, InterpSpeed);
        AO_Yaw = InterpAO_Yaw;
        if (FMath::Abs(AO_Yaw) < 15.0f)
        {
            TurningInPlace = ETurningInPlace::ETIP_NotTurning;
            StartingAimRotation = FRotator(0.0f, GetBaseAimRotation().Yaw, 0.0f);
        }
    }
}

float ABlasterCharacter::GetAimYaw() const
{
    return AO_Yaw;
}

float ABlasterCharacter::GetAimPitch() const
{
    return AO_Pitch;
}

ETurningInPlace ABlasterCharacter::GetTurningInPlace() const
{
    return TurningInPlace;
}

void ABlasterCharacter::FirePressed()
{
    if (!WeaponComponent) return;

    WeaponComponent->StartFire();
}

void ABlasterCharacter::FireReleased()
{
    if (!WeaponComponent) return;

    WeaponComponent->StopFire();
}

FVector ABlasterCharacter::GetHitTargetNoSpread() const
{
    if (!WeaponComponent) return FVector();

    return WeaponComponent->GetHitTargetNoSpread();
}

void ABlasterCharacter::OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
    CheckCameraOverlap();
}

void ABlasterCharacter::OnCameraCollisionEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
    CheckCameraOverlap();
}

void ABlasterCharacter::CheckCameraOverlap()
{
    const auto HideMesh = CameraCollisionComponent->IsOverlappingComponent(GetCapsuleComponent());
    GetMesh()->SetOwnerNoSee(HideMesh);

    TArray<USceneComponent*> MeshChildren;
    GetMesh()->GetChildrenComponents(true, MeshChildren);

    for (auto Child : MeshChildren)
    {
        const auto ChildGeometry = Cast<UPrimitiveComponent>(Child);
        if (ChildGeometry)
        {
            ChildGeometry->SetOwnerNoSee(HideMesh);
        }
    }
}

void ABlasterCharacter::HitByProjectile()
{
    if (IsAlive())
    {
        MulticastHit();
    }
}

void ABlasterCharacter::MulticastHit_Implementation()
{
    PlayHitReactMontage();
}

void ABlasterCharacter::PlayHitReactMontage()
{
    if (!IsValid(GetMesh()) || !HitReactMontage) return;
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon()) return;

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Play(HitReactMontage);
        AnimInstance->Montage_JumpToSection("FromFront");
    }
}

void ABlasterCharacter::SimProxiesTurn()
{
    if (!WeaponComponent || !WeaponComponent->GetCurrentWeapon()) return;

    bRotateRootBone = false;
}

bool ABlasterCharacter::IsAlive() const
{
    return HealthComponent && HealthComponent->IsAlive();
}

void ABlasterCharacter::Elim()
{
    MulticastElim();
    GetWorldTimerManager().SetTimer(ElimTimer, this, &ThisClass::ElimTimerFinished, ElimDelay);
}

void ABlasterCharacter::MulticastElim_Implementation()
{
    if (bIsElimmed) return;
    
    bIsElimmed = true;
    PlayElimMontage();

    if (DissolveMaterialInstance)
    {
        DynamicDissolveMaterialInstance = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
        GetMesh()->SetMaterial(0, DynamicDissolveMaterialInstance);
        DynamicDissolveMaterialInstance->SetScalarParameterValue("Dissolve", 0.6f);
        DynamicDissolveMaterialInstance->SetScalarParameterValue("Glow", 100.0f);
    }
    StartDissolve();
    StopMovement();

    GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
    GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void ABlasterCharacter::PlayElimMontage()
{
    if (!IsValid(GetMesh()) || !ElimMontage) return;

    if (UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance())
    {
        AnimInstance->Montage_Play(ElimMontage);
    }
}

void ABlasterCharacter::ElimTimerFinished() 
{
    auto BlasterGameMode = GetWorld()->GetAuthGameMode<ABlasterGameMode>();
    if (BlasterGameMode)
    {
        BlasterGameMode->RequestRespawn(this, Controller);
    }
}

void ABlasterCharacter::StartDissolve() 
{
    if (!DissolveCurve || !DissolveTimeline) return;

    DissolveTrack.BindDynamic(this, &ThisClass::UpdateDissolveMaterial);
    DissolveTimeline->AddInterpFloat(DissolveCurve, DissolveTrack);
    DissolveTimeline->Play();
}

void ABlasterCharacter::UpdateDissolveMaterial(float DissolveValue) 
{
    if (!DynamicDissolveMaterialInstance) return;

    DynamicDissolveMaterialInstance->SetScalarParameterValue("Dissolve", -DissolveValue);
}

void ABlasterCharacter::StopMovement() 
{
    GetCharacterMovement()->DisableMovement();
    GetCharacterMovement()->StopMovementImmediately();
    if (auto PC = GetController<APlayerController>())
    {
        DisableInput(PC);
    }
}
