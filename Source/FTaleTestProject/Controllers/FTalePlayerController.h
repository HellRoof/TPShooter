// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "FTalePlayerController.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStartRoundTimerDelegate, int32, RoundTime);

class UUserWidget;

UCLASS()
class FTALETESTPROJECT_API AFTalePlayerController : public APlayerController
{
	GENERATED_BODY()

protected:

public:
	AFTalePlayerController();

	void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite)
	UUserWidget* UserWidgetInGameMainConstructor;

	//UPROPERTY(BlueprintAssignable)
	//FStartRoundTimerDelegate StartRoundTimerDelegate;

	//UFUNCTION()
	//void StartRoundTimer(int32 RoundTime);
};
