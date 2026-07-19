// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "SP_ReserveAmmoWidget.generated.h"

class ASP_Weapon;
class UImage;
class UTextBlock;

UCLASS()
class SPAWNPOINT_API USP_ReserveAmmoWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeOnInitialized() override;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Ammo;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_WeaponIcon;
	
private:
	UFUNCTION()
	void OnPossessedPawnChanged(APawn* OldPawn, APawn* NewPawn);
	
	UFUNCTION()
	void OnCurrentReserveAmmoChanged(int32 RoundsInReserve, int32 RoundsInWeapon);

	UFUNCTION()
	void OnRoundFired(int32 RoundsCurrent, int32 RoundsMax, int32 RoundsInReserve);
	
	UFUNCTION()
	void OnWeaponFirstReplicated(ASP_Weapon* Weapon, bool bTargetingPlayer);
};
