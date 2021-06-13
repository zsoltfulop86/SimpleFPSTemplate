// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FPSPlayerController.generated.h"

/**
 * The controller that owns the given Pawn we're playing with (route input etc. to a given mesh e.g.)
 * Exists on the Server and the controlling Client
 */
UCLASS()
class FPSGAME_API AFPSPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, Category="PlayerController")
	void OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);
};
