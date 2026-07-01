// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameFramework/Actor.h"
#include "SP_Weapon.generated.h"

UCLASS()
class SPAWNPOINT_API ASP_Weapon : public AActor
{
	GENERATED_BODY()

public:
	ASP_Weapon();
	virtual void OnRep_Instigator() override;
	
	void AttachToOwningPawn() const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "SpawnPoint|Aiming")
	float AimFieldOfView { 65.0f };
	
protected:
	virtual void BeginPlay() override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "SpawnPoint|WeaponType")
	FGameplayTag WeaponType;

private:
	/** Weapon Mesh: 1st Person View */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	/** Weapon Mesh: 3rd Person View */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
	void SetMeshVisibilities(APawn* OwningPawn) const;
	
public:
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
	FGameplayTag GetWeaponType() const { return WeaponType; }
};
