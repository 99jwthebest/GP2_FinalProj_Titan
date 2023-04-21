// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GP2_JH_PersonalProjCharacter.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"

#include "JumpLandShake.h"
#include "LedgeGrabShake.h"
#include "MantleShake.h"
#include "QuickMantleShake.h"

#include "GameFramework/CharacterMovementComponent.h"
#include "ParkourMovementComponent.generated.h"

UENUM(BlueprintType)
enum class EParkourMovementType : uint8
{
	None, //UMETA(DisplayName = "Option 1"),
	LeftWallRun, //UMETA(DisplayName = "Option 2"),
	RightWallRun, //UMETA(DisplayName = "Option 3"),
	VerticalWallRun, //UMETA(DisplayName = "Option 4"),
	LedgeGrab, //UMETA(DisplayName = "Option 5"),
	Mantle, //UMETA(DisplayName = "Option 6"),
	Slide, //UMETA(DisplayName = "Option 7"),
	Crouch,
	Sprint, //UMETA(DisplayName = "Option 8"),
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP2_JH_PERSONALPROJ_API UParkourMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UParkourMovementComponent();

	//Default values
	float DefaultGravity;
	float DefaultGroundFriction;
	float DefaultBrakingDeceleration;
	float DefaultWalkSpeed;
	float DefaultCrouchSpeed;

	FVector rightEndPoint;
	FVector leftEndPoint;
	FVector WallRunNormal;
	FVector Eyes;
	FVector Feet;
	FVector mantlePosition;
	FVector VerticalWallRunNormal;
	float wallRunSpeed;
	float WallRunTargetGravity;
	float WallRunJumpHeight;
	float WallRunJumpOffForce;
	float mantleHeight;
	float VerticalWallRunSpeed;
	float MantleTraceDistance;
	float QuickMantleSpeed;
	float MantleSpeed;
	float ledgeGrabJumpOffForce;
	float ledgeGrabJumpHeight;
	float sprintSpeed;
	float slideImpulseAmount;
	float slideImpulseAmount2;
	float WallRunSprintSpeed;
	FTimerHandle TimerForWallRunSuppressed;
	UPROPERTY(VisibleAnywhere)
	bool OnWall;
	bool WallRunGravity;
	bool Change;
	bool CameraShake;
	UPROPERTY(VisibleAnywhere)
	bool SprintQueued;
	bool SlideQueued;

	int slideCount;
	float slidecountTIMING;
	

	EMovementMode PrevMovementMode;
	EMovementMode CurrentMovementMode;

	EParkourMovementType PrevParkourMovementMode;
	UPROPERTY(BlueprintReadWrite, Category = Weapon) 
	EParkourMovementType CurrentParkourMovementMode;
	
	EParkourMovementType NoneMode;
	EParkourMovementType RightWRun;
	EParkourMovementType LeftWRun;
	EParkourMovementType MantleMode;
	EParkourMovementType VerticalWRunMode;
	EParkourMovementType LedgeGrabMode;
	EParkourMovementType SprintMode;
	EParkourMovementType CrouchMode;
	EParkourMovementType SlideMode;
	
	//bools for gates
	bool openWallRunGate;
	bool closeWallRunGate;

	bool openMantleCheckGate;
	bool closeMantleCheckGate;

	bool openMantleGate;
	bool closeMantleGate;

	bool openVerticalWallRunGate;
	bool closeVerticalWallRunGate;

	bool openSprintGate;
	bool closeSprintGate;

	bool openSlideGate;
	bool closeSlideGate;

	
	
	void InitializeP();
	//void TriggerEventWallRun();
	void WallRunUpdateEvent();
	void WallRunEndVectors();
	bool WallRunMovement(FVector StartLocation, FVector EndLocation, float WallRunDirection);
	void SetWallRunGravity();
	void WallRunEnd(float ResetTime);

	void CameraTick();
	void CameraTilt(float TargetXRoll);
	void WallRunJump();
	void LandEventCustom();
	void JumpEventCustom();
	void CloseWallRunGate();
	void OpenWallRunGate();

	void MovementChanged(EMovementMode prevMoveMode, EMovementMode newMoveMode);
	void ParkourMovementChanged(EParkourMovementType prevParMoveMode, EParkourMovementType NewParMoveMode);
	void ResetMovement();
	bool SetParkourMovementMode(EParkourMovementType NewMode);
	void PlayJumpLandShake(); 
	void PlayLedgeGrabShake();
	void PlayMantleShake();
	void PlayQuickMantleShake();
	void OpenVerticalWallRunGate();
	void CloseVerticalWallRunGate();
	void VerticalWallRunUpdate();
	float DetectForwardInput();
	void VerticalWallRunEnd(float ResetTime);
	void MantleVectors();
	void VerticalWallRunMovement();
	
	void OpenMantleCheckGate();
	void CloseMantleCheckGate();
	void OpenMantleGate();
	void CloseMantleGate();
	void MantleCheck();
	
	void OpenAllGates();

	void LedgeGrabJump();

	void CrouchSlideEvent();

	void SprintStart();
	void SprintEnd();
	void CrouchStart();
	void CrouchEnd();
	void SlideStart();
	void SlideEnd(bool crouch);
	void OpenSprintGate();
	void CloseSprintGate();
	void SprintUpdate();

	void OpenSlideGate();
	void CloseSlideGate();

	void SlideUpdate();

	void SprintJump();
	void CheckQueues();
	void SlideJump();
	void CrouchJump();
	void SlideCountingUp();
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere)
	AGP2_JH_PersonalProjCharacter* MyCharacter;

};
