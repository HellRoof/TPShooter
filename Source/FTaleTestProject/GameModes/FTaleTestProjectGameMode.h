// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "FTaleTestProjectGameMode.generated.h"

class AFTaleGameState;
class APlayerController;

UCLASS()
class FTALETESTPROJECT_API AFTaleTestProjectGameMode : public AGameMode
{
	GENERATED_BODY()

protected:

	AFTaleGameState* GS;

	TArray<AController*> ConnectedPlayes;

	UPROPERTY(EditAnywhere)
	int32 RoundTime;

	int32 RoundLeftTime;

	UPROPERTY(EditAnywhere)
	int32 AfterMatchRestartTime;
public:
	AFTaleTestProjectGameMode();

	virtual void StartMatch();

	/*void OnPostLogin(AController* NewPlayer) override;*/

	FTimerHandle RoundTimerHandle;

	void OnRoundTimerEnd();
	void ReduceRoundTimerTime();

	/*void NotifyRemainRoundTimeToPlayers();*/

	FTimerHandle AfterMatchRestartTimerHandle;
};
