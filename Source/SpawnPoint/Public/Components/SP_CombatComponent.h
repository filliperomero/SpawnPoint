// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "Types/SP_Types.h"
#include "SP_CombatComponent.generated.h"

class UAnimMontage;
class UMaterialInstanceDynamic;
struct FHitResult;
class ASP_Weapon;
class USP_WeaponData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FReticleChanged, UMaterialInstanceDynamic*, ReticleDynMatInst, const FReticleParams&, ReticleParams, bool, bTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FAmmoCounterChanged, UMaterialInstanceDynamic*, AmmoCounterDynMatInst, int32, RoundsCurrent, int32, RoundsMax);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FRoundFired, int32, RoundsCurrent, int32, RoundsMax, int32, RoundsInReserve);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAimingStatusChanged, bool, bIsAiming);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FTargetingPlayerStatusChanged, bool, bTargetingPlayer);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FCurrentReserveAmmoChanged, int32, RoundsInReserve, int32, RoundsInWeapon, UMaterialInterface*, WeaponIconMaterial);

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
	
	void Notify_CycleWeapon();
	void Notify_ReloadWeapon();
	
	/* Cycle to the next weapon in the inventory */
	void InitiateCycleWeapon();
	void InitiateFireWeaponPressed();
	void InitiateFireWeaponReleased();
	void InitiateReloadWeapon();
	void InitiateAimPressed();
	void InitiateAimReleased();
	
	void AddAmmo(const FGameplayTag& WeaponType, int32 AmmoAmount);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnPoint|Weapon")
	TObjectPtr<USP_WeaponData> WeaponData;
	
	UPROPERTY(BlueprintReadOnly, Replicated)
	bool bAiming { false };
	
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing=OnRep_CurrentReserveAmmo)
	int32 CurrentReserveAmmo { 0 };
	
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
	
	UPROPERTY(BlueprintAssignable)
	FCurrentReserveAmmoChanged OnCurrentReserveAmmoChanged;
	
protected:
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|Weapon")
	float TraceLength { 20000.f };
	
	UFUNCTION()
	void BlendOut_CycleWeapon(UAnimMontage* Montage, bool bInterrupted);

private:
	TMap<FGameplayTag, int32> ReserveAmmo;
	bool bHitPlayerLastFrame { false };
	bool bHitPlayer { false };
	bool bFireTriggerPressed { false };
	FTimerHandle FireTimer;
	void FireTimerFinished();
	
	UPROPERTY(Transient, ReplicatedUsing = OnRep_CurrentWeapon, BlueprintReadOnly, meta=(AllowPrivateAccess="true"))
	TObjectPtr<ASP_Weapon> CurrentWeapon;
	
	UFUNCTION()
	void OnRep_CurrentWeapon(ASP_Weapon* PrevWeapon);
	
	UFUNCTION()
	void OnRep_CurrentReserveAmmo();
	
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
	
	UFUNCTION(Server, Reliable)
	void Server_CycleWeapon(int32 WeaponIndex);
	
	void Local_CycleWeapon(int32 WeaponIndex);
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_CycleWeapon(int32 WeaponIndex);
	
	int32 Local_WeaponIndex { 0 };
	
	int32 AdvanceWeaponIndex();
	
	void EquipWeapon(ASP_Weapon* Weapon);
	void SetCurrentWeapon(ASP_Weapon* NewWeapon, ASP_Weapon* LastWeapon);
	
	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(ASP_Weapon* Weapon);
	
	void Local_ReloadWeapon();
	
	UFUNCTION(Server, Reliable)
	void Server_ReloadWeapon();
	
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_ReloadWeapon();
	
	UFUNCTION(Client, Reliable)
	void Client_ReloadWeapon(int32 NewWeaponAmmo, int32 NewCarriedAmmo);
	
public:
	ASP_Weapon* GetCurrentWeapon() { return CurrentWeapon; }
	bool IsHittingPlayer() const { return bHitPlayer;}
};
