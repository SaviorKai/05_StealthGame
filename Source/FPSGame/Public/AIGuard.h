// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIGuard.generated.h"

class UPawnSensingComponent;

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

	UPROPERTY(VisibleAnywhere, Category="Components")
		UPawnSensingComponent* PawnSensingComp;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Create the function that will handle the pawn seen delegate, and use the signature of "OnSeePawn()" from the UE4 engine.
	UFUNCTION() //ALWAYS HAS TO BE HERE FOR DELEGATE BINDING!
		void HandleOnPawnSeen(APawn* SeenPawn); 

};
