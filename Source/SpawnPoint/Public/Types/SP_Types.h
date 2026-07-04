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