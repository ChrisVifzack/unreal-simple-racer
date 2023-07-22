// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PawnMovementComponent.h"
#include "SR_VehicleMovementComponent.generated.h"

class USR_VehicleInputComponent;


UENUM(BlueprintType)
enum class ESR_VehicleMovementAction : uint8
{
	None,
	Jump,
	Dash	
};

USTRUCT()
struct FSR_VehicleMovementConfig
{
	GENERATED_BODY()

	// The maximum speed the vehicle can reach
	UPROPERTY(EditAnywhere, Category = "Engine", meta=(Units="CentimetersPerSecond"))
	float MaxSpeed = 3000.0f;

	UPROPERTY(EditAnywhere, Category = "Engine")
	float EngineThrust = 2000.0f;

	UPROPERTY(EditAnywhere, Category = "Engine")
	float EngineBrake = 150.0f;

	UPROPERTY(EditAnywhere, Category = "Brake")
	float BrakeForce = 1500.0f;

	UPROPERTY(EditAnywhere)
	float GravityZ = 980.0f;

	UPROPERTY(EditAnywhere, Category = "Steering")
	float MaxSteeringAngle = 10.0f;

	UPROPERTY(EditAnywhere, Category = "Action")
	float JumpForce = 5000.0f;

	UPROPERTY(EditAnywhere, Category = "Action")
	float JumpCooldown = 1.5f;

	UPROPERTY(EditAnywhere, Category = "Action")
	float DashForce = 2000.0f;
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnMovementActionChanged, ESR_VehicleMovementAction, OldAction, ESR_VehicleMovementAction, NewAction);

/**
 * Using the Simple Single Track Model to emulate basic vehicle physics.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMPLERACER_API USR_VehicleMovementComponent : public UPawnMovementComponent
{
	GENERATED_BODY()

public:
	USR_VehicleMovementComponent();

	// Returns the component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "VehicleMovement")
	static USR_VehicleMovementComponent* FindVehicleMovementComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<USR_VehicleMovementComponent>() : nullptr);
	}

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebug = false;

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void AsyncPhysicsTickComponent(float delta, float sim) override;

	void TickMovement(float DeltaTime);

	FVector GetLocalVelocity() const;

	void UpdateGroundDetection();

protected:
	FHitResult DetectGround(const FVector& Location);

	void ApplyGravity(const float DeltaTime);
	void ApplySuspension(const float DeltaTime);

	void ApplyThrust(const float DeltaTime, const float ThrottleInput, const float SteeringInput);
	void ApplySteering(const float DeltaTime, const float SteeringInput);
	void ApplyBrake(const float DeltaTime, const float BrakeInput);
	void ApplyEngineBrake(const float DeltaTime, const float ThrottleInput, const float BrakeInput);

	void ApplyLateralDamping(const float DeltaTime);

	void ApplySpeedLimit();

	UPROPERTY(BlueprintReadOnly)
	ESR_VehicleMovementAction MovementAction = ESR_VehicleMovementAction::None;

	UPROPERTY(EditDefaultsOnly)
	FSR_VehicleMovementConfig VehicleMovementConfig = FSR_VehicleMovementConfig();

	UPROPERTY(Transient)
	TObjectPtr<USR_VehicleInputComponent> VehicleInput = nullptr;

	float AxleDistance = 100.f;

	float PrevSuspensionError = 0.f;

	// for suspension
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DesiredGroundDistance = 50.f;

	// wheel positions are used for traces for ground detection
	FVector AxleOriginFL = FVector::ZeroVector;
	FVector AxleOriginFR = FVector::ZeroVector;
	FVector AxleOriginBL = FVector::ZeroVector;
	FVector AxleOriginBR = FVector::ZeroVector;

	// Updated during ground detection and used to align vehicle rotation with ground.
	FVector GroundNormal = FVector::UpVector;

	// Updated during ground detection and used to fake suspension
	float GroundDistance = 0.f;

	// Updated during ground detection.
	bool bIsGrounded = false;

	FCollisionQueryParams TireTraceQueryParams = FCollisionQueryParams();

	bool IsJumpInCooldown = false;

	UFUNCTION()
	void ResetJumpCooldown()
	{
		IsJumpInCooldown = false;
	}

	FTimerHandle JumpCooldownTimerHandle;

protected: 
	void SetMovementAction(ESR_VehicleMovementAction NewAction)
	{
		if (MovementAction != NewAction)
		{
			auto OldAction = MovementAction;
			MovementAction = NewAction;
			OnMovementActionChanged.Broadcast(OldAction, MovementAction);
		}
	}

public:
	ESR_VehicleMovementAction GetMovementAction() const
	{
		return MovementAction;
	}

	UPROPERTY(BlueprintAssignable)
	FOnMovementActionChanged OnMovementActionChanged;

	UFUNCTION(BlueprintCallable)
	bool TryJump();

	UFUNCTION(BlueprintCallable)
	bool TryDash(); 

protected:
	void Jump();
	void Dash();

public:
	UFUNCTION()
	void OnDashEnded()
	{
		SetMovementAction(ESR_VehicleMovementAction::None);
	}
};
