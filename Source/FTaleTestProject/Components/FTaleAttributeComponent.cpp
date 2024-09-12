// Fill out your copyright notice in the Description page of Project Settings.


#include "FTaleAttributeComponent.h"

#include "Net/UnrealNetwork.h"
#include "FTaleTestProject/Characters/FTaleTestProjectCharacter.h"

UFTaleAttributeComponent::UFTaleAttributeComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFTaleAttributeComponent::BeginPlay()
{
	Super::BeginPlay();
	
	ComponentOwner = GetOwner<AFTaleTestProjectCharacter>();

	HealthCurrent = HealthMax;
	StaminaCurrent = StaminaMax;
}

void UFTaleAttributeComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFTaleAttributeComponent, HealthCurrent);

	DOREPLIFETIME(UFTaleAttributeComponent, StaminaCurrent);
}

void UFTaleAttributeComponent::ApplyDamage(float Damage)
{
	if (!IsValid(ComponentOwner))
	{
		return;
	}

	if (ComponentOwner->HasAuthority())
	{
		if (HealthCurrent < Damage)
		{
			HealthCurrent = 0.f;
		}
		else
		{
			HealthCurrent -= Damage;
		}

		OnRep_Health();

		if (FMath::IsNearlyZero(HealthCurrent) || HealthCurrent < 0.f)
		{
			ComponentOwner->Death();
		}
	}
}

void UFTaleAttributeComponent::OnRep_Health()
{
	HealthUpdaterDelegate.Broadcast(HealthCurrent);
}

void UFTaleAttributeComponent::OnRep_Stamina()
{
}

void UFTaleAttributeComponent::RestoreAttribute()
{
	HealthCurrent = HealthMax;
	//TO DO
	//StaminaCurrent = StaminaMax;

	if (IsValid(ComponentOwner))
	{
		if (ComponentOwner->GetLocalRole() == ROLE_Authority)
		{
			OnRep_Health();
		}
	}
}

void UFTaleAttributeComponent::Server_ApplyDamage_Implementation(float Damage)
{
	ApplyDamage(Damage);
}

