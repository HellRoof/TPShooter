// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FTaleWeaponBase.generated.h"

class UStaticMeshComponent;
class USkeletalMeshComponent;
class UAnimMontage;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBulletsCounterDelegate, int32, BulletCounter);

class AFTaleProjectile;

UCLASS()
class FTALETESTPROJECT_API AFTaleWeaponBase : public AActor
{
	GENERATED_BODY()
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	AFTaleWeaponBase();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	USkeletalMeshComponent* SKWeaponMesh;

	UPROPERTY(EditAnywhere)
	FString SocketForAttachWeapon;

	UPROPERTY(EditAnywhere)
	bool bIsSigleShotWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsMainSlotWeapon;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	bool bIsSecondarySlotWeapon;

	UPROPERTY(EditAnywhere)
	float DamagePerBullet;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 ClipSizeMax;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ClipSizeCurrent)
	int32 ClipSizeCurrent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int32 AmmoInBagMax;

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BagSizeCurrent)
	int32 AmmoInBagCurrent;

	UPROPERTY(EditAnywhere)
	float ShotDistance;

	UPROPERTY(EditAnywhere)
	float FireRate;

	UPROPERTY(EditAnywhere)
	FString MuzzleSocketName;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AFTaleProjectile> Projectile;

	UFUNCTION()
	void OnRep_ClipSizeCurrent();

	UFUNCTION()
	void OnRep_BagSizeCurrent();



	//Fire logic
	FTimerHandle FireTimerHandle;

	void StartFire();

	UFUNCTION(Server, reliable)
	void Server_Fire();

	UFUNCTION()
	void Fire();

	void StopFire();

	bool CanFire();

	bool bIsFiring;



	//Reload logic
	void StartReload();

	UFUNCTION()
	void OnReloadEnd(UAnimMontage* Montage, bool bInterrupted);

	bool CanReload();

	bool HaveEnoughAmmo();

	bool IsCurrentClipFull();

	void RestoreAmmo();

	UPROPERTY(BlueprintReadWrite)
	bool bIsReloading;

	UFUNCTION(Server, Reliable)
	void Server_PlayReloadAnimation();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlayReloadAnimation();

	void PlayReloadAnimation();



	UPROPERTY(BlueprintAssignable)
	FBulletsCounterDelegate BulletCounterDelegate;



	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* CharacterReloadMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* WeaponReloadMontage;



	//TO DO
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* WeaponEquipMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* WeaponUnequipMontage;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* CharacterFireMontage;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* WeaponFireMontage;

	//TO DO
	void Equip();
	void UnEquip();
	bool CanEquip();
	bool bIsEquiping;
	bool bIsEquipped;
	
};
