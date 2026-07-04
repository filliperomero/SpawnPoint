// Copyright Fillipe Romero. All Rights Reserved.

#include "SpawnPoint/Public/Characters/SP_ShooterCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SP_CombatComponent.h"
#include "Data/SP_WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Weapons/SP_Weapon.h"

ASP_ShooterCharacter::ASP_ShooterCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	GetCharacterMovement()->MovementState.bCanCrouch = true;
	
	SpringArm = CreateDefaultSubobject<USpringArmComponent>("SpringArm");
	SpringArm->SetupAttachment(GetRootComponent());
	SpringArm->TargetArmLength = 0.f;
	SpringArm->bEnableCameraLag = true;
	SpringArm->CameraLagSpeed = 15.f;
	SpringArm->bUsePawnControlRotation = true;
	
	FirstPersonCamera = CreateDefaultSubobject<UCameraComponent>("FirstPersonCamera");
	FirstPersonCamera->SetupAttachment(SpringArm);
	FirstPersonCamera->bUsePawnControlRotation = false;
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->SetupAttachment(FirstPersonCamera);
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	
	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	
	CombatComponent = CreateDefaultSubobject<USP_CombatComponent>("CombatComponent");
	CombatComponent->SetIsReplicated(true);
}

void ASP_ShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	FirstPersonCamera->SetFieldOfView(DefaultFieldOfView);
	StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
}

void ASP_ShooterCharacter::BeginDestroy()
{
	Super::BeginDestroy();
	
	if (IsValid(CombatComponent))
	{
		CombatComponent->DestroyInventory();
	}
}

FRotator ASP_ShooterCharacter::GetFixedAimRotation() const
{
	FRotator AimRotation = GetBaseAimRotation();
	
	if (AimRotation.Pitch > 90.f && !IsLocallyControlled())
	{
		// Map pitch from [270, 360) to [-90, 0]
		// We need to do this since there is a compression made on this value for multiplayer and we need to map it properly
		const FVector2D InRange(270.f, 360.f);
		const FVector2D OutRange(-90.f, 0.f);
		
		AimRotation.Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AimRotation.Pitch);
	}
	
	return AimRotation;
}

bool ASP_ShooterCharacter::HasCurrentWeapon() const
{
	return IsValid(CombatComponent) && CombatComponent->GetCurrentWeapon() != nullptr;
}

void ASP_ShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateFABRIKSocketTransform();
	CalculateTurnInPlaceParameters(DeltaTime);
}

void ASP_ShooterCharacter::CalculateFABRIKSocketTransform()
{
	if (IsValid(CombatComponent) && IsValid(CombatComponent->GetCurrentWeapon()) && IsValid(CombatComponent->GetCurrentWeapon()->GetMesh3P()))
	{
		FABRIK_SocketTransform = CombatComponent->GetCurrentWeapon()->GetMesh3P()->GetSocketTransform("FABRIK_Socket", RTS_World);
		
		FVector OutLocation;
		FRotator OutRotation;
		GetMesh()->TransformToBoneSpace("hand_r", FABRIK_SocketTransform.GetLocation(), FABRIK_SocketTransform.GetRotation().Rotator(), OutLocation, OutRotation);
		
		FABRIK_SocketTransform.SetLocation(OutLocation);
		FABRIK_SocketTransform.SetRotation(OutRotation.Quaternion());
	}
}

void ASP_ShooterCharacter::CalculateTurnInPlaceParameters(float DeltaTime)
{
	FVector Velocity = GetVelocity();
	Velocity.Z = 0.f;
	const float Speed = Velocity.Size();
	const bool bIsInAir = GetCharacterMovement()->IsFalling();
	
	if (Speed <= 0.f && !bIsInAir) // Standing still, not jumping
	{
		const FRotator CurrentAimRotation(0.f, GetBaseAimRotation().Yaw, 0.f);
		// StartingAimRotation initially set in BeginPlay
		const FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(CurrentAimRotation, StartingAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;
		
		if (TurningStatus == ETurningInPlace::NotTurning)
		{
			InterpAO_Yaw = AO_Yaw;
		}
		
		TurnInPlace(DeltaTime);
	}
	
	if (Speed > 0.f || bIsInAir) // Running or jumping
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
		
		const FRotator AimRotation = GetBaseAimRotation();
		const FRotator MovementRotation = UKismetMathLibrary::MakeRotFromX(GetVelocity());
		MovementOffsetYaw = UKismetMathLibrary::NormalizedDeltaRotator(MovementRotation, AimRotation).Yaw;
		
		TurningStatus = ETurningInPlace::NotTurning;
	}
	
	AO_Yaw *= -1.f;
}

void ASP_ShooterCharacter::TurnInPlace(float DeltaTime)
{
	if (AO_Yaw > 90.f)
	{
		TurningStatus = ETurningInPlace::Right;
	}
	else if (AO_Yaw < -90.f)
	{
		TurningStatus = ETurningInPlace::Left;
	}
	
	if (TurningStatus != ETurningInPlace::NotTurning) // Means we're turning left or right
	{
		InterpAO_Yaw = FMath::FInterpTo(InterpAO_Yaw, 0.f, DeltaTime, 4.0f);
		AO_Yaw = InterpAO_Yaw;
		
		if (FMath::Abs(AO_Yaw) < 5.f)
		{
			TurningStatus = ETurningInPlace::NotTurning;
			StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		}
	}
}

void ASP_ShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	
	UEnhancedInputComponent* SPInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);
	SPInputComponent->BindAction(CycleWeaponAction, ETriggerEvent::Started, this, &ThisClass::InputCycleWeapon);
	SPInputComponent->BindAction(ReloadWeaponAction, ETriggerEvent::Started, this, &ThisClass::InputReloadWeapon);
	SPInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Started, this, &ThisClass::InputFireWeaponPressed);
	SPInputComponent->BindAction(FireWeaponAction, ETriggerEvent::Completed, this, &ThisClass::InputFireWeaponReleased);
	SPInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Started, this, &ThisClass::InputAimPressed);
	SPInputComponent->BindAction(AimWeaponAction, ETriggerEvent::Completed, this, &ThisClass::InputAimReleased);
}

void ASP_ShooterCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	
	if (IsValid(CombatComponent))
	{
		CombatComponent->SpawnInventory();
	}
}

FName ASP_ShooterCharacter::GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const
{
	checkf(CombatComponent->WeaponData, TEXT("No Weapon Data Asset - Fill out BP_SP_ShooterCharacter"))
	
	return CombatComponent->WeaponData->GripPoints.FindChecked(WeaponType);
}

USkeletalMeshComponent* ASP_ShooterCharacter::GetMesh1P_Implementation() const
{
	return Mesh1P;
}

USkeletalMeshComponent* ASP_ShooterCharacter::GetMesh3P_Implementation() const
{
	return GetMesh();
}

void ASP_ShooterCharacter::InputCycleWeapon()
{
	CombatComponent->InitiateCycleWeapon();
}

void ASP_ShooterCharacter::InputReloadWeapon()
{
	CombatComponent->InitiateReloadWeapon();
}

void ASP_ShooterCharacter::InputFireWeaponPressed()
{
	CombatComponent->InitiateFireWeaponPressed();
}

void ASP_ShooterCharacter::InputFireWeaponReleased()
{
	CombatComponent->InitiateFireWeaponReleased();
}

void ASP_ShooterCharacter::InputAimPressed()
{
	CombatComponent->InitiateAimPressed();
	OnAim(true);
}

void ASP_ShooterCharacter::InputAimReleased()
{
	CombatComponent->InitiateAimReleased();
	OnAim(false);
}
