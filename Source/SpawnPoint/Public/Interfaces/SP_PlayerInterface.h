// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SP_PlayerInterface.generated.h"

struct FGameplayTag;

UINTERFACE()
class USP_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPAWNPOINT_API ISP_PlayerInterface
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FName GetWeaponAttachPoint(const FGameplayTag& WeaponType) const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetMesh1P() const;
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	USkeletalMeshComponent* GetMesh3P() const;
};
