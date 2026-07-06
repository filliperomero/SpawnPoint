// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SP_CombatComponent.generated.h"

struct FHitResult;
class ASP_Weapon;
class USP_WeaponData;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPAWNPOINT_API USP_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USP_CombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	/* Cycle to the next weapon in the inventory */
	void InitiateCycleWeapon();
	void InitiateFireWeaponPressed();
	void InitiateFireWeaponReleased();
	void InitiateReloadWeapon();
	void InitiateAimPressed();
	void InitiateAimReleased();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnPoint|Weapon")
	TObjectPtr<USP_WeaponData> WeaponData;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming { false };
	
	void Equip(ASP_Weapon* WeaponToEquip);
	void SpawnInventory();
	void DestroyInventory();
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Weapon")
	float TraceLength { 20000.f };

private:
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<ASP_Weapon> CurrentWeapon;
	
	UFUNCTION()
	void OnRep_CurrentWeapon(ASP_Weapon* PrevWeapon);
	
	UPROPERTY(Transient, Replicated)
	TArray<ASP_Weapon*> Inventory;
	
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Weapon")
	TArray<TSubclassOf<ASP_Weapon>> DefaultWeaponClasses;
	
	ASP_Weapon* SpawnWeapon(TSubclassOf<ASP_Weapon> WeaponClass) const;
	
	UFUNCTION(Server, Reliable)
	void Server_Aim(bool bPressed);
	
	UFUNCTION(Server, Reliable)
	void Server_FireWeapon(const FHitResult& Hit);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_FireWeapon(const FHitResult& Hit);
	
	void Local_Aim(bool bPressed);
	void Local_FireWeapon();
	
public:
	ASP_Weapon* GetCurrentWeapon() { return CurrentWeapon; };
};
