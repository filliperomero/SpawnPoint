// Copyright Fillipe Romero. All Rights Reserved.

#include "Weapons/SP_Weapon.h"

#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/SP_PlayerInterface.h"

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

void ASP_Weapon::OnRep_Instigator()
{
	Super::OnRep_Instigator();
	
	// This will make sure we are attaching to the Owning Pawn on the clients
	AttachToOwningPawn();
}

void ASP_Weapon::BeginPlay()
{
	Super::BeginPlay();
}

void ASP_Weapon::AttachToOwningPawn() const
{
	APawn* OwningPawn = GetInstigator();
	if (!IsValid(OwningPawn) || !OwningPawn->Implements<USP_PlayerInterface>()) return;
	
	SetMeshVisibilities(OwningPawn);
	
	const FName AttachPoint = ISP_PlayerInterface::Execute_GetWeaponAttachPoint(OwningPawn, WeaponType);
	USkeletalMeshComponent* PawnMesh1P = ISP_PlayerInterface::Execute_GetMesh1P(OwningPawn);
	USkeletalMeshComponent* PawnMesh3P = ISP_PlayerInterface::Execute_GetMesh3P(OwningPawn);
	
	Mesh1P->AttachToComponent(PawnMesh1P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
	Mesh3P->AttachToComponent(PawnMesh3P, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
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
