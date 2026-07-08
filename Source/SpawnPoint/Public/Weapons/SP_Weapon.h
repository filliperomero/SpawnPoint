// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SP_Weapon.generated.h"

enum EPhysicalSurface : int;

UENUM(BlueprintType)
enum class EFireType : uint8
{
	Auto UMETA(DisplayName = "Automatic"),
	SemiAuto UMETA(DisplayName = "SemiAutomatic"),
};

UCLASS()
class SPAWNPOINT_API ASP_Weapon : public AActor
{
	GENERATED_BODY()

public:
	ASP_Weapon();
	virtual void OnRep_Instigator() override;
	
	void AttachToOwningPawn() const;
	void WeaponTrace(FHitResult& OutHitResult, float TraceLength);
	void Local_Fire(const FVector& ImpactPoint, const FVector& ImpactNormal, TEnumAsByte<EPhysicalSurface> ImpactSurfaceType, bool bIsFirstPerson);
	void Auth_Fire();
	void Rep_Fire(int32 AuthAmmo);
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnPoint|Aiming")
	float AimFieldOfView { 65.f };
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnPoint|Aiming")
	float TraceRadius { 5.f };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnPoint|FireType")
	EFireType FireType { EFireType::SemiAuto };
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnPoint|FireType")
	float FireRate { 0.1f };
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Ammo")
	int32 Ammo { 5 };
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Ammo")
	int32 MagCapacity { 10 };
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Ammo")
	int32 StartingCarriedAmmo { 10 };
	
protected:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintImplementableEvent)
	void FireEffects(const FVector& ImpactPoint, const FVector& ImpactNormal, EPhysicalSurface ImpactSurfaceType, bool bIsFirstPerson);
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnPoint|WeaponType")
	FGameplayTag WeaponType;
	
	/** Weapon Mesh: 1st Person View */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoint|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	/** Weapon Mesh: 3rd Person View */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoint|Weapon")
	TObjectPtr<USkeletalMeshComponent> Mesh3P;

private:
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
	int32 Sequence { 0 };
	
public:
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	FGameplayTag GetWeaponType() const { return WeaponType; }
};
