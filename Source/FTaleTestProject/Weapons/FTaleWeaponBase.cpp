// Fill out your copyright notice in the Description page of Project Settings.


#include "FTaleTestProject/Weapons/FTaleWeaponBase.h"

#include "Animation/AnimMontage.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FTaleTestProject/Characters/FTaleTestProjectCharacter.h"
#include "FTaleTestProject/Weapons/FTaleProjectile.h"
#include "Camera/CameraComponent.h"
#include "FTaleTestProject/Components/FTaleInventoryComponent.h"
#include "Net/UnrealNetwork.h"

#pragma optimize("", off)

AFTaleWeaponBase::AFTaleWeaponBase()
{
	PrimaryActorTick.bCanEverTick = true;

	/*WeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComp"));*/
	SKWeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMeshComp"));
}

void AFTaleWeaponBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AFTaleWeaponBase, ClipSizeCurrent);
	DOREPLIFETIME(AFTaleWeaponBase, AmmoInBagCurrent);
}

void AFTaleWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	ClipSizeCurrent = ClipSizeMax;
	AmmoInBagCurrent = AmmoInBagMax;
}

void AFTaleWeaponBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AFTaleWeaponBase::OnRep_ClipSizeCurrent()
{
	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (IsValid(WeaponOwner->InventoryComponent))
		{
			WeaponOwner->InventoryComponent->ClipAmmoUpdaterDelegate.Broadcast(ClipSizeCurrent);
		}
	}
}

void AFTaleWeaponBase::OnRep_BagSizeCurrent()
{
	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (IsValid(WeaponOwner->InventoryComponent))
		{
			WeaponOwner->InventoryComponent->BagAmmoUpdaterDelegate.Broadcast(AmmoInBagCurrent);
		}
	}
}

void AFTaleWeaponBase::StartFire()
{
	if (!CanFire())
	{
		return;
	}

	if (bIsSigleShotWeapon)
	{
		Fire();
	}
	else
	{
		UWorld* World = GetWorld();
		if (!World)
		{
			return;
		}

		World->GetTimerManager().SetTimer(FireTimerHandle, this, &AFTaleWeaponBase::Fire, FireRate, true, false);
	}

}

void AFTaleWeaponBase::Fire()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	FVector StartPoint;
	if (SKWeaponMesh)
	{
		StartPoint = SKWeaponMesh->GetSocketLocation(FName(MuzzleSocketName));
	}

	FVector Direction;
	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (UCameraComponent* Camera = WeaponOwner->GetFollowCamera())
		{
			Direction = Camera->GetForwardVector();
		}
	}

	if (HasAuthority())
	{
		AFTaleProjectile* SpawnedProjectile = World->SpawnActor<AFTaleProjectile>(Projectile, StartPoint, (Direction * ShotDistance).Rotation());
		
		if (IsValid(SpawnedProjectile))
		{
			SpawnedProjectile->DamageToApply = DamagePerBullet;
		}

		if (ClipSizeCurrent > 0)
		{
			ClipSizeCurrent--;
		}

		OnRep_ClipSizeCurrent();

	}
	else
	{
		Server_Fire();
	}
}

void AFTaleWeaponBase::Server_Fire_Implementation()
{
	if (HasAuthority())
	{
		Fire();
	}
}

void AFTaleWeaponBase::StopFire()
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	World->GetTimerManager().ClearTimer(FireTimerHandle);
}

bool AFTaleWeaponBase::CanFire()
{
	if (bIsReloading || ClipSizeCurrent <= 0)
	{
		return false;
	}

	return true;
}

void AFTaleWeaponBase::StartReload()
{
	if (!CanReload())
	{
		return;
	}

	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (!IsValid(WeaponOwner->GetMesh()) || !IsValid(SKWeaponMesh))
		{
			return;
		}

		if (!IsValid(WeaponOwner->GetMesh()->GetAnimInstance()) || !IsValid(SKWeaponMesh->GetAnimInstance()))
		{
			return;
		}

		if (!WeaponReloadMontage || !CharacterReloadMontage)
		{
			return;
		}

		bIsReloading = true;

		PlayReloadAnimation();
	}
}

void AFTaleWeaponBase::OnReloadEnd(UAnimMontage* Montage, bool bInterrupted)
{
	int32 ClipSizeToAddIntoCurrent = ClipSizeMax - ClipSizeCurrent;

	if (ClipSizeToAddIntoCurrent <= AmmoInBagCurrent)
	{
		ClipSizeCurrent = ClipSizeMax;
		AmmoInBagCurrent -= ClipSizeToAddIntoCurrent;
	}
	else if (ClipSizeToAddIntoCurrent > AmmoInBagCurrent)
	{
		ClipSizeCurrent += AmmoInBagCurrent;
		AmmoInBagCurrent = 0;
	}

	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (IsValid(WeaponOwner->InventoryComponent))
		{
			WeaponOwner->InventoryComponent->ClipAmmoUpdaterDelegate.Broadcast(ClipSizeCurrent);
			WeaponOwner->InventoryComponent->BagAmmoUpdaterDelegate.Broadcast(AmmoInBagCurrent);
		}

		if (IsValid(WeaponOwner->GetMesh()))
		{
			if (IsValid(WeaponOwner->GetMesh()->GetAnimInstance()))
			{
				WeaponOwner->GetMesh()->GetAnimInstance()->OnMontageEnded.Clear();
			}
		}
	}

	bIsReloading = false;

}

bool AFTaleWeaponBase::CanReload()
{
	if (IsCurrentClipFull())
	{
		return false;
	}


	if (bIsReloading)
	{
		return false;
	}

	return HaveEnoughAmmo();
}

bool AFTaleWeaponBase::HaveEnoughAmmo()
{
	if (AmmoInBagCurrent > 0)
	{
		return true;
	}

	return false;
}

bool AFTaleWeaponBase::IsCurrentClipFull()
{
	if (ClipSizeCurrent == ClipSizeMax)
	{
		return true;
	}

	return false;
}

void AFTaleWeaponBase::RestoreAmmo()
{
	ClipSizeCurrent = ClipSizeMax;
	AmmoInBagCurrent = AmmoInBagMax;

	//if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	//{
	//	if (WeaponOwner->GetLocalRole() == ROLE_Authority)
	//	{
	//		OnRep_ClipSizeCurrent();
	//		OnRep_BagSizeCurrent();
	//	}
	//}
}

void AFTaleWeaponBase::Multicast_PlayReloadAnimation_Implementation()
{
	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (!IsValid(WeaponOwner->GetMesh()) || !IsValid(SKWeaponMesh))
		{
			return;
		}

		if (!IsValid(WeaponOwner->GetMesh()->GetAnimInstance()) || !IsValid(SKWeaponMesh->GetAnimInstance()))
		{
			return;
		}

		if (!WeaponReloadMontage || !CharacterReloadMontage)
		{
			return;
		}

		SKWeaponMesh->GetAnimInstance()->Montage_Play(WeaponReloadMontage, 1.f, EMontagePlayReturnType());
		WeaponOwner->GetMesh()->GetAnimInstance()->Montage_Play(CharacterReloadMontage, 1.f, EMontagePlayReturnType());

		WeaponOwner->GetMesh()->GetAnimInstance()->OnMontageEnded.AddDynamic(this, &AFTaleWeaponBase::OnReloadEnd);
	}
}

void AFTaleWeaponBase::PlayReloadAnimation()
{
	if (AFTaleTestProjectCharacter* WeaponOwner = Cast<AFTaleTestProjectCharacter>(GetOwner()))
	{
		if (WeaponOwner->GetLocalRole() == ROLE_Authority)
		{
			Multicast_PlayReloadAnimation();
		}
		else
		{
			Server_PlayReloadAnimation();
			Multicast_PlayReloadAnimation();
		}
	}
}

void AFTaleWeaponBase::Server_PlayReloadAnimation_Implementation()
{
	PlayReloadAnimation();
}

void AFTaleWeaponBase::Equip()
{
}

void AFTaleWeaponBase::UnEquip()
{
}

bool AFTaleWeaponBase::CanEquip()
{
	return false;
}

#pragma optimize("", on)