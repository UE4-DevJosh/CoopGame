// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SCharacter.generated.h"

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASCharacter();

protected:
	/*
	//
	// FUNCTIONS
	//
	*/

	virtual void BeginPlay() override;
	void MoveForward(float Value);
	void MoveRight(float Value);
	void BeginCrouch();
	void EndCrouch();
	void BeginADS();
	void EndADS();
	void StartFire();
	void StopFire();
	
	UFUNCTION()
	void OnHealthChanged(USHealthComponent* OwningHealthComponent, float Health, float DeltaHealth, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	/*
	//
	// COMPONENTS
	//
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class USHealthComponent* HealthComponent;

	/*
	//
	// WEAPON
	//
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<class ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName WeaponAttachSocketName;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	class ASWeapon* CurrentWeapon;

	/*
	//
	// VARIABLES
	//
	*/

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ADSFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float FOVInterpSpeed;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bWantsToAim;

	float DefaultFOV;

	//Pawn died recently.
	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;

public:	

	/*
	//
	// FUNCTIONS
	//
	*/

	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	/*
	//
	// ETC
	//
	*/

	virtual FVector GetPawnViewLocation() const override;

	class UCameraComponent* GetCameraComponent();

	class ASWeapon* GetCurrentWeapon();
};
