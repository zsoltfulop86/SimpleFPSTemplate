// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ConstructorHelpers.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();
}

void AFPSGameMode::CompleteMission(APawn* InstigatorPawn)
{
	if (InstigatorPawn)
	{
		// Disables player controller
		InstigatorPawn->DisableInput(nullptr);

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

				// Always get the reference (*) to the object placed in the game
				// Get the controller of the instigator and transfer its view from the instigator pawn to the new actor
				APlayerController* PlayerController = Cast<APlayerController>(InstigatorPawn->GetController());
				if (PlayerController)
				{
					PlayerController->SetViewTargetWithBlend(NewViewTarget, 0.5f,
					                                         EViewTargetBlendFunction::VTBlend_Cubic);
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "SpectatingViewpointClass is nullptr, Please update GameMode class with valid subclass. Cannot change spectating viewtarget."
		       ))
	}

	OnMissionCompleted(InstigatorPawn);
}
