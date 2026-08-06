// Copyright Fillipe Romero. All Rights Reserved.

#include "Components/SP_EliminationComponent.h"

USP_EliminationComponent::USP_EliminationComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
}

void USP_EliminationComponent::OnRoundReported(AActor* Attacker, AActor* Victim, bool bHit, bool bHeadShot, bool bLethal)
{
	// TOOD
}
