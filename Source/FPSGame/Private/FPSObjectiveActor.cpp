// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h" // for USphereComponent
#include "Components/StaticMeshComponent.h" // for UStaticMeshComponent
#include "Kismet/GameplayStatics.h" // for UGameplayStatics
#include "FPSCharacter.h" //for AFPSCharacter;

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);							// 
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);								// Sets all the channels to Ignore (just to brush over them all)
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);						// Changes only Pawn to Overlap.
	SphereComp->SetupAttachment(MeshComp);

	

}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	

}

// Called every frame
void AFPSObjectiveActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	SpawnEffects();

	AFPSCharacter* OverlappingCharacter = Cast<AFPSCharacter>(OtherActor);			//We're casting here to AFPSCharacter, so that we can access the vars that it will have (bIsCarryingObjective). If the cast fails, the code won't run, due to the pointer protection below. 

	if (OverlappingCharacter)														//Pointer Protection
	{ 
		OverlappingCharacter->bIsCarryingObjective = true;

		Destroy();																	// Destroy this actor, since we don't want anyone else to pick it up.
	}
	
}



void AFPSObjectiveActor::SpawnEffects()
{
	if (!PickupFX) { return; }														//Pointer Protection
	
	UGameplayStatics::SpawnEmitterAtLocation(
		this,
		PickupFX,
		GetActorLocation()
	);
}

