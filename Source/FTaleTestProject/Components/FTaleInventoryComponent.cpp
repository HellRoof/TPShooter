// Fill out your copyright notice in the Description page of Project Settings.


#include "FTaleInventoryComponent.h"

#include "FTaleTestProject/Characters/FTaleTestProjectCharacter.h"
#include "FTaleTestProject/Weapons/FTaleWeaponBase.h"
#include "Net/UnrealNetwork.h"

UFTaleInventoryComponent::UFTaleInventoryComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	
	ActiveSlotIndex = -1;
	
}

void UFTaleInventoryComponent::BeginPlay()
{
	Super::BeginPlay();	

	ComponentOwner = GetOwner<AFTaleTestProjectCharacter>();

	SetIsReplicated(true);

	FillInventory();

	if (IsValid(ComponentOwner))
	{
		if (ComponentOwner->GetLocalRole() == ROLE_Authority)
		{
			OnRep_Inventory();
		}
	}
}

void UFTaleInventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UFTaleInventoryComponent, ActiveSlotIndex);

	DOREPLIFETIME(UFTaleInventoryComponent, Inventory);
}

void UFTaleInventoryComponent::OnRep_ActiveSlotIndex()
{
	SetActiveSlot(ActiveSlotIndex);

	if (IsValid(GetWeaponByActiveSlot()))
	{
		ClipAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->ClipSizeCurrent);
		BagAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->AmmoInBagCurrent);
		NewWeaponEquippedDelegate.Broadcast();
	}
}

void UFTaleInventoryComponent::OnRep_Inventory()
{
	SetActiveSlot1();

	if (IsValid(GetWeaponByActiveSlot()))
	{
		ClipAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->ClipSizeMax);
		BagAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->AmmoInBagMax);


		if (!IsValid(ComponentOwner))
		{
			return;
		}

		if (ComponentOwner->GetLocalRole() == ROLE_AutonomousProxy)
		{
			NewWeaponEquippedDelegate.Broadcast();
		}
	}
}

AFTaleWeaponBase* UFTaleInventoryComponent::GetWeaponByActiveSlot()
{
	if (Inventory.IsValidIndex(ActiveSlotIndex))
	{
		return Inventory[ActiveSlotIndex];
	}

	return nullptr;
}

float UFTaleInventoryComponent::GetWeaponDamage()
{
	AFTaleWeaponBase* CurrentWeapon = GetWeaponByActiveSlot();

	if (IsValid(CurrentWeapon))
	{
		return CurrentWeapon->DamagePerBullet;
	}

	return 0.0f;
}

void UFTaleInventoryComponent::FillInventory()
{
	if (!IsValid(ComponentOwner))
	{
		return;
	}

	if (ComponentOwner->HasAuthority())
	{
		if (StartupWeapons.Num() > 0)
		{
			UWorld* World = GetWorld();
			if (!IsValid(World))
			{
				return;
			}

			for (TSubclassOf<AFTaleWeaponBase>& Iter : StartupWeapons)
			{
				AFTaleWeaponBase* SpawnedWeapon = World->SpawnActor<AFTaleWeaponBase>(Iter, FVector(0, 0, 0), FRotator(0, 0, 0));
				Inventory.Add(SpawnedWeapon);

				SpawnedWeapon->SetActorHiddenInGame(true);

				SpawnedWeapon->AttachToComponent(ComponentOwner->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName(SpawnedWeapon->SocketForAttachWeapon));

				SpawnedWeapon->SetOwner(ComponentOwner);
			}
		}
	}
}

void UFTaleInventoryComponent::SetActiveSlot1()
{
	SetActiveSlot(0);
}

void UFTaleInventoryComponent::Server_SetActiveSlot1_Implementation()
{
	SetActiveSlot1();
}

void UFTaleInventoryComponent::SetActiveSlot2()
{
	SetActiveSlot(1);
}

void UFTaleInventoryComponent::RestoreAmmo()
{
	for (AFTaleWeaponBase* Iter : Inventory)
	{
		if (IsValid(Iter))
		{
			Iter->RestoreAmmo();
		}
	}

	UpdateUIAmmo();
}

void UFTaleInventoryComponent::Multicast_OnOwnerDeath_Implementation()
{
	for (AFTaleWeaponBase* Iter : Inventory)
	{
		if (IsValid(Iter))
		{
			Iter->SetActorHiddenInGame(true);
		}
	}
}

void UFTaleInventoryComponent::Multicast_OnOwnerRespawn_Implementation()
{
	if (IsValid(GetWeaponByActiveSlot()))
	{
		GetWeaponByActiveSlot()->SetActorHiddenInGame(false);
	}
}

void UFTaleInventoryComponent::Server_SetActiveSlot2_Implementation()
{
	SetActiveSlot2();
}

void UFTaleInventoryComponent::SetActiveSlot(int32 SlotInex)
{
	if (!IsValid(ComponentOwner))
	{
		return;
	}

	if (ComponentOwner->HasAuthority())
	{
		if (AFTaleWeaponBase* CurrentWeapon = GetWeaponByActiveSlot())
		{
			CurrentWeapon->SetActorHiddenInGame(true);
		}

		ActiveSlotIndex = SlotInex;

		if (AFTaleWeaponBase* WeaponToEquip = GetWeaponByActiveSlot())
		{
			WeaponToEquip->Equip();
			WeaponToEquip->SetActorHiddenInGame(false);

			UpdateUIAmmo();

			if (ComponentOwner->GetLocalRole() == ROLE_Authority)
			{
				NewWeaponEquippedDelegate.Broadcast();
			}
		}
	}
	else
	{
		if (SlotInex == 0)
		{
			Server_SetActiveSlot1();
		}
		else if(SlotInex == 1)
		{
			Server_SetActiveSlot2();
		}
	}
}

void UFTaleInventoryComponent::UpdateUIAmmo()
{
	//Process ListenServer UI ammo update
	//if (IsValid(ComponentOwner))
	//{
	//	if (ComponentOwner->GetLocalRole() == ROLE_Authority)
	//	{
	if (IsValid(GetWeaponByActiveSlot()))
	{
		ClipAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->ClipSizeCurrent);
		BagAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->AmmoInBagCurrent);
	}

	//			return;
	//		}
	//	}
	//}

	//Process Client UI ammo update
	//if (IsValid(GetWeaponByActiveSlot()))
	//{
	//	ClipAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->ClipSizeMax);
	//	BagAmmoUpdaterDelegate.Broadcast(GetWeaponByActiveSlot()->AmmoInBagMax);
	//}
}
