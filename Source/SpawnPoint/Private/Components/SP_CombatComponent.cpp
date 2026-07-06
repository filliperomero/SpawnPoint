// Copyright Fillipe Romero. All Rights Reserved.

#include "Components/SP_CombatComponent.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Components/SkeletalMeshComponent.h"
#include "Data/SP_WeaponData.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Interfaces/SP_PlayerInterface.h"
#include "Net/UnrealNetwork.h"
#include "Weapons/SP_Weapon.h"

USP_CombatComponent::USP_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USP_CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USP_CombatComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ThisClass, Inventory);
	DOREPLIFETIME(ThisClass, CurrentWeapon);
	DOREPLIFETIME_CONDITION(ThisClass, bAiming, COND_SkipOwner);
}

void USP_CombatComponent::InitiateCycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate Cycle Weapon"), false);
}

void USP_CombatComponent::InitiateFireWeaponPressed()
{
	Local_FireWeapon();
}

void USP_CombatComponent::InitiateFireWeaponReleased()
{
}

void USP_CombatComponent::Local_FireWeapon()
{
	ensure(IsValid(WeaponData));
	
	UAnimMontage* Montage1P = WeaponData->FirstPersonMontages.FindChecked(CurrentWeapon->GetWeaponType()).FireMontage;
	USkeletalMeshComponent* Mesh1P = ISP_PlayerInterface::Execute_GetMesh1P(GetOwner());
	
	if (IsValid(Mesh1P) && IsValid(Montage1P))
	{
		Mesh1P->GetAnimInstance()->Montage_Play(Montage1P);
	}
	
	Server_FireWeapon();
}

void USP_CombatComponent::Server_FireWeapon_Implementation()
{
	Multicast_FireWeapon();
}

void USP_CombatComponent::Multicast_FireWeapon_Implementation()
{
	APawn* OwningPawn = Cast<APawn>(GetOwner());
	
	if (OwningPawn->IsLocallyControlled())
	{
		// TODO: Do Locally-controller stuff.
	}
	else
	{
		ensure(IsValid(WeaponData));
		
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
}

void USP_CombatComponent::Equip(ASP_Weapon* WeaponToEquip)
{
	CurrentWeapon = WeaponToEquip;
	CurrentWeapon->AttachToOwningPawn();
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
		}
	}
	
	if (Inventory.Num() > 0)
	{
		Equip(Inventory[0]);
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

void USP_CombatComponent::OnRep_CurrentWeapon(ASP_Weapon* PrevWeapon)
{
	if (!IsValid(CurrentWeapon)) return;
	
	CurrentWeapon->AttachToOwningPawn();
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
