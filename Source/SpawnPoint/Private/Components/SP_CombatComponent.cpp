// Copyright Fillipe Romero. All Rights Reserved.

#include "Components/SP_CombatComponent.h"

#include "TimerManager.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/SP_WeaponData.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/SP_PlayerInterface.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "SpawnPoint/SpawnPoint.h"
#include "Weapons/SP_Weapon.h"

USP_CombatComponent::USP_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USP_CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	if (!IsValid(OwningPawn) || !OwningPawn->IsLocallyControlled()) return;
	
	APlayerController* PC = Cast<APlayerController>(OwningPawn->GetController());
	if (!IsValid(PC)) return;
	
	FVector EyesWorldLocation;
	FRotator EyesRotation;
	PC->GetActorEyesViewPoint(EyesWorldLocation, EyesRotation);
	const FVector EyesWorldDirection = UKismetMathLibrary::GetForwardVector(EyesRotation);
	
	const FVector Start = EyesWorldLocation;
	const FVector End = Start + EyesWorldDirection * TraceLength;
	
	FHitResult Hit;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(GetOwner());
	
	FCollisionResponseParams ResponseParams;
	ResponseParams.CollisionResponse.SetAllChannels(ECR_Ignore);
	ResponseParams.CollisionResponse.SetResponse(ECC_Pawn, ECR_Block);
	ResponseParams.CollisionResponse.SetResponse(ECC_PhysicsBody, ECR_Block);
	
	GetWorld()->LineTraceSingleByChannel(Hit, Start, End, SpawnPointTraceChannels::ECC_Weapon, QueryParams, ResponseParams);
	
	bHitPlayer = IsValid(Hit.GetActor()) && Hit.GetActor()->Implements<USP_PlayerInterface>();
	
	if (bHitPlayer != bHitPlayerLastFrame)
	{
		OnTargetingPlayerStatusChanged.Broadcast(bHitPlayer);
	}
	
	bHitPlayerLastFrame = bHitPlayer;
}

void USP_CombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Inventory);
	DOREPLIFETIME(ThisClass, CurrentWeapon);
	DOREPLIFETIME_CONDITION(ThisClass, bAiming, COND_SkipOwner);
	DOREPLIFETIME_CONDITION(ThisClass, CurrentReserveAmmo, COND_OwnerOnly);
}

void USP_CombatComponent::InitiateCycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate Cycle Weapon"), false);
}

void USP_CombatComponent::InitiateFireWeaponPressed()
{
	if (!IsValid(CurrentWeapon)) return;
	
	bFireTriggerPressed = true;
	
	if (CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void USP_CombatComponent::InitiateFireWeaponReleased()
{
	bFireTriggerPressed = false;
}

void USP_CombatComponent::Local_FireWeapon()
{
	if (!IsValid(CurrentWeapon)) return;
	
	ensure(IsValid(WeaponData));
	
	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->GetWeaponType()).FireMontage;
	USkeletalMeshComponent* Mesh1P = ISP_PlayerInterface::Execute_GetMesh1P(GetOwner());
	
	if (IsValid(Mesh1P) && IsValid(Montage1P))
	{
		Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
	}
	
	FHitResult Hit;
	CurrentWeapon->WeaponTrace(Hit, TraceLength);
	
	EPhysicalSurface ImpactSurfaceType = Hit.PhysMaterial.IsValid(false) 
		? Hit.PhysMaterial->SurfaceType.GetValue() 
		: SurfaceType1;
	CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, ImpactSurfaceType, true);
	
	OnRoundFired.Broadcast(CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
	
	GetWorld()->GetTimerManager().SetTimer(FireTimer, this, &ThisClass::FireTimerFinished, CurrentWeapon->FireRate);
	Server_FireWeapon(Hit);
}

void USP_CombatComponent::FireTimerFinished()
{
	if (!IsValid(CurrentWeapon)) return;
	
	if (bFireTriggerPressed && CurrentWeapon->FireType == EFireType::Auto && CurrentWeapon->Ammo > 0)
	{
		Local_FireWeapon();
	}
}

void USP_CombatComponent::Server_FireWeapon_Implementation(const FHitResult& Hit)
{
	if (!IsValid(CurrentWeapon)) return;
	
	if (GetNetMode() != NM_ListenServer || !Cast<APawn>(GetOwner())->IsLocallyControlled())
	{
		CurrentWeapon->Auth_Fire();
	}
	
	Multicast_FireWeapon(Hit, CurrentWeapon->Ammo);
}

void USP_CombatComponent::Multicast_FireWeapon_Implementation(const FHitResult& Hit, int32 AuthAmmo)
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	
	if (OwningPawn->IsLocallyControlled())
	{
		CurrentWeapon->Rep_Fire(AuthAmmo);
	}
	else
	{
		ensure(IsValid(WeaponData));
		
		EPhysicalSurface ImpactSurfaceType = Hit.PhysMaterial.IsValid(false) 
			? Hit.PhysMaterial->SurfaceType.GetValue() 
			: SurfaceType1;
		CurrentWeapon->Local_Fire(Hit.ImpactPoint, Hit.ImpactNormal, ImpactSurfaceType, false);
		
		UAnimMontage* Montage3P = WeaponData->ThirdPersonMontages.FindChecked(CurrentWeapon->GetWeaponType()).FireMontage;
		USkeletalMeshComponent* Mesh3P = ISP_PlayerInterface::Execute_GetMesh3P(GetOwner());
	
		if (IsValid(Mesh3P) && IsValid(Montage3P))
		{
			Mesh3P->GetAnimInstance()->Montage_Play(Montage3P);
		}
	}
}

void USP_CombatComponent::InitiateReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Initiate Reload Weapon"), false);
}

void USP_CombatComponent::InitiateAimPressed()
{
	Local_Aim(true);
	Server_Aim(true);
}

void USP_CombatComponent::InitiateAimReleased()
{
	Local_Aim(false);
	Server_Aim(false);
}

void USP_CombatComponent::Server_Aim_Implementation(bool bPressed)
{
	Local_Aim(bPressed);
}

void USP_CombatComponent::Local_Aim(bool bPressed)
{
	bAiming = bPressed;
	OnAimingStatusChanged.Broadcast(bAiming);
}

void USP_CombatComponent::Equip(ASP_Weapon* WeaponToEquip)
{
	CurrentWeapon = WeaponToEquip;
	CurrentWeapon->AttachToOwningPawn();
	
	CurrentReserveAmmo = ReserveAmmo.FindChecked(CurrentWeapon->GetWeaponType());
	OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, CurrentWeapon->Ammo);
}

void USP_CombatComponent::SpawnInventory()
{
	if (GetOwner()->GetLocalRole() < ROLE_Authority) return;

	for (TSubclassOf<ASP_Weapon>& WeaponClass : DefaultWeaponClasses)
	{
		ASP_Weapon* NewWeapon = SpawnWeapon(WeaponClass);

		if (IsValid(NewWeapon))
		{
			Inventory.AddUnique(NewWeapon);
			ReserveAmmo.Add(NewWeapon->GetWeaponType(), NewWeapon->StartingCarriedAmmo);
		}
	}
	
	if (Inventory.Num() > 0)
	{
		Equip(Inventory[0]);
		InitializeWeaponWidgets();
	}
}

void USP_CombatComponent::DestroyInventory()
{
	for (ASP_Weapon* Weapon : Inventory)
	{
		if (IsValid(Weapon))
		{
			Weapon->Destroy();
		}
	}
	
	Inventory.Empty();
}

void USP_CombatComponent::InitializeWeaponWidgets()
{
	if (IsValid(CurrentWeapon))
	{
		OnReticleChanged.Broadcast(CurrentWeapon->GetReticleDynamicMaterialInstance(), CurrentWeapon->ReticleParams, bHitPlayer);
		OnAmmoCounterChanged.Broadcast(CurrentWeapon->GetAmmoCounterDynamicMaterialInstance(), CurrentWeapon->Ammo, CurrentWeapon->MagCapacity);
	}
}

void USP_CombatComponent::OnRep_CurrentWeapon(ASP_Weapon* PrevWeapon)
{
	if (!IsValid(CurrentWeapon)) return;
	
	CurrentWeapon->AttachToOwningPawn();
	ISP_PlayerInterface::Execute_WeaponReplicated(GetOwner());
	InitializeWeaponWidgets();
}

void USP_CombatComponent::OnRep_CurrentReserveAmmo()
{
	if (!IsValid(CurrentWeapon)) return;
	
	OnCurrentReserveAmmoChanged.Broadcast(CurrentReserveAmmo, CurrentWeapon->Ammo);
}

ASP_Weapon* USP_CombatComponent::SpawnWeapon(TSubclassOf<ASP_Weapon> WeaponClass) const
{
	AActor* OwningActor = GetOwner();
	
	if (!IsValid(OwningActor)) return nullptr;
	if (OwningActor->GetLocalRole() < ROLE_Authority) return nullptr;
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Instigator = Cast<APawn>(OwningActor);
	SpawnParams.Owner = OwningActor;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	
	return GetWorld()->SpawnActor<ASP_Weapon>(WeaponClass, SpawnParams);
}
