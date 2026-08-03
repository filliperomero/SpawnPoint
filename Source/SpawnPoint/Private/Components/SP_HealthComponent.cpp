// Copyright Fillipe Romero. All Rights Reserved.

#include "Components/SP_HealthComponent.h"

#include "Net/UnrealNetwork.h"

USP_HealthComponent::USP_HealthComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.bCanEverTick = false;
	
	SetIsReplicatedByDefault(true);
}

void USP_HealthComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USP_HealthComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(USP_HealthComponent, DeathState);
	DOREPLIFETIME_CONDITION(USP_HealthComponent, Health, COND_OwnerOnly);
	DOREPLIFETIME_CONDITION(USP_HealthComponent, MaxHealth, COND_OwnerOnly);
}

float USP_HealthComponent::GetHealthNormalized() const
{
	return MaxHealth > 0.f ? Health / MaxHealth : 0.f;
}

bool USP_HealthComponent::ChangeHealthByAmount(float Amount, AActor* Instigator)
{
	float OldValue = Health;
	Health = FMath::Clamp(Health + Amount, 0.f, MaxHealth);
	OnHealthChanged.Broadcast(this, OldValue, Health, Instigator);
	
	if (Health <= 0.f)
	{
		StartDeath();
	}
	
	return Health <= 0.f;
}

void USP_HealthComponent::StartDeath()
{
	if (DeathState != EDeathState::NotDead) return;
	
	DeathState = EDeathState::DeathStarted;
	OnDeathStarted.Broadcast();
	GetOwner()->ForceNetUpdate();
}

void USP_HealthComponent::ChangeMaxHealthByAmount(float Amount, AActor* Instigator)
{
	float OldValue = MaxHealth;
	MaxHealth += Amount;
	
	OnMaxHealthChanged.Broadcast(this, OldValue, MaxHealth, Instigator);
}

void USP_HealthComponent::OnRep_DeathState(EDeathState OldDeathState)
{
	if (DeathState == EDeathState::DeathStarted)
	{
		OnDeathStarted.Broadcast();
	}
}

void USP_HealthComponent::OnRep_Health(float OldHealth)
{
	OnHealthChanged.Broadcast(this, OldHealth, Health, nullptr);
}

void USP_HealthComponent::OnRep_MaxHealth(float OldMaxHealth)
{
	OnMaxHealthChanged.Broadcast(this, OldMaxHealth, MaxHealth, nullptr);
}
