// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * Can be used as a container to replicate anything from GameMode
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	/** Multicast the OnMissionCompleted function to all clients -> a GameMode event running only on the server */
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnMissionCompleted(APawn* InstigatorPawn, bool bMissionSuccess);
};
