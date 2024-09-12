// Copyright Epic Games, Inc. All Rights Reserved.

#include "FTaleTestProjectGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FTaleTestProject/Controllers/FTalePlayerController.h"
#include "FTaleTestProject/GameStates/FTaleGameState.h"

AFTaleTestProjectGameMode::AFTaleTestProjectGameMode()
{
}

void AFTaleTestProjectGameMode::StartMatch()
{
	Super::StartMatch();

	RoundLeftTime = RoundTime;

	GS = Cast<AFTaleGameState>(GameState.Get());

	if (IsValid(GS))
	{
		GS->RoundLeftTime = RoundTime;
	}

	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(RoundTimerHandle, this, &AFTaleTestProjectGameMode::ReduceRoundTimerTime, 1.f, true);
	}
}	

//void AFTaleTestProjectGameMode::OnPostLogin(AController* NewPlayer)
//{
//	if (IsValid(NewPlayer))
//	{
//		ConnectedPlayes.AddUnique(NewPlayer);
//	}
//}

void AFTaleTestProjectGameMode::OnRoundTimerEnd()
{
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(AfterMatchRestartTimerHandle, this, &AGameMode::RestartGame, AfterMatchRestartTime, false);
	}
}

void AFTaleTestProjectGameMode::ReduceRoundTimerTime()
{
	if (RoundLeftTime <= 0)
	{
		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().ClearTimer(RoundTimerHandle);
		}

		OnRoundTimerEnd();
	}
	else
	{
		RoundLeftTime--;

		if (GS)
		{
			GS->RoundLeftTime = RoundLeftTime;
			GS->OnRep_RoundLeftTime();
		}
		//NotifyRemainRoundTimeToPlayers();
	}
}

//void AFTaleTestProjectGameMode::NotifyRemainRoundTimeToPlayers()
//{
//	if (ConnectedPlayes.Num() == 0)
//	{
//		return;
//	}
//
//	for (AController* Iter : ConnectedPlayes)
//	{
//		if (AFTalePlayerController* PC = Cast<AFTalePlayerController>(Iter))
//		{
//			PC->StartRoundTimerDelegate.Broadcast(RoundLeftTime);
//		}
//	}
//}
