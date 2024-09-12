// Fill out your copyright notice in the Description page of Project Settings.


#include "FTaleGameState.h"
#include "FTaleGameState.h"

AFTaleGameState::AFTaleGameState()
{
	SetReplicates(true);
}

void AFTaleGameState::OnRep_RoundLeftTime()
{
	StartRoundTimerDelegate.Broadcast(RoundLeftTime);
}

void AFTaleGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFTaleGameState, RoundLeftTime);
}