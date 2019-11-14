// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGuard.h"
#include "Perception/PawnSensingComponent.h" //UPawnSensingComponent
#include "DrawDebugHelpers.h" //DrawDebugSphere()
#include "TimerManager.h" //GetWorldTimerManager

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
	
}

// Called every frame
void AAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AAIGuard::HandleOnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn) { return; }																	//Pointer Protection
	
	//Debug Code
	UE_LOG(LogTemp, Warning, TEXT("I See you!"));
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Blue, false, 10.0f);		
}

void AAIGuard::HandleOnHearNoise(APawn* HeardPawn, const FVector& Location, float Volume)		// Changed 'Instigator' to 'HeardPawn'
{
	
	///Turn when we hear you
	auto Direction = Location - GetActorLocation();												//Using Vector Math, we subtract the target location, from my location, to get the direction vector.
	Direction.Normalize();																		//We normalize the vector, since we only want the direction rotation.
	FRotator LookAtTarget = Direction.Rotation();												//Why didn't he just use this? (Instead of FRotationMatrix)
	//FRotator LookAtTarget = FRotationMatrix::MakeFromX(Direction).Rotator();					//(Used my version instead). We use FRotationMatrix::MakeFromX().Rotator(); to turn it 
	SetActorRotation(FRotator(0.0f,LookAtTarget.Yaw,0.0f));										//We only want to use the Yaw rotation here, so he doesn't rotate up and down.

	
	//Reset to original rotation after some time
	//GetWorldTimerManager().ClearTimer(MyResetTimerHandle);									//Why did he use this? Doesn't seem like its needed, since the timer gets replaced.
	GetWorldTimerManager().SetTimer(MyResetTimerHandle, this, &AAIGuard::ResetOrientation, 3.0f,false);
																								
																								
																								//Debug Code
	UE_LOG(LogTemp, Warning, TEXT("I Hear you!"));
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Red, false, 10.0f);				//Instead of using the heard pawn (instigator), we just use the location the delegate returned :D)
}


void AAIGuard::ResetOrientation()
{
	
	//Turn back to the original position when timer is triggered.
	SetActorRotation(OriginalRotation);										
}