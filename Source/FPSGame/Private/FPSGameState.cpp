// Fill out your copyright notice in the Description page of Project Settings.


#include "FPSGameState.h"

#include "EngineUtils.h"
#include "FPSPlayerController.h"

void AFPSGameState::Multicast_OnMissionCompleted_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	// Call OnMissionCompleted() on the local player controller and iterate the locally controlled pawns and
	//  disable their input
	for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; It++)
	{
		AFPSPlayerController* PlayerController = Cast<AFPSPlayerController>(It->Get());
		if (PlayerController && PlayerController->IsLocalController())
		{
			PlayerController->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			APawn* CurrentPawn = PlayerController->GetPawn();
			if (CurrentPawn && CurrentPawn->IsLocallyControlled())
			{
				// Disable if controlled
				CurrentPawn->DisableInput(nullptr);
			}
		}
	}
}
