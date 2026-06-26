// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SP_Weapon.generated.h"

UCLASS()
class SPAWNPOINT_API ASP_Weapon : public AActor
{
	GENERATED_BODY()

public:
	ASP_Weapon();
	
protected:
	virtual void BeginPlay() override;

private:
	/** Weapon Mesh: 1st Person View */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	/** Weapon Mesh: 3rd Person View */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh3P;
	
public:
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	USkeletalMeshComponent* GetMesh3P() const { return Mesh3P; }
};
