// Fill out your copyright notice in the Description page of Project Settings.


#include "AIGuard.h"
#include "Perception/PawnSensingComponent.h" //UPawnSensingComponent
#include "DrawDebugHelpers.h" //DrawDebugSphere()

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
	
}

// Called every frame
void AAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AAIGuard::HandleOnPawnSeen(APawn* SeenPawn)
{
	if (!SeenPawn) { return; }																				//Pointer Protection
	
	//Debug Code
	UE_LOG(LogTemp, Warning, TEXT("I See you!"));
	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 32.0f, 12, FColor::Blue, false, 10.0f);		
}

void AAIGuard::HandleOnHearNoise(APawn* HeardPawn, const FVector& Location, float Volume)		// Changed 'Instigator' to 'HeardPawn'
{
	//Debug Code
	UE_LOG(LogTemp, Warning, TEXT("I Hear you!"));
	DrawDebugSphere(GetWorld(), Location, 32.0f, 12, FColor::Red, false, 10.0f);				//Instead of using the heard pawn (instigator), we just use the location the delegate returned :D)
}
