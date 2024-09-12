// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FTaleInventoryComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FClipAmmoUpdaterDelegate, int32, NewClipAmmo);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBagAmmoUpdaterDelegate, int32, NewBagAmmo);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FNewWeaponEquippedDelegate);

class UInputAction;
class AFTaleWeaponBase;
class AFTaleTestProjectCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class FTALETESTPROJECT_API UFTaleInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UFTaleInventoryComponent();

protected:

	virtual void BeginPlay() override;

	UPROPERTY()
	AFTaleTestProjectCharacter* ComponentOwner;

public:	

	UPROPERTY(ReplicatedUsing = OnRep_Inventory)
	TArray<AFTaleWeaponBase*> Inventory;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<AFTaleWeaponBase>> StartupWeapons;

	UPROPERTY(ReplicatedUsing = OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex;

	UFUNCTION(BlueprintCallable)
	AFTaleWeaponBase* GetWeaponByActiveSlot();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

	UFUNCTION()
	void OnRep_Inventory();

	UFUNCTION(BlueprintCallable)
	float GetWeaponDamage();

	void FillInventory();

	UPROPERTY(BlueprintAssignable)
	FNewWeaponEquippedDelegate NewWeaponEquippedDelegate;

	//Slot selection
	UFUNCTION(Server, Reliable)
	void Server_SetActiveSlot1();
	void SetActiveSlot1();



	UFUNCTION(Server, Reliable)
	void Server_SetActiveSlot2();
	void SetActiveSlot2();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnOwnerDeath();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_OnOwnerRespawn();

	void RestoreAmmo();

	UPROPERTY(BlueprintAssignable)
	FClipAmmoUpdaterDelegate ClipAmmoUpdaterDelegate;

	UPROPERTY(BlueprintAssignable)
	FBagAmmoUpdaterDelegate BagAmmoUpdaterDelegate;

protected:
	void SetActiveSlot(int32 SlotInex);

	void UpdateUIAmmo();

};
