// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"

#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "FPSGameState.h"
#include "FPSPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

// No instance of a GameMode on clients, so everything inside runs on the Server
AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	// Set the GameState to be used
	GameStateClass = AFPSGameState::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	// Needs to run on all clients, so all controls are disabled -> impl in GameState
	if (InstigatorPawn)
	{
		// Moved below for multiplayer purposes:
		// APlayerController* PlayerController = Cast<APlayerController>(InstigatorPawn->GetController());

		// Disables player controller if nullptr or PlayerController -> moved to GameState for multiplayer
		// InstigatorPawn->DisableInput(PlayerController);

		if (SpectatingViewpointClass)
		{
			// Get the new viewport
			TArray<AActor*> ReturnedActors;
			// Give value to ReturnedActors with & as a reference
			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);

			// Change view target if any valid actor found
			if (ReturnedActors.Num() > 0)
			{
				// Get the first element
				AActor* NewViewTarget = ReturnedActors[0];

				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
				{
					AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(It->Get());

					// Don't check if local controller as it's called on the server
					if (PlayerController)
					{
						PlayerController->SetViewTargetWithBlend(NewViewTarget, 0.5f,
						                                         EViewTargetBlendFunction::VTBlend_Cubic);
					}
				}

				// Always get the reference (*) to the object placed in the game
				// Get the controller of the instigator and transfer its view from the instigator pawn to the new actor
				// Moved above for multiplayer replication:
				// PlayerController = Cast<APlayerController>(InstigatorPawn->GetController());
				// if (PlayerController)
				// {
				// 	PlayerController->SetViewTargetWithBlend(NewViewTarget, 0.5f,
				// 	                                         EViewTargetBlendFunction::VTBlend_Cubic);
				// }
			}
		}

		// Set Instigator to hidden so it won't be detected after game over (hides in game as well)
		InstigatorPawn->SetActorHiddenInGame(true);
		// Set Instigator to hidden so it won't be detected after game over (won't hide in game)
		// InstigatorPawn->SetHidden(true);
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "SpectatingViewpointClass is nullptr, Please update GameMode class with valid subclass. Cannot change spectating viewtarget."
		       ))
	}

	// Get GameState with a getter that casts to the correct type (generic)
	AFPSGameState* FPSGameState = GetGameState<AFPSGameState>();
	if (FPSGameState)
	{
		FPSGameState->Multicast_OnMissionCompleted(InstigatorPawn, bMissionSuccess);
	}

	// Needs to run on all clients to show the Mission Complete widget on all clients -> impl in GameState
	OnMissionCompleted(InstigatorPawn, bMissionSuccess);
}
