// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Actor.h"
#include "SP_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FHealthChanged, USP_HealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);


UENUM(BlueprintType)
enum class EDeathState : uint8 
{
	NotDead,
	DeathStarted,
	DeathFinished
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class SPAWNPOINT_API USP_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USP_HealthComponent();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION(BlueprintPure, Category = "SpawnPoint|Health")
	static USP_HealthComponent* FindHealthComponent(const AActor* Actor) { return IsValid(Actor) ? Actor->FindComponentByClass<USP_HealthComponent>() : nullptr; }
	
	UFUNCTION(BlueprintCallable)
	float GetHealthNormalized() const;
	
	/* Return true if lethal */
	bool ChangeHealthByAmount(float Amount, AActor* Instigator);
	void ChangeMaxHealthByAmount(float Amount, AActor* Instigator);
	
	UPROPERTY(BlueprintAssignable)
	FHealthChanged OnHealthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FHealthChanged OnMaxHealthChanged;
	
private:
	UPROPERTY(ReplicatedUsing = OnRep_Health)
	float Health { 0.f };
	
	UPROPERTY(ReplicatedUsing = OnRep_MaxHealth)
	float MaxHealth { 0.f };
	
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EDeathState DeathState { EDeathState::NotDead };
	
	UFUNCTION()
	void OnRep_DeathState(EDeathState OldDeathState);
	
	UFUNCTION()
	void OnRep_Health(float OldHealth);
	
	UFUNCTION()
	void OnRep_MaxHealth(float OldMaxHealth);
	
public:
	EDeathState GetDeathState() const { return DeathState; }
	float GetHealth() const { return Health; }
	float GetMaxHealth() const { return MaxHealth; }
};
