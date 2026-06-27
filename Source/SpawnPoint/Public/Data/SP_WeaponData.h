// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "SP_WeaponData.generated.h"

/**
 * 
 */
UCLASS()
class SPAWNPOINT_API USP_WeaponData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category = "SpawnPoint|WeaponData|Weapons")
	TMap<FGameplayTag, FName> GripPoints;
};
