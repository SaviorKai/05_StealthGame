// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "FPSGameMode.generated.h"

UCLASS()
class AFPSGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:

	AFPSGameMode();

	void CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess);

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Spectating")
		TSubclassOf<AActor> SpectatingViewportClass;
};


///////// NETWORKING NOTES //////////////
/*

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// What Objects live where by Default? Client / Servers? ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Game Mode			= Only on Server / Host (Can't be replicated)
Player Controller	= On self and Server, per controller. Thus for two players, the server would have 2 controllers. However, players can only see / access their own.




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// If you want an AActor to show & move on all clients (Replicate) //////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

1. In the CONSTRUCTOR of the actor you want to replicate, add:

	SetReplicates(true);
	SetReplicateMovement(true);


2. Next, you need to ensure the code you use to spawn the actor, is run on the SERVER (not client). Client isn't able to instruct the server to to spawn the actor, as the server is authoritative. 
To do this, we need to use server functions. Go to the class where the spawning code lives, and create a server function, and move the spawn code into it:

In header file:
	UFUNCTION(Server, Reliable, WithValidation)
		void FunctionNameHere();

- 'Server' = Marks this as a server function.
- 'WithValidation' = Required with 'Server' Functions.
- 'Reliable' = This says that it will definitely (eventually) reach the server.

In cpp file:
	void class::FunctionNameHere_Implementation()
	{
		// Paste Spawn Code here //
	}

	bool class::FunctionNameHere_Validate()
	{
		return true;
	}

- Note that we have to use _Implementation() to define our function. 
- Also note, that we have to add a BOOL function for _Validate(), and return true (until you are ready to do some actual checking).

3. Remove the spawn code from where it was located, and call the new function we wrote instead. This will then ask the server to execute the code for us, and replicate the object to all clients. 
Note, we don't call with _Implementation(). 

4. Go through the code of the REPLICATED ACTOR, and find statements that need to be run by the server only, like 'Destroy()'. 
We don't want clients destroying actors for example, because they don't own th4e actor. To then ensure the code is only run on the server, use this if check:

if (Role == ROLE_Authority)
{
	// code here //. Example: Destroy();
}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// If you want to run code only on an actor or controller, that is owned by the current client ////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Remember that actors live on both the server, and the clients. Thus, if you use "ROLE_Authority", the code might run twice. 
We can make the code run only once on the owning client, by using:

*For Actors:
	if (IsLocallyControlled())
	{
		// code here // 
	}

- This can be used to run code either only for the client and owner of the object, OR, only on the server (if false)
- We can also complete this check on controllers:

*For Controllers:
	if (IsLocalController())
	{
		// code here //
	}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// If you want to run code only on the server	 /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

We can specify that we want a specific piece of code only to run, when the code is currently being run by the Server:

	if (Role == ROLE_Authority)
	{
		// code here //. Example: Destroy();
	}




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// If you want to replicate vars to all when changed ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

This could be useful for when you want to update a state of an NPC (like a guard), and ensure that the 
guard state is also updated on the client's version of the guard. We can replicate vars to all clients, by:

1. On the header, ensure we add the 'Replicated' tag to the UPROPERTY:

Basic:
	UPROPERTY(Replicated)
	float Life = 0;

OR:
UPROPERTY(Replicated, BlueprintReadOnly, Category = "Gameplay")
	float Life = 0;

- Note that it can be used with other tags.

2. Make sure you setup the replication rules. To do this,  you need to add:

*In the header file:

	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

*In the cpp file:

	#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps

	void ClassNameHere::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ClassNameHere, VarToReplicate);       // Example: DOREPLIFETIME(AIGuard, GuardState);
	}

- Remember to update the class names, and also the Var to replicate.
- Remember the #include.
- Note: We could have used 'DOREPLIFETIME_CONDITION(AAIGuard, GuardState, COND_OwnerOnly);		// COND_OwnerOnly adds the replication to the owner. This would optimize the code.




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// If you want to replicate vars to all CLIENTS ONLY when changed, with Function() //////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

We can have a function be called whenever the value is changed, but this function will only be called on CLIENTS. To do this:

1. Create the Replication with Function:

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	float GuardState = 0;

	UFUNCTION()
	void OnRep_GuardState();

- Note that you have to specify the function you want to run when the var is changed, 
- Remember to declare and define the new function, if it doesn't exist.

2. Make sure you setup the replication rules. To do this,  you need to add:

*In the header file:
	
	virtual void GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const override;

*In the cpp file:
	
	#include "Net/UnrealNetwork.h" // DOREPLIFETIME & GetLifetimeReplicatedProps

	void ClassNameHere::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
	{
		Super::GetLifetimeReplicatedProps(OutLifetimeProps);

		DOREPLIFETIME(ClassNameHere, VarToReplicate);       // Example: DOREPLIFETIME(AIGuard, GuardState);
	}

- Remember to update the class names, and also the Var to replicate.
- Remember the #include.

3. Remember that this code only runs on the client. Thus, if you are the server (P2P Networking), then you will need to ensure this code runs for yourself as well.
You can do this by calling the 'OnRep_GuardState()' function directly. 




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////// If you want to create a function that runs on all clients (WHEN called from server) //////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

This is useful, if for example you want to update the state of the game to all players.
You can create a function that runs on all clients(including server) when it's called by the SERVER. 


1. Define / declare the function with (NetMulticast, Reliable):

*In the header file: 

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFunctionNameHere(type* VarName);

*In the cpp file: 
	void Class::MulticastFunctionNameHere_Implementation(type* VarName)
	{
		// code here //   
	}
	

- Note that the definition uses "_Implementation" again. 
- Note that you can use this to pass vars.
- Note that 'IsLocallyControlled()' is very useful for functions like these.
- It seems like GameMode cannot use NetMulticast (because it cannot be replicated).



*/


