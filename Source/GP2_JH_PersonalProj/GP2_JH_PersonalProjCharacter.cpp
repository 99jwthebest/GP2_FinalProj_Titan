// Copyright Epic Games, Inc. All Rights Reserved.

#include "GP2_JH_PersonalProjCharacter.h"
#include "GP2_JH_PersonalProjProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ParkourMovementComponent.h"
#include "ParkourMovement.h"

//////////////////////////////////////////////////////////////////////////
// AGP2_JH_PersonalProjCharacter

AGP2_JH_PersonalProjCharacter::AGP2_JH_PersonalProjCharacter()
{
	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	JumpMaxCount = 2;

}

void AGP2_JH_PersonalProjCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//Maybe change to AParkourMovement* ParkourMove = Cast<AParkourMovement>(GetWorld()->GetFirstPlayerController()->GetPawn()); parkourMove->ParkourM();
	parkourMove = FindComponentByClass<UParkourMovementComponent>();
	if(parkourMove != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Found Component"))
		parkourMove->InitializeP();
	}
}


//////////////////////////////////////////////////////////////////////////// Input

void AGP2_JH_PersonalProjCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AGP2_JH_PersonalProjCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AGP2_JH_PersonalProjCharacter::Look);

		//Crouch
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AGP2_JH_PersonalProjCharacter::Crouching);

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AGP2_JH_PersonalProjCharacter::Sprinting);

		
	}
}


void AGP2_JH_PersonalProjCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void AGP2_JH_PersonalProjCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void AGP2_JH_PersonalProjCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AGP2_JH_PersonalProjCharacter::GetHasRifle()
{
	return bHasRifle;
}

void AGP2_JH_PersonalProjCharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);
	//The "Super::Landed(Hit)" call is necessary to ensure that the default implementation in the AActor class is called first.
	UE_LOG(LogTemp, Warning, TEXT("Land is working!!!"))

	parkourMove->LandEventCustom();
}

void AGP2_JH_PersonalProjCharacter::Jump()
{
	Super::Jump();
	//UE_LOG(LogTemp, Warning, TEXT("Jump Count: %i"), JumpCurrentCount)
	parkourMove->JumpEventCustom();
}

void AGP2_JH_PersonalProjCharacter::OnMovementModeChanged(EMovementMode PreviousMovementMode, EMovementMode NewMovementMode)
{
	Super::OnMovementModeChanged(PreviousMovementMode, NewMovementMode);

	parkourMove->MovementChanged(PreviousMovementMode, NewMovementMode);
	//UE_LOG(LogTemp, Warning, TEXT("MovementMode Changed!!!"))
	
	EMovementMode curMode = GetCharacterMovement()->MovementMode;
	UE_LOG(LogTemp, Warning, TEXT("Movement mode changed to: %d"), static_cast<int32>(curMode));
}

void AGP2_JH_PersonalProjCharacter::Sprinting()
{
	parkourMove->SprintStart();
}

void AGP2_JH_PersonalProjCharacter::Crouching()
{
	parkourMove->CrouchSlideEvent();
}


