// Copyright Epic Games, Inc. All Rights Reserved.

#include "FTaleTestProjectCharacter.h"

#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Net/UnrealNetwork.h"
#include "FTaleTestProject/Components/FTaleAttributeComponent.h"
#include "FTaleTestProject/Components/FTaleInventoryComponent.h"
#include "FTaleTestProject/Weapons/FTaleWeaponBase.h"

DEFINE_LOG_CATEGORY(LogTemplateCharacter);

//////////////////////////////////////////////////////////////////////////
// AFTaleTestProjectCharacter

AFTaleTestProjectCharacter::AFTaleTestProjectCharacter()
{
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f); // ...at this rotation rate

	GetCharacterMovement()->JumpZVelocity = 700.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = 500.f;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 1500.0f;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	//Create an inventory
	InventoryComponent = CreateDefaultSubobject<UFTaleInventoryComponent>(TEXT("Inventory"));

	//Create an attributes
	AttributeComponent = CreateDefaultSubobject<UFTaleAttributeComponent>(TEXT("Attributes"));

	bReplicates = true;
}

void AFTaleTestProjectCharacter::BeginPlay()
{ 
	Super::BeginPlay();

	bIsAlive = true;

	
}

void AFTaleTestProjectCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AFTaleTestProjectCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//DOREPLIFETIME(AFTaleTestProjectCharacter, Health);

	DOREPLIFETIME(AFTaleTestProjectCharacter, RightMovementValue);
	DOREPLIFETIME(AFTaleTestProjectCharacter, ForwardMovementValue);
	DOREPLIFETIME(AFTaleTestProjectCharacter, FollowCameraPitchRotation);
}

//////////////////////////////////////////////////////////////////////////
// Input

void AFTaleTestProjectCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
	
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {
		
		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AFTaleTestProjectCharacter::Move);
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Completed, this, &AFTaleTestProjectCharacter::MoveEnd);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFTaleTestProjectCharacter::Look);

		EnhancedInputComponent->BindAction(ScoreBoardAction, ETriggerEvent::Started, this, &AFTaleTestProjectCharacter::ShowScoreBoard);
		EnhancedInputComponent->BindAction(ScoreBoardAction, ETriggerEvent::Completed, this, &AFTaleTestProjectCharacter::HideScoreBoard);

		EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Started, this, &AFTaleTestProjectCharacter::StartReload);

		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFTaleTestProjectCharacter::StartFire);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AFTaleTestProjectCharacter::StopFire);

		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AFTaleTestProjectCharacter::StartAim);
		EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AFTaleTestProjectCharacter::StopAim);

		EnhancedInputComponent->BindAction(InventorySlot1Action, ETriggerEvent::Started, InventoryComponent, &UFTaleInventoryComponent::SetActiveSlot1);
		EnhancedInputComponent->BindAction(InventorySlot2Action, ETriggerEvent::Started, InventoryComponent, &UFTaleInventoryComponent::SetActiveSlot2);
	}
	else
	{
		UE_LOG(LogTemplateCharacter, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AFTaleTestProjectCharacter::ShowScoreBoard()
{
}

void AFTaleTestProjectCharacter::HideScoreBoard()
{
}

void AFTaleTestProjectCharacter::StartAim()
{
	bIsAiming = true;

	MoveSpringArm();
}

void AFTaleTestProjectCharacter::StopAim()
{
	bIsAiming = false;

	MoveSpringArm();
}

void AFTaleTestProjectCharacter::Server_Death_Implementation()
{
	Death();
}

void AFTaleTestProjectCharacter::Multicast_Death_Implementation()
{
	SetActorHiddenInGame(true);
}

void AFTaleTestProjectCharacter::Death()
{
	if (GetLocalRole() == ROLE_Authority)
	{

		bIsAlive = false;

		Multicast_Death();

		if (IsValid(InventoryComponent))
		{
			InventoryComponent->Multicast_OnOwnerDeath();
		}

		if (UWorld* World = GetWorld())
		{
			World->GetTimerManager().SetTimer(RespawnTimerHandle, this, &AFTaleTestProjectCharacter::Respawn, RespawnTime, false);
		}
	}
	else if(GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Death();
		Multicast_Death();
	}
}

void AFTaleTestProjectCharacter::Server_Respawn_Implementation()
{
	Respawn();
}

void AFTaleTestProjectCharacter::Multicast_Respawn_Implementation()
{
	SetActorHiddenInGame(false);
}

void AFTaleTestProjectCharacter::Respawn()
{
	if (GetLocalRole() == ROLE_Authority)
	{

		bIsAlive = true;

		Multicast_Respawn();
		
		if (IsValid(InventoryComponent))
		{
			InventoryComponent->Multicast_OnOwnerRespawn();
			InventoryComponent->RestoreAmmo();
		}

		if (IsValid(AttributeComponent))
		{
			AttributeComponent->RestoreAttribute();
		}
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_Respawn();
		Multicast_Respawn();
	}
}

void AFTaleTestProjectCharacter::MovementReplication(float RightMovement, float ForwardMovement)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		RightMovementValue = RightMovement;
		ForwardMovementValue = ForwardMovement;
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_MovementReplication(RightMovement, ForwardMovement);
	}
}

void AFTaleTestProjectCharacter::Server_MovementReplication_Implementation(float RightMovement, float ForwardMovement)
{
	MovementReplication(RightMovement, ForwardMovement);
}

void AFTaleTestProjectCharacter::PitchRotationReplication(float PitchRotation)
{
	if (GetLocalRole() == ROLE_Authority)
	{
		FollowCameraPitchRotation = PitchRotation;
	}
	else if (GetLocalRole() == ROLE_AutonomousProxy)
	{
		Server_PitchRotationReplication(PitchRotation);
	}
}

void AFTaleTestProjectCharacter::Server_PitchRotationReplication_Implementation(float PitchRotation)
{
	PitchRotationReplication(PitchRotation);
}

void AFTaleTestProjectCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	MovementReplication(MovementVector.X, MovementVector.Y);
	/*RightMovementValue = MovementVector.X;
	ForwardMovementValue = MovementVector.Y;*/

	if (Controller != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	
		// get right vector
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, MovementVector.Y);
		AddMovementInput(RightDirection, MovementVector.X);
	}
}

void AFTaleTestProjectCharacter::MoveEnd()
{
	MovementReplication(0.f, 0.f);
}

void AFTaleTestProjectCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);

		if (IsValid(FollowCamera))
		{
			PitchRotationReplication(FollowCamera->GetComponentRotation().Pitch);
		}
	}
}

void AFTaleTestProjectCharacter::StartFire()
{
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	if (AFTaleWeaponBase* CurrentWeapon = InventoryComponent->GetWeaponByActiveSlot())
	{
		CurrentWeapon->StartFire();
	}
}

void AFTaleTestProjectCharacter::StopFire()
{
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	if (AFTaleWeaponBase* CurrentWeapon = InventoryComponent->GetWeaponByActiveSlot())
	{
		CurrentWeapon->StopFire();
	}
}

void AFTaleTestProjectCharacter::StartReload()
{
	if (!IsValid(InventoryComponent))
	{
		return;
	}

	if (AFTaleWeaponBase* CurrentWeapon = InventoryComponent->GetWeaponByActiveSlot())
	{
		CurrentWeapon->StartReload();
	}
}