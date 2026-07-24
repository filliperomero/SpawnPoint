// Copyright Fillipe Romero. All Rights Reserved.

#include "Weapons/SP_Weapon.h"

#include "KismetTraceUtils.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/SP_PlayerInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "SpawnPoint/SpawnPoint.h"

ASP_Weapon::ASP_Weapon()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	bNetUseOwnerRelevancy = true;
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh1P");
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetHiddenInGame(true);
	SetRootComponent(Mesh1P);
	
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>("Mesh3P");
	Mesh3P->SetupAttachment(Mesh1P);
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetHiddenInGame(true);
}

void ASP_Weapon::BeginPlay()
{
	Super::BeginPlay();
}

void ASP_Weapon::AttachToOwningPawn(APawn* Pawn) const
{
	if (!IsValid(Pawn) || !Pawn->Implements<USP_PlayerInterface>()) return;
	
	SetMeshVisibilities(Pawn);
	
	const FName AttachPoint = ISP_PlayerInterface::Execute_GetWeaponAttachPoint(Pawn, WeaponType);
	USkeletalMeshComponent* PawnMesh1P = ISP_PlayerInterface::Execute_GetMesh1P(Pawn);
	USkeletalMeshComponent* PawnMesh3P = ISP_PlayerInterface::Execute_GetMesh3P(Pawn);
	
	Mesh1P->AttachToComponent(PawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	Mesh3P->AttachToComponent(PawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
}

void ASP_Weapon::WeaponTrace(FHitResult& OutHitResult, float TraceLength)
{
	FCollisionQueryParams QueryParams;
	QueryParams.bReturnPhysicalMaterial = true;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldStatic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_WorldDynamic, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);
	
	ensure(GetInstigator());
	
	if (APlayerController* PC = Cast<APlayerController>(GetInstigator()->GetController()); IsValid(PC))
	{
		FVector EyesWorldLocation;
		FRotator EyesWorldRotation;
		PC->GetActorEyesViewPoint(EyesWorldLocation, EyesWorldRotation);
		const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesWorldRotation);
		
		const FVector Start = EyesWorldLocation;
		const FVector End = Start + EyesWorldDirection * TraceLength;
		
		const bool bHit = GetWorld()->SweepSingleByChannel(
			OutHitResult, 
			Start, 
			End, 
			FQuat::Identity, 
			SpawnPointTraceChannels::ECC_Weapon,
			FCollisionShape::MakeSphere(TraceRadius),
			QueryParams,
			ResponseParams);
		
		if (!bHit)
		{
			// That way, when we hit the sky, it does not go to the origin of the map
			OutHitResult.ImpactPoint = End;
		}
	}
}

void ASP_Weapon::Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson)
{
	FireEffects(ImpactPoint, ImpactNormal, ImpactSurfaceType, bIsFirstPerson);
	
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
		++Sequence;
	}
}

void ASP_Weapon::Auth_Fire()
{
	Ammo = FMath::Clamp(Ammo - 1, 0, MagCapacity);
}

void ASP_Weapon::Rep_Fire(int32 AuthAmmo)
{
	if (GetInstigator()->IsLocallyControlled())
	{
		Ammo = AuthAmmo;
		--Sequence;
		Ammo -= Sequence;
	}
}

UMaterialInstanceDynamic* ASP_Weapon::GetReticleDynamicMaterialInstance()
{
	checkf(ReticleMaterial, TEXT("Assign Reticle Material in %s"), *GetName());
	
	if (!IsValid(DynMatInst_Reticle))
	{
		DynMatInst_Reticle = UMaterialInstanceDynamic::Create(ReticleMaterial, this);
	}
	
	return DynMatInst_Reticle;
}

UMaterialInstanceDynamic* ASP_Weapon::GetAmmoCounterDynamicMaterialInstance()
{
	checkf(AmmoCounterMaterial, TEXT("Assign Ammo Counter Material in %s"), *GetName());
	
	if (!IsValid(DynMatInst_AmmoCounter))
	{
		DynMatInst_AmmoCounter = UMaterialInstanceDynamic::Create(AmmoCounterMaterial, this);
	}
	
	return DynMatInst_AmmoCounter;
}

void ASP_Weapon::SetMeshVisibilities(APawn* OwningPawn) const
{
	if (OwningPawn->IsLocallyControlled())
	{
		Mesh1P->SetHiddenInGame(false);
		Mesh3P->SetHiddenInGame(true);
	}
	else
	{
		Mesh1P->SetHiddenInGame(true);
		Mesh3P->SetHiddenInGame(false);
	}
}
