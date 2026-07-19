// Copyright Fillipe Romero. All Rights Reserved.

#include "UI/SP_ReserveAmmoWidget.h"

#include "Characters/SP_ShooterCharacter.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Components/SP_CombatComponent.h"
#include "Interfaces/SP_PlayerInterface.h"
#include "Weapons/SP_Weapon.h"

class ASP_ShooterCharacter;

void USP_ReserveAmmoWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	
	Image_WeaponIcon->SetRenderOpacity(0.f);
	Text_Ammo->SetRenderOpacity(0.f);
	
	/** 
	 * The code below is to solve any possible race condition since we cannot promise
	 * the order that it will happen. Maybe once we possess, the weapon will be valid already (and already replicated), maybe not.
	 */
	
	GetOwningPlayer()->OnPossessedPawnChanged.AddDynamic(this, &ThisClass::OnPossessedPawnChanged);
	
	ASP_ShooterCharacter* ShooterCharacter = Cast<ASP_ShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	USP_CombatComponent* Combat = USP_CombatComponent::FindCombatComponent(ShooterCharacter);
	if (!IsValid(Combat)) return;
	
	OnPossessedPawnChanged(nullptr, ShooterCharacter);
	
	if (ShooterCharacter->HasWeaponFirstReplicated())
	{
		ASP_Weapon* Weapon = ISP_PlayerInterface::Execute_GetCurrentWeapon(ShooterCharacter);
		if (IsValid(Weapon))
		{
			OnCurrentReserveAmmoChanged(ISP_PlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
		}
	}
	else
	{
		ShooterCharacter->OnWeaponFirstReplicated.AddDynamic(this, &ThisClass::OnWeaponFirstReplicated);
	}
}

void USP_ReserveAmmoWidget::OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn)
{
	USP_CombatComponent* OldPawnCombat = USP_CombatComponent::FindCombatComponent(OldPawn);
	if (IsValid(OldPawnCombat))
	{
		OldPawnCombat->OnCurrentReserveAmmoChanged.RemoveDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		OldPawnCombat->OnRoundFired.RemoveDynamic(this, &ThisClass::OnRoundFired);
	}
	
	USP_CombatComponent* NewPawnCombat = USP_CombatComponent::FindCombatComponent(NewPawn);
	if (IsValid(NewPawnCombat))
	{
		Image_WeaponIcon->SetRenderOpacity(1.f);
		Text_Ammo->SetRenderOpacity(1.f);
		
		NewPawnCombat->OnCurrentReserveAmmoChanged.AddDynamic(this, &ThisClass::OnCurrentReserveAmmoChanged);
		NewPawnCombat->OnRoundFired.AddDynamic(this, &ThisClass::OnRoundFired);
	}
}

void USP_ReserveAmmoWidget::OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon)
{
	if (IsValid(Text_Ammo))
	{
		const FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}"), RoundsInWeapon, RoundsInReserve);
		Text_Ammo->SetText(AmmoText);
	}
}

void USP_ReserveAmmoWidget::OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve)
{
	if (!IsValid(Text_Ammo)) return;

	const FText AmmoText = FText::Format(NSLOCTEXT("AmmoText", "AmmoKey", "{0}/{1}"), RoundsCurrent, RoundsInReserve);
	Text_Ammo->SetText(AmmoText);
}

void USP_ReserveAmmoWidget::OnWeaponFirstReplicated(ASP_Weapon* Weapon, bool bTargetingPlayer)
{
	ASP_ShooterCharacter* ShooterCharacter = Cast<ASP_ShooterCharacter>(GetOwningPlayer()->GetPawn());
	if (!IsValid(ShooterCharacter)) return;
	
	OnCurrentReserveAmmoChanged(ISP_PlayerInterface::Execute_GetReserveAmmo(ShooterCharacter), Weapon->Ammo);
}
