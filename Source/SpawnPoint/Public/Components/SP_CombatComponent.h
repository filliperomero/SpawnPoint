// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Types/SP_Types.h"
#include "SP_CombatComponent.generated.h"

class UMaterialInstanceDynamic;
struct FHitResult;
class ASP_Weapon;
class USP_WeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, bTargetingPlayer);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPAWNPOINT_API USP_CombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USP_CombatComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category = "SpawnPoint|Combat")
	static USP_CombatComponent* FindCombatComponent(const AActor* Actor) { return IsValid(Actor) ? Actor->FindComponentByClass<USP_CombatComponent>() : nullptr; }
	
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
	void InitializeWeaponWidgets();
	
	UPROPERTY(BlueprintAssignable)
	FReticleChanged OnReticleChanged;
	
	UPROPERTY(BlueprintAssignable)
	FAmmoCounterChanged OnAmmoCounterChanged;
	
	UPROPERTY(BlueprintAssignable)
	FRoundFired OnRoundFired;
	
	UPROPERTY(BlueprintAssignable)
	FAimingStatusChanged OnAimingStatusChanged;
	
	UPROPERTY(BlueprintAssignable)
	FTargetingPlayerStatusChanged OnTargetingPlayerStatusChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Weapon")
	float TraceLength { 20000.f };

private:
	bool bHitPlayerLastFrame { false };
	bool bFireTriggerPressed { false };
	FTimerHandle FireTimer;
	void FireTimerFinished();
	
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
	void Multicast_FireWeapon(const FHitResult& Hit, int32 AuthAmmo);
	
	void Local_Aim(bool bPressed);
	void Local_FireWeapon();
	
public:
	ASP_Weapon* GetCurrentWeapon() { return CurrentWeapon; };
};
