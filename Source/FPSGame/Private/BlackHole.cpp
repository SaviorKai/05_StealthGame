// Fill out your copyright notice in the Description page of Project Settings.


#include "BlackHole.h"
#include "Components/SphereComponent.h" // for USphereComponent
#include "Components/StaticMeshComponent.h" // for UStaticMeshComponent
#include "Components/PrimitiveComponent.h"  // for UPrimitiveComponent

// Sets default values
ABlackHole::ABlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	SetRootComponent(MeshComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MeshComp->SetCollisionResponseToAllChannels(ECR_Ignore);								// Sets all the channels to Ignore (just to brush over them all)
	
	//InnerSphere - Destroys actors on hit
	InnerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("InnerSphereComp"));
	InnerSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InnerSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);							// Sets all the channels to Ignore (just to brush over them all)
	InnerSphereComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);			// Changes only PhysicsBody to Overlap.
	InnerSphereComp->SetSphereRadius(100);
	InnerSphereComp->SetupAttachment(MeshComp);
	


	//Pull Radius
	PullRadiusSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PullRadiusSphere"));
	PullRadiusSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);							
	PullRadiusSphere->SetCollisionResponseToAllChannels(ECR_Ignore);								// Sets all the channels to Ignore (just to brush over them all)
	PullRadiusSphere->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);				// Changes only PhysicsBody to Overlap.
	PullRadiusSphere->SetSphereRadius(5000);														// Default Value
	PullRadiusSphere->SetupAttachment(MeshComp);

	//Setup OnComponent Hit						
	InnerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHole::OnOverlapBegin);		// set up a notification for when this component hits something. NOTE: Won't work with exact function name "OnOverlapBegin"
	
}

// Called when the game starts or when spawned
void ABlackHole::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TArray<AActor*> OverlappingActors;														//Out Parameter Array

	PullRadiusSphere->GetOverlappingActors(OverlappingActors);								//Out Parameter. Note: You can set the additional argument here, if you only want certain actors to be returned.
	//PullRadiusSphere->GetOverlappingComponents(); NOTE: Overlapping components could have been used too.

	for (auto item : OverlappingActors)
	{
		if (!item) { return; }																//Pointer Protection since we're deleting items

		auto* ConvertedItem = Cast<UPrimitiveComponent>(item->GetRootComponent());			//Convert the item root component to Primitive component so we can call the force functions
		
		ConvertedItem->AddRadialForce(														//Adds a force to all bodies in THIS COMPONENT only.
			PullRadiusSphere->GetComponentLocation(),
			PullRadiusSphere->GetScaledSphereRadius(),
			-2000.0f,																			
			ERadialImpulseFalloff::RIF_Constant,											//RIF_Linear = Like orbital gravity. Constant = consistent at all ranges.
			true																			//IVAN NOTE: Blocks don't move without this as true.
		);
		
	}

}

void ABlackHole::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	OtherActor->Destroy();
}