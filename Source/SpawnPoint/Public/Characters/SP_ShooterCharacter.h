// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SP_ShooterCharacter.generated.h"

class USP_CombatComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SPAWNPOINT_API ASP_ShooterCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASP_ShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USP_CombatComponent> CombatComponent;
	
	/* 1st Person View (arms) */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UCameraComponent> FirstPersonCamera;
};
