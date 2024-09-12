// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "Net/UnrealNetwork.h"
#include "FTaleGameState.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartRoundTimerDelegate, int32, RoundTime);

UCLASS()
class FTALETESTPROJECT_API AFTaleGameState : public AGameState
{
	GENERATED_BODY()

public:
	AFTaleGameState();

	UPROPERTY(ReplicatedUsing = OnRep_RoundLeftTime)
	int32 RoundLeftTime;

	UFUNCTION()
	void OnRep_RoundLeftTime();

	UPROPERTY(BlueprintAssignable)
	FStartRoundTimerDelegate StartRoundTimerDelegate;
	
};
