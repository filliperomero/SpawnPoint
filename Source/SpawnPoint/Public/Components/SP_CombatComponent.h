// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SP_CombatComponent.generated.h"

class ASP_Weapon;
class USP_WeaponData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPAWNPOINT_API USP_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USP_CombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	
	/* Cycle to the next weapon in the inventory */
	void InitiateCycleWeapon();
	void InitiateFireWeaponPressed();
	void InitiateFireWeaponReleased();
	void InitiateReloadWeapon();
	void InitiateAimPressed();
	void InitiateAimReleased();
	
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Weapon")
	TObjectPtr<USP_WeaponData> WeaponData;
	
	void SpawnInventory();
	void DestroyInventory();
	
protected:

private:
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Weapon")
	TSubclassOf<ASP_Weapon> DefaultWeaponClass;
	
	ASP_Weapon* SpawnWeapon(TSubclassOf<ASP_Weapon> WeaponClass) const;
};
