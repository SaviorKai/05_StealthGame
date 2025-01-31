// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGuard.h"
#include "Perception/PawnSensingComponent.h" //UPawnSensingComponent
#include "DrawDebugHelpers.h" //DrawDebugSphere()
#include "TimerManager.h" //GetWorldTimerManager
#include "FPSGameMode.h" // AFPSGameMode
#include "Blueprint/AIBlueprintHelperLibrary.h" // UAIBlueprintHelperLibrary
#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps



// Sets default values
AAIGuard::AAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComponent"));   ///Note: this doesn't need to be attached to anything.

	//Bind the 'OnSeenPawn()' event to our 'HandleOnPawnSeen()' function.
	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIGuard::HandleOnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AAIGuard::HandleOnHearNoise);
	

}

// Called when the game starts or when spawned
void AAIGuard::BeginPlay()
{
	Super::BeginPlay();

	OriginalRotation = GetActorRotation();
	
	SetGuardState(EAIState::Idle);																//We use the function to set the state here, since we want to have the UI change in blueprints.

	/// Patrol if Turned on [AI Movement] ///
	if (bCanPatrol)
	{
		MoveToNextPatrolPoint();
	}
}

// Called every frame
void AAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	/// Change Patrol Point to Next one, if we're close enough ///
	if (CurrentPatrolPoint)
	{
		FVector MyDelta = GetActorLocation() - CurrentPatrolPoint->GetActorLocation();			//Get the Vector Difference
		float DistanceGoal = MyDelta.Size();													//Turn it into a single float with Size() which give us the total magnitude of the Vector. We can use this for Distance.

		if (DistanceGoal < 100)
		{
			MoveToNextPatrolPoint();
		}
	}

	// OLD CODE - Ivan Original //
	/*
	//Check if the state is idle
	if (GuardState == EAIState::Idle)
	{
		//Navigate to PatrolPointA
		if (!PatrolPointA) { return; }														//Pointer Protection
		
		auto MyDirection = PatrolPointA->GetActorLocation() - GetActorLocation();			//Vector Math to get the correct rotation
		MyDirection.Normalize();															//Normalize (since we only want rotation)
		FRotator LookAtTarget = MyDirection.Rotation();										//Convert to FRotator					
		SetActorRotation(FRotator(0.0f, LookAtTarget.Yaw, 0.0f));							//Rotate actor on Yaw only.

		//Rotate to the correct next step Direction
		

		

		//Move forward direction
		Cast<APawn>(this)->AddMovementInput(GetActorForwardVector(), 1.0f);
	}
	*/
}

void AAIGuard::HandleOnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn) { return; }																	//Pointer Protection
	
	/// Turn to the Seen Pawn, and end game ///
	AFPSGameMode* OurGameMode = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());				//Get game mode and cast to our type.	//Note: GetAuthGameMode() does not work in Multiplayer.(More about this later)
	OurGameMode->CompleteMission(SeenPawn, false);

	/// Change the state of the guard accordingly ///
	SetGuardState(EAIState::Alerted);

	/// If Patrolling, Stop Movement [AI Movement] ///
	if (GetController())
	{
		GetController()->StopMovement();
	}

	/// Debug Code ///
	//UE_LOG(LogTemp, Warning, TEXT("I See you!"));
	//DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Blue, false, 10.0f);		
}

void AAIGuard::HandleOnHearNoise(APawn* HeardPawn, const FVector& Location, float Volume)		// Changed 'Instigator' to 'HeardPawn'
{
	if (GuardState == EAIState::Alerted) { return; }											//If the guard is already Alerted, we don't care about noises.
	
	/// Turn when we hear you ///
	auto Direction = Location - GetActorLocation();												//Using Vector Math, we subtract the target location, from my location, to get the direction vector.
	Direction.Normalize();																		//We normalize the vector, since we only want the direction rotation.
	FRotator LookAtTarget = Direction.Rotation();												//Why didn't he just use this? (Instead of FRotationMatrix)
	//FRotator LookAtTarget = FRotationMatrix::MakeFromX(Direction).Rotator();					//(Used my version instead). We use FRotationMatrix::MakeFromX().Rotator(); to turn it 
	SetActorRotation(FRotator(0.0f,LookAtTarget.Yaw,0.0f));										//We only want to use the Yaw rotation here, so he doesn't rotate up and down.
	
	/// Change the state of the guard accordingly ///
	SetGuardState(EAIState::Suspicious);
	
	/// Reset to original rotation after some time ///
	//GetWorldTimerManager().ClearTimer(MyResetTimerHandle);									//Why did he use this? Doesn't seem like its needed, since the timer gets replaced.
	GetWorldTimerManager().SetTimer(MyResetTimerHandle, this, &AAIGuard::ResetOrientation, 3.0f,false);
	
	/// If Patrolling, Stop Movement [AI Movement] ///
	if (GetController())
	{
		GetController()->StopMovement();
	}

	/// Debug Code ///
	//UE_LOG(LogTemp, Warning, TEXT("I Hear you!"));
	//DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Red, false, 10.0f);				//Instead of using the heard pawn (instigator), we just use the location the delegate returned :D)
}

void AAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted) { return; }											//If the guard is already alerted, we skip this (avoid resetting the game)
	
	/// Turn back to the original position when timer is triggered. ///
	SetActorRotation(OriginalRotation);	
	
	/// Change the state of the guard accordingly ///
	SetGuardState(EAIState::Idle);

	///	Continue Patrol if Turned on [AI Movement] ///
	if (bCanPatrol)
	{
		MoveToNextPatrolPoint();
	}
}



void AAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState == NewState) { return; }														//If the state is already correctly set, do nothing.

	GuardState = NewState;																		//else, update the state.

	OnRep_GuardState();																			/// The funtion only runs on clients, but we also want it to run on the server. Thus, we manually call it here. We don't call OnStateChanged() directly, because it will run on both and possibly cause missmatch.
}


void AAIGuard::MoveToNextPatrolPoint()
{
	/// Update the Control Points ///
	if (CurrentPatrolPoint == nullptr || CurrentPatrolPoint == PatrolPointB)					//If current point = empty, OR current = Point B
	{
		CurrentPatrolPoint = PatrolPointA;
	}
	else
	{
		CurrentPatrolPoint = PatrolPointB;
	}

	/// Move the Character to the current ///
	// * This will handle the movement, as long as our character has a movement component, controller and Mesh.
	// * This actor has a movement component, since it was derived from Character.
	// * Remember that the AIcontroller is automatically created. 
	UAIBlueprintHelperLibrary::SimpleMoveToActor(GetController(), CurrentPatrolPoint);


}


/// [NETWORKING] ///
void AAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);
}

/// [NETWORKING] ///
void AAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AAIGuard, GuardState);
}