// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIGuard.generated.h"

class UPawnSensingComponent;

// Enum Class for saving the state of the Guard. Always remember to add UENUM(). 
UENUM(BlueprintType)
enum class EAIState : uint8
{
	Idle, 
	Suspicious,
	Alerted
};

UCLASS()
class FPSGAME_API AAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UFUNCTION() //ALWAYS HAS TO BE HERE FOR DELEGATE BINDING!
		void ResetOrientation();

	UPROPERTY(VisibleAnywhere, Category = "Components")
		UPawnSensingComponent* PawnSensingComp;

	FRotator OriginalRotation;
	FTimerHandle MyResetTimerHandle;

	//Set Default Guard State from the Enum we created.
	UPROPERTY(ReplicatedUsing=OnRep_GuardState)																	/// [NETWORKING (1/3)] This calls the bound function on CLIENTS only, when the var is changed.
		EAIState GuardState = EAIState::Suspicious;																//Updated in Begin Play(). I want the state to swap at start, because the change state code it set to not run if the state is the same.

	UFUNCTION()  
		void OnRep_GuardState();																				/// [NETWORKING (2/3)] - This function was bound above, and is called by CLIENTS only, when the var is changed. We can call this manually if we like.

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;		/// [NETWORKING (3/3)] ///

	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")									
		void OnStateChanged(EAIState NewState);																	//Blueprint Event (Doesn't need to be defined!)

	/// AI Patrolling ///
	
	UPROPERTY(EditInstanceOnly, Category = "AI")
		bool bCanPatrol;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition="bCanPatrol"))				//NOTE:  the meta setting will enable or disable the ability to change this in the editor if bCanPatrol is turned off. 
		AActor* PatrolPointA;

	UPROPERTY(EditInstanceOnly, Category = "AI", meta = (EditCondition = "bCanPatrol"))				//NOTE:  the meta setting will enable or disable the ability to change this in the editor if bCanPatrol is turned off. 
		AActor* PatrolPointB;

	AActor* CurrentPatrolPoint;

	void MoveToNextPatrolPoint();
	


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Create the function that will handle the pawn seen delegate, and use the signature of "OnSeePawn()" from the UE4 engine.
	UFUNCTION() //ALWAYS HAS TO BE HERE FOR DELEGATE BINDING!
		void HandleOnPawnSeen(APawn* SeenPawn); 

	UFUNCTION() //ALWAYS HAS TO BE HERE FOR DELEGATE BINDING!
		void HandleOnHearNoise(APawn* HeardPawn, const FVector& Location, float Volume);				// Changed 'Instigator' to 'HeardPawn'

};
