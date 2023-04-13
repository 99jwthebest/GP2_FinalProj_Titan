// Fill out your copyright notice in the Description page of Project Settings.


#include "ParkourMovementComponent.h"

#include <string>

#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "LegacyCameraShake.h"
#include "TimerManager.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values for this component's properties
UParkourMovementComponent::UParkourMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	CameraShake = true;
	WallRunGravity = false;
	wallRunSpeed = 850.0f;
	WallRunTargetGravity = 0.25f;
	WallRunJumpHeight = 400.0f;
	WallRunJumpOffForce = 300.0f;
	mantleHeight = 40.0f;
	VerticalWallRunSpeed = 300.0f;
	MantleSpeed = 10.0f;
	QuickMantleSpeed = 20.0f;
	ledgeGrabJumpOffForce = 300.0f;
	ledgeGrabJumpHeight = 400.0f;
	sprintSpeed = 1000.0f;
	slideImpulseAmount = 600.0f;
	WallRunSprintSpeed = 1200.0f;

	NoneMode = EParkourMovementType::None;
	RightWRun = EParkourMovementType::RightWallRun;
	LeftWRun = EParkourMovementType::LeftWallRun;
	MantleMode = EParkourMovementType::Mantle;
	VerticalWRunMode = EParkourMovementType::VerticalWallRun;
	LedgeGrabMode = EParkourMovementType::LedgeGrab;
	SprintMode = EParkourMovementType::Sprint;
	CrouchMode = EParkourMovementType::Crouch;
	SlideMode = EParkourMovementType::Slide;


	/*openWallRunGate = true;
	closeWallRunGate = false;

	openVerticalWallRunGate = true;
	closeVerticalWallRunGate = false;
	*/
	
	openMantleCheckGate = false;
	closeMantleCheckGate = false;

	openMantleGate = false;
	closeMantleGate = false;

	SprintQueued = false;	

}


// Called when the game starts
void UParkourMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	MyCharacter = Cast<AGP2_JH_PersonalProjCharacter>(GetOwner());

}


// Called every frame
void UParkourMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}


void UParkourMovementComponent::InitializeP()
{
	if (MyCharacter != nullptr)
	{
		//Default Values
		DefaultGravity = MyCharacter->GetCharacterMovement()->GravityScale;
		DefaultGroundFriction = MyCharacter->GetCharacterMovement()->GroundFriction;
		DefaultBrakingDeceleration = MyCharacter->GetCharacterMovement()->BrakingDecelerationWalking;
		DefaultWalkSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeed;
		DefaultCrouchSpeed = MyCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched;
		
		FTimerHandle MyTimerHandleW;
		FTimerHandle MyTimerHandleVW;
		FTimerHandle MyTimerHandleMC;
		FTimerHandle MyTimerHandleCT;
		FTimerHandle MyTimerHandleSpT;
		FTimerHandle MyTimerHandleSlideT;

		
		UE_LOG(LogTemp, Warning, TEXT("Runnning Timer EVent!!"))

		GetWorld()->GetTimerManager().SetTimer(MyTimerHandleW, this, &UParkourMovementComponent::WallRunUpdateEvent, 0.0167f, true);
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandleVW, this, &UParkourMovementComponent::VerticalWallRunUpdate, 0.0167f, true);
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandleMC, this, &UParkourMovementComponent::MantleCheck, 0.0167f, true);
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandleSpT, this, &UParkourMovementComponent::SprintUpdate, 0.0167f, true);
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandleSlideT, this, &UParkourMovementComponent::SlideUpdate, 0.0167f, true);
		GetWorld()->GetTimerManager().SetTimer(MyTimerHandleCT, this, &UParkourMovementComponent::CameraTick, 0.0167f, true);
	}
}

void UParkourMovementComponent::WallRunUpdateEvent()
{
	if(closeWallRunGate == false && openWallRunGate == true)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Wall Run Open for sprinting!!"))
		WallRunEndVectors();  // CurrentParkourMovementMode == EParkourMovementType::Sprint ||
		if(DetectForwardInput() > 0.0f &&
			CurrentParkourMovementMode == EParkourMovementType::None ||
			CurrentParkourMovementMode == EParkourMovementType::LeftWallRun ||
			CurrentParkourMovementMode == EParkourMovementType::RightWallRun)
		{
			if (WallRunMovement(MyCharacter->GetActorLocation(), rightEndPoint, -1.0f))
			{
				SetParkourMovementMode(RightWRun);

				SetWallRunGravity();
			}
			else if (CurrentParkourMovementMode == EParkourMovementType::RightWallRun)
			{
				WallRunEnd(0.5f);
			}
			else
			{
				WallRunEndVectors();
				if (WallRunMovement(MyCharacter->GetActorLocation(), leftEndPoint, 1.0f))
				{
					SetParkourMovementMode(LeftWRun);
					
					SetWallRunGravity();
				}
				else
				{
					WallRunEnd(0.5f);
				}
			}
		}
		else
		{
			WallRunEnd(1.0f);
		}
	}
}

void UParkourMovementComponent::WallRunEndVectors()
{
	// so we can draw a ray cast slightly backwards so that the player can rotate the camera while shooting and they won't fall off the wall.

	FVector characterLocation = MyCharacter->GetActorLocation();

	FVector charRightVector = MyCharacter->GetActorRightVector();
	float rightMultiplier = 75.0f; //length of 75 for the raycast
	FVector newRightVector = charRightVector * rightMultiplier;

	FVector charForwardVector = MyCharacter->GetActorForwardVector();
	float forwardMultiplier = -35.0f;  // multiply the forward vector by -35.0f so that it's pointing slightly backwards.
	FVector newForwardVector = charForwardVector * forwardMultiplier;

	rightEndPoint = characterLocation + newRightVector + newForwardVector;



	FVector characterLocation2 = MyCharacter->GetActorLocation();

	FVector charRightVector2 = MyCharacter->GetActorRightVector();
	float rightMultiplier2 = -75.0f; //length of 75 for the raycast
	FVector newRightVector2 = charRightVector2 * rightMultiplier2;

	FVector charForwardVector2 = MyCharacter->GetActorForwardVector();
	float forwardMultiplier2 = -35.0f;  // multiply the forward vector by -35.0f so that it's pointing slightly backwards.
	FVector newForwardVector2 = charForwardVector2 * forwardMultiplier2;

    leftEndPoint = characterLocation2 + newRightVector2 + newForwardVector2;

}

bool UParkourMovementComponent::WallRunMovement(FVector StartLocation, FVector EndLocation, float WallRunDirection)
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;

	GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, TraceParams);

	OnWall = false;

	float zMin = -0.52f;
	float zMax = 0.52f;

	WallRunNormal = HitResult.Normal;
	
	if (HitResult.bBlockingHit)
	{
		if (FMath::IsWithinInclusive(WallRunNormal.Z, zMin, zMax) && MyCharacter->GetCharacterMovement()->IsFalling())
		{
			float launchingFloat = UKismetMathLibrary::SelectFloat(WallRunSprintSpeed, wallRunSpeed, SprintQueued) * WallRunDirection;
			
			FVector upVector;
			upVector = FVector(0.0f, 0.0f, 1.0f);
			FVector launchingVector = FVector::CrossProduct(WallRunNormal, upVector) * launchingFloat;
			// calculate the forward direction and the wall run direction tells which way to run on the wall
			if (WallRunGravity == false)
			{
				// Move player forward
				MyCharacter->LaunchCharacter(launchingVector, true, true);
				OnWall = true;
				return OnWall;
			}
			else
			{
				// Move player forward
				MyCharacter->LaunchCharacter(launchingVector, true, false);
				OnWall = true;
				return OnWall;
			}
		}
	}

	OnWall = false;
	return OnWall;
}

void UParkourMovementComponent::SetWallRunGravity()
{
	float GScale = MyCharacter->GetCharacterMovement()->GravityScale;
	float DeltaTime = GetWorld()->GetDeltaSeconds();
	float currentValue;

	currentValue = FMath::FInterpTo(GScale, WallRunTargetGravity, DeltaTime, 10.0f);

	MyCharacter->GetCharacterMovement()->GravityScale = currentValue;
}

void UParkourMovementComponent::WallRunEnd(float ResetTime)
{
	if (CurrentParkourMovementMode == EParkourMovementType::LeftWallRun || CurrentParkourMovementMode == EParkourMovementType::RightWallRun)
	{
		if(SetParkourMovementMode(NoneMode))
		{
			CloseWallRunGate();
			UE_LOG(LogTemp, Warning, TEXT("Closed the WallRunGate in wallRunEnd!!!!"))
			WallRunGravity = false;

			FTimerHandle TimerForCloseGate;
			GetWorld()->GetTimerManager().SetTimer(TimerForCloseGate, this, &UParkourMovementComponent::OpenWallRunGate, ResetTime, false);
			UE_LOG(LogTemp, Warning, TEXT("Opened the WallRunGate in wallRunEnd!!!!"))

		}
	}
}

void UParkourMovementComponent::CameraTick()
{
	switch (CurrentParkourMovementMode)
	{
		case EParkourMovementType::None:
			CameraTilt(0.0f);
		//UE_LOG(LogTemp, Warning, TEXT("mode is set to none after sprinting!!"))
			break;
		case EParkourMovementType::LeftWallRun:
			CameraTilt(15.0f);
			break;
		case EParkourMovementType::RightWallRun:
			CameraTilt(-15.0f);
			break;
		case EParkourMovementType::VerticalWallRun:
			CameraTilt(0.0f);
			break;
		case EParkourMovementType::LedgeGrab:
			CameraTilt(0.0f);
			break;
		case EParkourMovementType::Mantle:
			CameraTilt(0.0f);
			//UE_LOG(LogTemp, Warning, TEXT("Mantle tilt working!!!!"))
			break;
		case EParkourMovementType::Slide:
			CameraTilt(-15.0f);
			break;
		case EParkourMovementType::Crouch:
			CameraTilt(0.0f);
			break;
		case EParkourMovementType::Sprint:
			CameraTilt(0.0f);
			break;
		default:
			//MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, (uint8)EParkourMovementType::Sprint);
			// Handle other movement types here
			break;
	}
}

void UParkourMovementComponent::CameraTilt(float TargetxRoll)
{
	
	FRotator currentRotation = MyCharacter->GetController()->GetControlRotation();

	float TargetyPitch = MyCharacter->GetController()->GetControlRotation().Pitch;
	float TargetzYaw = MyCharacter->GetController()->GetControlRotation().Yaw;
	FRotator TargetRoll(TargetyPitch, TargetzYaw, TargetxRoll);

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	FRotator NewRotation = FMath::RInterpTo(currentRotation, TargetRoll, DeltaTime, 10.0f);

	MyCharacter->GetController()->SetControlRotation(NewRotation);
}

void UParkourMovementComponent::WallRunJump()
{
	if (CurrentParkourMovementMode == EParkourMovementType::LeftWallRun || CurrentParkourMovementMode == EParkourMovementType::RightWallRun)
	{
		float WallXResult = WallRunNormal.X * WallRunJumpOffForce; //WallRunNormal has no info.
		float WallYResult = WallRunNormal.Y * WallRunJumpOffForce;

		FVector LaunchingVelocity(WallXResult, WallYResult, WallRunJumpHeight);

		WallRunEnd(0.35f);
		MyCharacter->LaunchCharacter(LaunchingVelocity, false, true);
	}
}

void UParkourMovementComponent::LandEventCustom()
{
	//end events
	WallRunEnd(0.0f);
	VerticalWallRunEnd(0.0f);
	SprintEnd();
	SlideEnd(false);
	
	CloseWallRunGate();
	CloseVerticalWallRunGate();
	CloseSlideGate();
	CloseSprintGate();
	PlayJumpLandShake();
	CheckQueues();
}

void UParkourMovementComponent::JumpEventCustom()
{
	if(CurrentParkourMovementMode == EParkourMovementType::None)
	{
		if(MyCharacter->GetCharacterMovement()->IsFalling())
		{
			
		}
		else
		{
			OpenAllGates();
			PlayJumpLandShake();
		}
	}
	else 
	{
		WallRunJump();
		LedgeGrabJump();
		SlideJump();
		CrouchJump();
		SprintJump();
	}
}

void UParkourMovementComponent::CloseWallRunGate()
{
	closeWallRunGate = true;
	openWallRunGate = false;
}

void UParkourMovementComponent::OpenWallRunGate()
{
	closeWallRunGate = false;
	openWallRunGate = true;
}

void UParkourMovementComponent::MovementChanged(EMovementMode prevMoveMode, EMovementMode newMoveMode)
{
	UE_LOG(LogTemp, Warning, TEXT("Character is valid low level!!"))
	if (MyCharacter->IsValidLowLevel())
	{

		PrevMovementMode = prevMoveMode;
		CurrentMovementMode = newMoveMode;

		if(PrevMovementMode == MOVE_Walking && CurrentMovementMode == MOVE_Falling)
		{
			SprintJump();
			WallRunEnd(0.0f);
			VerticalWallRunEnd(0.0f);
			SprintEnd();
			SlideEnd(false);
			OpenAllGates();
		}
		else if(PrevMovementMode == MOVE_Falling && CurrentMovementMode == MOVE_Walking)
		{
			CheckQueues();
		}
	}
}

void UParkourMovementComponent::ParkourMovementChanged(EParkourMovementType prevParMoveMode, EParkourMovementType NewParMoveMode)
{
	PrevParkourMovementMode = prevParMoveMode;
	CurrentParkourMovementMode = NewParMoveMode;

	ResetMovement();
}

void UParkourMovementComponent::ResetMovement()
{
	if (CurrentParkourMovementMode == EParkourMovementType::None || CurrentParkourMovementMode == EParkourMovementType::Crouch)
	{
		MyCharacter->GetCharacterMovement()->GravityScale = DefaultGravity;
		MyCharacter->GetCharacterMovement()->GroundFriction = DefaultGroundFriction;
		MyCharacter->GetCharacterMovement()->BrakingDecelerationWalking = DefaultBrakingDeceleration;
		MyCharacter->GetCharacterMovement()->MaxWalkSpeed = DefaultWalkSpeed;
		MyCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = DefaultCrouchSpeed;

		MyCharacter->GetCharacterMovement()->SetPlaneConstraintEnabled(false);

		//UE_LOG(LogTemp, Warning, TEXT("MovementMode is equal to leftWallRun, in reset!!"))

		// Set the parkour movement mode
		switch (PrevParkourMovementMode)
		{
			case EParkourMovementType::None:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				break;
			case EParkourMovementType::LeftWallRun:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				break;
			case EParkourMovementType::RightWallRun:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				break;
			case EParkourMovementType::VerticalWallRun:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				break;
			case EParkourMovementType::LedgeGrab:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Falling);
				break;
			case EParkourMovementType::Mantle:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				break;
			case EParkourMovementType::Slide:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				break;
			case EParkourMovementType::Crouch:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				break;
			case EParkourMovementType::Sprint:
				MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
				break;

			default:
				//MyCharacter->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking, (uint8)EParkourMovementType::Sprint);
				// Handle other movement types here
				break;
		}
	}
}

bool UParkourMovementComponent::SetParkourMovementMode(EParkourMovementType NewMode)
{
	if (NewMode == CurrentParkourMovementMode)
	{
		UE_LOG(LogTemp, Warning, TEXT("MovementMode Did not change!!"))
		Change = false;
		return Change;
	}
	else
	{
		ParkourMovementChanged(CurrentParkourMovementMode, NewMode);
		UE_LOG(LogTemp, Warning, TEXT("MovementMode Changed!!"))
	}
	Change = true;
	return Change;
}

void UParkourMovementComponent::PlayJumpLandShake()
{
	if(CameraShake)
	{
		FVector shakeRot = MyCharacter->GetActorLocation();
		float innerRadius = 0.0f;
		float outerRadius = 100.0f;
		float fallOff = 1.0f;
		
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), UJumpLandShake::StaticClass(), shakeRot, innerRadius, outerRadius, fallOff, false);
	}
}

void UParkourMovementComponent::PlayLedgeGrabShake()
{
	if(CameraShake)
	{
		FVector shakeRot = MyCharacter->GetActorLocation();
		float innerRadius = 0.0f;
		float outerRadius = 100.0f;
		float fallOff = 1.0f;
		
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), ULedgeGrabShake::StaticClass(), shakeRot, innerRadius, outerRadius, fallOff, false);
	}
}

void UParkourMovementComponent::PlayMantleShake()
{
	if(CameraShake)
	{
		FVector shakeRot = MyCharacter->GetActorLocation();
		float innerRadius = 0.0f;
		float outerRadius = 100.0f;
		float fallOff = 1.0f;
		
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), UMantleShake::StaticClass(), shakeRot, innerRadius, outerRadius, fallOff, false);
	}
}

void UParkourMovementComponent::PlayQuickMantleShake()
{
	if(CameraShake)
	{
		FVector shakeRot = MyCharacter->GetActorLocation();
		float innerRadius = 0.0f;
		float outerRadius = 100.0f;
		float fallOff = 1.0f;
		
		UGameplayStatics::PlayWorldCameraShake(GetWorld(), UQuickMantleShake::StaticClass(), shakeRot, innerRadius, outerRadius, fallOff, false);
	}
}

void UParkourMovementComponent::OpenVerticalWallRunGate()
{
	openVerticalWallRunGate = true;
	closeVerticalWallRunGate = false;
}

void UParkourMovementComponent::CloseVerticalWallRunGate()
{
	openVerticalWallRunGate = false;
	closeVerticalWallRunGate = true;
	
	CloseMantleGate();
}

void UParkourMovementComponent::VerticalWallRunUpdate()
{
	if(openVerticalWallRunGate == true && closeVerticalWallRunGate == false)
	{
		if(DetectForwardInput() > 0.0f && MyCharacter->GetCharacterMovement()->IsFalling() &&
			CurrentParkourMovementMode == EParkourMovementType::None || CurrentParkourMovementMode == EParkourMovementType::VerticalWallRun ||
			CurrentParkourMovementMode == EParkourMovementType::LeftWallRun || CurrentParkourMovementMode == EParkourMovementType::RightWallRun)
		{
			MantleVectors();
			
			const TArray<AActor*> NewActors = { /* your AActor pointers here */ };
			const TArray<AActor*>& Actors = NewActors;
			FHitResult outHit;
			FHitResult& HitResult = outHit;

			if(UKismetSystemLibrary::CapsuleTraceSingle(GetWorld(), Eyes, Feet, 20.0f, 10.0f,
				TraceTypeQuery1, false, NewActors, EDrawDebugTrace::None, HitResult, true))
			{
				MantleTraceDistance = HitResult.Distance;
				UE_LOG(LogTemp, Warning, TEXT("Mantle Trace Distance in Update: %f"), MantleTraceDistance);

				if(MyCharacter->GetCharacterMovement()->IsWalkable(HitResult))
				{
					FVector forhalfheight (0.0f, 0.0f, MyCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());

					mantlePosition = HitResult.ImpactPoint + forhalfheight;
					UE_LOG(LogTemp, Warning, TEXT("Mantle Position In Update: %s"), *mantlePosition.ToString());

					CloseVerticalWallRunGate();
					
					if(SetParkourMovementMode(LedgeGrabMode))
					{
						MyCharacter->GetCharacterMovement()->DisableMovement();
						MyCharacter->GetMovementComponent()->StopMovementImmediately(); //might have to change to get characatermovement

						MyCharacter->GetCharacterMovement()->GravityScale = 0.0f;
						
						PlayLedgeGrabShake(); 

						if(MantleTraceDistance > MyCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
						{
							OpenMantleCheckGate(); //might have to be backwards
						}
						else
						{
							FTimerHandle timerforMantle;
							GetWorld()->GetTimerManager().SetTimer(timerforMantle, this, &UParkourMovementComponent::OpenMantleCheckGate, 0.25f, false);
						}
					}
				}
				else
				{
					VerticalWallRunMovement();
				}
			}
			else
			{
				VerticalWallRunMovement();
			}
		}
		else
		{
			VerticalWallRunEnd(0.35f);
		}
	}
}

float UParkourMovementComponent::DetectForwardInput()
{
	//Detecting forward input
	FVector actorFowardVector = MyCharacter->GetActorForwardVector();
	FVector lastInputVector = MyCharacter->GetCharacterMovement()->GetLastInputVector();

	float dotProduct = FVector::DotProduct(actorFowardVector, lastInputVector);

	return dotProduct;
}


void UParkourMovementComponent::VerticalWallRunEnd(float ResetTime)
{
	if(CurrentParkourMovementMode == EParkourMovementType::LedgeGrab ||
		CurrentParkourMovementMode == EParkourMovementType::VerticalWallRun ||
		CurrentParkourMovementMode == EParkourMovementType::Mantle)
	{
		if(SetParkourMovementMode(NoneMode))
		{
			CloseVerticalWallRunGate();
			CloseMantleCheckGate();
			FTimerHandle TimerForVerticalRunGate;
			GetWorld()->GetTimerManager().SetTimer(TimerForVerticalRunGate, this, &UParkourMovementComponent::OpenVerticalWallRunGate, ResetTime, false);
			//Checking to see if you are already sprinting and if you are then you can continue sprint after a wall run or wall climb, it will automatically sprint again.
			FTimerHandle TimerForCheckQueues;
			GetWorld()->GetTimerManager().SetTimer(TimerForCheckQueues, this, &UParkourMovementComponent::CheckQueues, 0.2f, false);
		}
	}
	else
	{
		
	}
}

void UParkourMovementComponent::MantleVectors()
{
	//Mantle Vectors
	FRotator outRot;		
	FVector outLoc;
	MyCharacter->GetController()->GetActorEyesViewPoint(outLoc, outRot);
	
	FVector addToOutLoc (0.0f, 0.0f, 50.0f);
	FVector outLocAdded = outLoc + addToOutLoc;
		
	FVector multipliedForwVector = MyCharacter->GetActorForwardVector() * 50.0f;

	FVector bothVecsAdded = outLocAdded + multipliedForwVector;
		
	Eyes = bothVecsAdded;

		
	//Might have to change bottom to Getunscaledcapsulehalfheight
	float capsuleHei = MyCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	float subtractedHeight = capsuleHei - mantleHeight;
	FVector splitVec (0.0f, 0.0f, subtractedHeight);
		
	FVector actorLoc = MyCharacter->GetActorLocation();
	FVector subtractedVector = actorLoc - splitVec;
	FVector addSubVec = subtractedVector + multipliedForwVector;
		
	Feet = addSubVec;
}

void UParkourMovementComponent::VerticalWallRunMovement()
{
	FHitResult HitResult;
	FCollisionQueryParams TraceParams;
	TraceParams.bTraceComplex = true;
	TraceParams.bReturnPhysicalMaterial = false;
	//TraceParams.AddIgnoredActors(ObjectsToIgnore);

	MantleVectors();

	FVector multipliedForwVector = MyCharacter->GetActorForwardVector() * 50.0f;
	FVector addResult = Feet + multipliedForwVector;

	if(DetectForwardInput() > 0.0f && GetWorld()->LineTraceSingleByChannel(HitResult, Feet, addResult, ECC_Visibility, TraceParams))
	{
		VerticalWallRunNormal = HitResult.ImpactNormal;
		SetParkourMovementMode(VerticalWRunMode);

		float multipliedXvector = VerticalWallRunNormal.X * -600.0f;
		float multipliedYvector = VerticalWallRunNormal.Y * -600.0f;
		FVector launchVel (multipliedXvector, multipliedYvector, VerticalWallRunSpeed);
		
		MyCharacter->LaunchCharacter(launchVel, true, true);
	}
	else
	{
		VerticalWallRunEnd(0.35f);
	}
}


void UParkourMovementComponent::OpenMantleCheckGate()
{
	openMantleCheckGate = true;
	closeMantleCheckGate = false;
}

void UParkourMovementComponent::CloseMantleCheckGate()
{
	openMantleCheckGate = false;
	closeMantleCheckGate = true;
}

void UParkourMovementComponent::OpenMantleGate()
{
	openMantleGate = true;
	closeMantleGate = false;
	UE_LOG(LogTemp, Warning, TEXT("Mantle Gate is Open!!"))

}

void UParkourMovementComponent::CloseMantleGate()
{
	openMantleGate = false;
	closeMantleGate = true;
}

void UParkourMovementComponent::MantleCheck()
{
	if (openMantleCheckGate == true && closeMantleCheckGate == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Went through check in mantle check!!"))
		if(DetectForwardInput() > 0.0f && CurrentParkourMovementMode == EParkourMovementType::LedgeGrab ||
			MantleTraceDistance > MyCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
		{
			if(SetParkourMovementMode(MantleMode))
			{
				if(MantleTraceDistance > MyCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
				{

					PlayQuickMantleShake();
					CloseMantleCheckGate();
					OpenMantleGate();
				}
				else
				{

					PlayMantleShake();
					CloseMantleCheckGate();
					OpenMantleGate();
				}
			}
		}
	}
	else if(openMantleGate == true && closeMantleGate == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Mantle Move is working!!"))

		FVector LookAtStart (MyCharacter->GetActorLocation().X, MyCharacter->GetActorLocation().Y, 0.0f);
		FVector LookAtTarget (mantlePosition.X, mantlePosition.Y, 0.0f);
		FRotator TargetRotate  = UKismetMathLibrary::FindLookAtRotation(LookAtStart, LookAtTarget);

		float deltaTime = GetWorld()->GetDeltaSeconds();
		
		FRotator interpRotate = FMath::RInterpTo(MyCharacter->GetController()->GetControlRotation(), TargetRotate, deltaTime, 7.0f);

		MyCharacter->GetController()->SetIgnoreLookInput(true);
		MyCharacter->GetController()->SetControlRotation(interpRotate);

		if(MantleTraceDistance > MyCharacter->GetCapsuleComponent()->GetScaledCapsuleHalfHeight())
		{
			FVector quickMantleVec = FMath::VInterpTo(MyCharacter->GetActorLocation(), mantlePosition, deltaTime, QuickMantleSpeed);
			MyCharacter->SetActorLocation(quickMantleVec);
			float VectorLength = FVector::Dist(MyCharacter->GetActorLocation(), mantlePosition);
			if (VectorLength < 8.0f)
			{
				MyCharacter->GetController()->ResetIgnoreLookInput();
				VerticalWallRunEnd(0.5f);
				UE_LOG(LogTemp, Warning, TEXT("Made it to quick mantle end!!"))

			}
		}
		else
		{
			FVector slowMantleVec = FMath::VInterpTo(MyCharacter->GetActorLocation(), mantlePosition, deltaTime, MantleSpeed);
			MyCharacter->SetActorLocation(slowMantleVec);
			float VectorLength = FVector::Dist(MyCharacter->GetActorLocation(), mantlePosition);
			if (VectorLength < 8.0f)
			{
				MyCharacter->GetController()->ResetIgnoreLookInput();
				VerticalWallRunEnd(0.5f);
				UE_LOG(LogTemp, Warning, TEXT("Made it to slow mantle end!!"))
			}
		}
	}
}

void UParkourMovementComponent::OpenAllGates()
{
		OpenWallRunGate();
		OpenVerticalWallRunGate();
		OpenSlideGate();
		OpenSprintGate();
}

void UParkourMovementComponent::LedgeGrabJump()
{
	if(CurrentParkourMovementMode == EParkourMovementType::LedgeGrab ||
		CurrentParkourMovementMode == EParkourMovementType::VerticalWallRun ||
		CurrentParkourMovementMode == EParkourMovementType::Mantle)
	{
		VerticalWallRunEnd(0.35);

	
		float multipliedXvector = VerticalWallRunNormal.X * ledgeGrabJumpOffForce;  //Vertical Wall Run Normal has no info.
		float multipliedYvector = VerticalWallRunNormal.Y * ledgeGrabJumpOffForce;
		FVector launchVel (multipliedXvector, multipliedYvector, ledgeGrabJumpHeight);
		
		MyCharacter->LaunchCharacter(launchVel, false, true);
	}
}

void UParkourMovementComponent::CrouchSlideEvent()
{
	//Parkour Cancel
	if(CurrentParkourMovementMode == EParkourMovementType::Mantle ||
		CurrentParkourMovementMode == EParkourMovementType::LedgeGrab ||
		CurrentParkourMovementMode == EParkourMovementType::VerticalWallRun)
	{
		VerticalWallRunEnd(0.5f);
	}
	else if (CurrentParkourMovementMode == EParkourMovementType::LeftWallRun ||
			CurrentParkourMovementMode == EParkourMovementType::RightWallRun)
	{
		WallRunEnd(0.5f);
	}
	else
	{
		//Can Slide
		if(DetectForwardInput() > 0.0f && CurrentParkourMovementMode == EParkourMovementType::Sprint || SprintQueued)
		{
			if(MyCharacter->GetCharacterMovement()->IsWalking())
			{
				SlideStart();
			}
			else
			{
				SlideQueued = true;
			}
		}
		else
		{
			//Crouch Toggle
			switch (CurrentParkourMovementMode)
			{
				case EParkourMovementType::None:
					CrouchStart();
					break;
				case EParkourMovementType::LeftWallRun:
					break;
				case EParkourMovementType::RightWallRun:
					break;
				case EParkourMovementType::VerticalWallRun:
					break;
				case EParkourMovementType::LedgeGrab:
					break;
				case EParkourMovementType::Mantle:
					break;
				case EParkourMovementType::Slide:
					break;
				case EParkourMovementType::Crouch:
					CrouchEnd();
					break;
				case EParkourMovementType::Sprint:
					break;
				default:
					break;
			}
		}
	}
}

void UParkourMovementComponent::SprintStart()
{
	CrouchEnd();
	SlideEnd(false);

	if(CurrentParkourMovementMode == EParkourMovementType::None && MyCharacter->GetCharacterMovement()->IsWalking() || SprintQueued)
	{
		UE_LOG(LogTemp, Warning, TEXT("Sprint Start going throught first check!!!!"))

		if(SetParkourMovementMode(SprintMode))
		{
			UE_LOG(LogTemp, Warning, TEXT("Sprinting!!!!"))

			MyCharacter->GetCharacterMovement()->MaxWalkSpeed = sprintSpeed;
			OpenSprintGate();
			SprintQueued = false;
			SlideQueued = false;
		}
	}
}

void UParkourMovementComponent::SprintEnd()
{
	if(CurrentParkourMovementMode == EParkourMovementType::Sprint)
	{
		if(SetParkourMovementMode(NoneMode))
		{
			UE_LOG(LogTemp, Warning, TEXT("SettingParkour to none for wallrunning!!!!"))
			OpenWallRunGate();
			OpenVerticalWallRunGate();
			CloseSprintGate();
			FTimerHandle OpenSprintHandle;
			GetWorld()->GetTimerManager().SetTimer(OpenSprintHandle, this, &UParkourMovementComponent::OpenSprintGate, 0.1f, false);

		}
	}
}

void UParkourMovementComponent::CrouchStart()
{
	if(CurrentParkourMovementMode == EParkourMovementType::None)
	{
		MyCharacter->Crouch();
		SetParkourMovementMode(CrouchMode);
		SprintQueued = false;
		SlideQueued = false;
	}
}

void UParkourMovementComponent::CrouchEnd()
{
	if(CurrentParkourMovementMode == EParkourMovementType::Crouch)
	{
		MyCharacter->UnCrouch();
		SetParkourMovementMode(NoneMode);
		SprintQueued = false;
		SlideQueued = false;
	}
}

void UParkourMovementComponent::SlideStart()
{
	if(DetectForwardInput() > 0.0f &&
		MyCharacter->GetCharacterMovement()->IsWalking() &&
		CurrentParkourMovementMode == EParkourMovementType::Sprint || SprintQueued)
	{
		SprintEnd();
		SetParkourMovementMode(SlideMode);
		MyCharacter->Crouch();

		MyCharacter->GetCharacterMovement()->GroundFriction = 0.0f;
		MyCharacter->GetCharacterMovement()->BrakingDecelerationWalking = 1400.0; //can change it if you want to make it slide for a longer time
		MyCharacter->GetCharacterMovement()->MaxWalkSpeedCrouched = 0.0f;

		FVector forwardVelNormalized = MyCharacter->GetCharacterMovement()->Velocity.GetSafeNormal(); //might have to change to normalize or kismetVector normalize
		MyCharacter->GetCharacterMovement()->SetPlaneConstraintFromVectors(forwardVelNormalized, MyCharacter->GetActorUpVector());

		MyCharacter->GetCharacterMovement()->SetPlaneConstraintEnabled(true);


		//Get Slide Vector
		FHitResult HitResult;
		FCollisionQueryParams TraceParams;
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = false;
		//TraceParams.AddIgnoredActors(ObjectsToIgnore);
		
		FVector actorLoc = MyCharacter->GetActorLocation();
		FVector upVector = MyCharacter->GetActorUpVector() * -200.0f;
		FVector addALocToUp = actorLoc + upVector;
		
		GetWorld()->LineTraceSingleByChannel(HitResult, actorLoc, addALocToUp, ECC_Visibility, TraceParams);

		FVector actRightVec = MyCharacter->GetActorRightVector();
		FVector crossProductOfSlide = FVector::CrossProduct(HitResult.ImpactNormal, actRightVec) * -1.0f;

		FVector impulse = crossProductOfSlide * slideImpulseAmount;
		if(crossProductOfSlide.Z <= 0.02f)
		{
			MyCharacter->GetCharacterMovement()->AddImpulse(impulse, true);
			OpenSlideGate();

			SprintQueued = false;
			SlideQueued = false;
		}
		else
		{
			OpenSlideGate();
			SprintQueued = false;
			SlideQueued = false;
		}

	}
}

void UParkourMovementComponent::SlideEnd(bool crouch)
{
	if(CurrentParkourMovementMode == EParkourMovementType::Slide)
	{
		if(crouch == true)
		{
			if(SetParkourMovementMode(CrouchMode))
			{
				CloseSlideGate();
				
				if(crouch == true)
				{
					UE_LOG(LogTemp, Warning, TEXT("Crouch actually Succeeded!!"))

				}
				else
				{
					MyCharacter->UnCrouch();
					UE_LOG(LogTemp, Warning, TEXT("Uncrouched!!"))

				}
			}
		}
		else
		{
			if(SetParkourMovementMode(NoneMode))
			{
				CloseSlideGate();

				MyCharacter->UnCrouch();
			}
		}
	}
}

void UParkourMovementComponent::OpenSprintGate()
{
	openSprintGate = true;
	closeSprintGate = false;
}

void UParkourMovementComponent::CloseSprintGate()
{
	openSprintGate = false;
	closeSprintGate = true;
}

void UParkourMovementComponent::SprintUpdate()
{
	if(openSprintGate == true && closeSprintGate == false)
	{
		if(CurrentParkourMovementMode == EParkourMovementType::Sprint)
		{
			if (DetectForwardInput() > 0.0f)
			{

			}
			else
			{
				SprintEnd();
			}
		}
	}
}

void UParkourMovementComponent::OpenSlideGate()
{
	openSlideGate = true;
	closeSlideGate = false;
}

void UParkourMovementComponent::CloseSlideGate()
{
	openSlideGate = false;
	closeSlideGate = true;
}

void UParkourMovementComponent::SlideUpdate()
{
	if(openSlideGate == true && closeSlideGate == false)
	{
		if(CurrentParkourMovementMode == EParkourMovementType::Slide)
		{
			if(MyCharacter->GetCharacterMovement()->Velocity.Length() <= 35.0f)
			{
				UE_LOG(LogTemp, Warning, TEXT("Slide Ending!!"))
				SlideEnd(true);
			}
		}
	}
}

void UParkourMovementComponent::SprintJump()
{
	if(CurrentParkourMovementMode == EParkourMovementType::Sprint)
	{
		SprintEnd();
		SprintQueued = true;
	}
}

void UParkourMovementComponent::CheckQueues()
{
	if(SlideQueued)
	{
		SlideStart();
	}
	else if (SprintQueued)
	{
		SprintStart();
		UE_LOG(LogTemp, Warning, TEXT("Sprint Start being Executed from check queues!!"))
	}
}

void UParkourMovementComponent::SlideJump()
{
	if(CurrentParkourMovementMode == EParkourMovementType::Slide)
	{
		SlideEnd(false);
	}
}

void UParkourMovementComponent::CrouchJump()
{
	if(CurrentParkourMovementMode == EParkourMovementType::Crouch)
	{
		CrouchEnd();
	}
}





