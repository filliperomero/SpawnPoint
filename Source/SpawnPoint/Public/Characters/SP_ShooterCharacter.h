// Copyright Fillipe Romero. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/SP_PlayerInterface.h"
#include "Types/SP_Types.h"
#include "SP_ShooterCharacter.generated.h"

class UInputAction;
class USP_CombatComponent;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class SPAWNPOINT_API ASP_ShooterCharacter : public ACharacter, public ISP_PlayerInterface
{
	GENERATED_BODY()

public:
	ASP_ShooterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void PossessedBy(AController* NewController) override;
	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;
	
	UFUNCTION(BlueprintCallable)
	FRotator GetFixedAimRotation() const;
	
	UFUNCTION(BlueprintCallable)
	bool HasCurrentWeapon() const;
	
	UPROPERTY(BlueprintReadOnly, Category = "SpawnPoint|FABRIK")
	FTransform FABRIK_SocketTransform;
	
	/** PlayerInterface */
	virtual FName GetWeaponAttachPoint_Implementation(const FGameplayTag& WeaponType) const override;
	virtual USkeletalMeshComponent* GetMesh1P_Implementation() const override;
	virtual USkeletalMeshComponent* GetMesh3P_Implementation() const override;
	/** ~PlayerInterface */
	
protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnAim(bool bIsAiming);
	
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "SpawnPoint|Aiming")
	float DefaultFieldOfView { 90.f };

private:
	void InputCycleWeapon();
	void InputReloadWeapon();
	void InputFireWeaponPressed();
	void InputFireWeaponReleased();
	void InputAimPressed();
	void InputAimReleased();
	
	void CalculateFABRIKSocketTransform();
	void CalculateTurnInPlaceParameters(float DeltaTime);
	void TurnInPlace(float DeltaTime);
	
	FRotator StartingAimRotation;
	float InterpAO_Yaw;
	
	UPROPERTY(BlueprintReadOnly, Category = "SpawnPoint|TurnInPlace", meta=(AllowPrivateAccess="true"))
	float AO_Yaw;
	
	UPROPERTY(BlueprintReadOnly, Category = "SpawnPoint|Strafing", meta=(AllowPrivateAccess="true"))
	float MovementOffsetYaw;
	
	UPROPERTY(BlueprintReadOnly, Category = "SpawnPoint|TurnInPlace", meta=(AllowPrivateAccess="true"))
	ETurningInPlace TurningStatus { ETurningInPlace::NotTurning };
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoint|Combat", meta=(AllowPrivateAccess="true"))
	TObjectPtr<USP_CombatComponent> CombatComponent;
	
	/* 1st Person View (arms) */
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USkeletalMeshComponent> Mesh1P;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USpringArmComponent> SpringArm;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "SpawnPoint|Camera", meta=(AllowPrivateAccess="true"))
	TObjectPtr<UCameraComponent> FirstPersonCamera;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> CycleWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> FireWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> ReloadWeaponAction;
	
	UPROPERTY(EditAnywhere, Category = "SpawnPoint|Input")
	TObjectPtr<UInputAction> AimWeaponAction;
};
