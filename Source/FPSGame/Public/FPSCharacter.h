// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSCharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class UCameraComponent;
class AFPSProjectile;
class USoundBase;
class UAnimSequence;
class UPawnNoiseEmitterComponent;


UCLASS()
class AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

protected:

	/** Pawn mesh: 1st person view  */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Mesh")
	USkeletalMeshComponent* Mesh1PComponent;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Mesh")
	USkeletalMeshComponent* GunMeshComponent;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera")
	UCameraComponent* CameraComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	UPawnNoiseEmitterComponent* NoiseEmitterComp;


public:
	AFPSCharacter();

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category="Projectile")
	TSubclassOf<AFPSProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category="Gameplay")
	USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay")
	UAnimSequence* FireAnimation;

	UPROPERTY(Replicated, BlueprintReadOnly, Category="Gameplay")												/// [NETWORKING (1/2)] : Replicated added.
		bool bIsCarryingObjective = false;

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;		/// [NETWORKING (2/2)] : Added the rules for replication

protected:
	
	/// Fires a projectile. ///
	void Fire();

	/// Handles moving forward/backward ///
	void MoveForward(float Val);

	/// Handles strafing movement, left and right ///
	void MoveRight(float Val);

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/// [NETWORKING] ///
	UFUNCTION(Server, Reliable, WithValidation)				//Mark this as a Server function, that it's reliable (will reach the server eventually), WithValidation is used for sanity checking by the client (Cheating, Sync issues, etc). 
		void ServerFire();									//used in the Fire() function for projectile spawning. 



public:
	/** Returns Mesh1P subobject **/
	USkeletalMeshComponent* GetMesh1P() const { return Mesh1PComponent; }

	/** Returns FirstPersonCameraComponent subobject **/
	UCameraComponent* GetFirstPersonCameraComponent() const { return CameraComponent; }

	virtual void Tick(float DetlaTime) override;

	UPROPERTY(BlueprintReadOnly, Category = "Gameplay")
		bool bIsGameOver = false;

};

