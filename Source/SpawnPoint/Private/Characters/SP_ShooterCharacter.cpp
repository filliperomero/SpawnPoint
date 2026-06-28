// Copyright Fillipe Romero. All Rights Reserved.

#include "SpawnPoint/Public/Characters/SP_ShooterCharacter.h"

#include "EnhancedInputComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/SP_CombatComponent.h"
#include "Data/SP_WeaponData.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

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
}

void ASP_ShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
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
}

void ASP_ShooterCharacter::InputAimReleased()
{
	CombatComponent->InitiateAimReleased();
}

