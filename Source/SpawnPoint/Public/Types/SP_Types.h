// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "SP_Types.generated.h"

UENUM(BlueprintType)
enum class ETurningInPlace : uint8
{
	Left UMETA(DisplayName = "TurningLeft"),
	Right UMETA(DisplayName = "TurningRight"),
	NotTurning UMETA(DisplayName = "NotTurning")
};

USTRUCT(BlueprintType)
struct FReticleParams
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ShapeCutFactor_RoundFired = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float ScaleFactor_RoundFired = 0.f;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	float RoundFiredInterpSpeed = 20.f;
};
