// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTaleAttributeComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHealthUpdaterDelegate, float, NewHealth);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStaminaUpdaterDelegate, float, NewStamina);

class AFTaleTestProjectCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FTALETESTPROJECT_API UFTaleAttributeComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UFTaleAttributeComponent();

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	AFTaleTestProjectCharacter* ComponentOwner;

public:

	//Health
	UFUNCTION(Server, reliable)
	void Server_ApplyDamage(float Damage);

	UFUNCTION(BlueprintCallable)
	void ApplyDamage(float Damage);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health)
	float HealthCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float HealthMax;

	UFUNCTION()
	void OnRep_Health();

	bool bIsAlive;

	UPROPERTY(BlueprintAssignable)
	FHealthUpdaterDelegate HealthUpdaterDelegate;


	//Stamina
	//TODO
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stamina)
	float StaminaCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float StaminaMax;

	UFUNCTION()
	virtual void OnRep_Stamina();

	UPROPERTY(BlueprintAssignable)
	FStaminaUpdaterDelegate StaminaUpdaterDelegate;

	void RestoreAttribute();
};
