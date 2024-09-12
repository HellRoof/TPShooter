// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Logging/LogMacros.h"
#include "FTaleTestProjectCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UFTaleInventoryComponent;
class UFTaleAttributeComponent;
class UInputMappingContext;
class UInputAction;
class UAnimMontage;
class AFTaleWeaponBase;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateCharacter, Log, All);

UCLASS(config=Game)
class AFTaleTestProjectCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFTaleTestProjectCharacter();

private:

	//Camera

	/** Camera boom positioning the camera behind the character */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;

	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

public:

	//Input

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* LookAction;

	/** Score Board Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ScoreBoardAction;

	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* ReloadAction;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* FireAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* AimAction;

	/** Equip Slot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventorySlot1Action;

	/** Equip Slot Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	UInputAction* InventorySlot2Action;


protected:

	// APawn interface
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// To add mapping context
	virtual void BeginPlay();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	void MoveEnd();

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	//Fire logic
	void StartFire();

	void StopFire();

	//Reload logic
	void StartReload();

	//Score board logic
	void ShowScoreBoard();
	void HideScoreBoard();

public:

	//TO DO
	UFUNCTION(BlueprintImplementableEvent)
	void MoveSpringArm();

	void StartAim();
	void StopAim();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CameraBoomNormalDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float CameraBoomAimDistance;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float InAimModeSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float OutAimModeSpeed;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAiming;



	bool bIsAlive;

	UPROPERTY(EditAnywhere)
	float RespawnTime;

	UFUNCTION(Server, Reliable)
	void Server_Death();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Death();

	void Death();

	UFUNCTION(Server, Reliable)
	void Server_Respawn();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Respawn();

	UFUNCTION()
	void Respawn();

	FTimerHandle RespawnTimerHandle;



	//Components
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFTaleInventoryComponent* InventoryComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UFTaleAttributeComponent* AttributeComponent;

	//Movement vars for new BS
	UPROPERTY(Replicated, BlueprintReadOnly)
	float RightMovementValue;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float ForwardMovementValue;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float FollowCameraPitchRotation;

	UFUNCTION(Server, Reliable)
	void Server_MovementReplication(float RightMovement, float ForwardMovement);
	void MovementReplication(float RightMovement, float ForwardMovement);

	UFUNCTION(Server, Reliable)
	void Server_PitchRotationReplication(float PitchRotation);
	void PitchRotationReplication(float PitchRotation);



	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UAnimMontage* CharacterDeathMontage;

	/** Returns CameraBoom subobject **/
	FORCEINLINE class USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE class UCameraComponent* GetFollowCamera() const { return FollowCamera; }
};

