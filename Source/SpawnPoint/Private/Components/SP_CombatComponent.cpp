// Copyright Fillipe Romero. All Rights Reserved.

#include "Components/SP_CombatComponent.h"

#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "Weapons/SP_Weapon.h"

USP_CombatComponent::USP_CombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void USP_CombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USP_CombatComponent::InitiateCycleWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, TEXT("Initiate Cycle Weapon"), false);
}

void USP_CombatComponent::InitiateFireWeaponPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Initiate Fire Weapon Pressed"), false);
}

void USP_CombatComponent::InitiateFireWeaponReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Initiate Fire Weapon Released"), false);
}

void USP_CombatComponent::InitiateReloadWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Black, TEXT("Initiate Reload Weapon"), false);
}

void USP_CombatComponent::InitiateAimPressed()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Initiate Aim Pressed"), false);
}

void USP_CombatComponent::InitiateAimReleased()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Initiate Aim Released"), false);
}

void USP_CombatComponent::SpawnInventory()
{
	ASP_Weapon* NewWeapon = SpawnWeapon(DefaultWeaponClass);
	
	// Important check since it will only exist on Servers
	if (IsValid(NewWeapon))
	{
		NewWeapon->AttachToOwningPawn();
	}
}

void USP_CombatComponent::DestroyInventory()
{
	// TODO: Destroy the inventory once we have one.
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

