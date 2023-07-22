// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Spline/SplineFollowComponent.h"
#include "Components/ActorComponent.h"
#include "SR_AutoDriveComponent.generated.h"

class ASR_BaseVehicle;

/**
 * Component handles spline following and other automatic behavior.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMPLERACER_API USR_AutoDriveComponent : public USplineFollowComponent
{
	GENERATED_BODY()

public:
	USR_AutoDriveComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

protected:
	virtual void UpdateErrors(float DeltaTime) override;
	virtual void UpdateInputs(float DeltaTime) override;
	virtual void ApplyInputs(float DeltaTime) override;

	UPROPERTY(Transient)
	ASR_BaseVehicle* Vehicle = nullptr;

	// Difference between desired and actual speed (used to update PID controller)
	float SpeedError = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PID)
	FPIDConfig SpeedPIDConfig;

	UPROPERTY(BlueprintReadOnly, Category = PID)
	FPIDState SpeedPIDState;

	float SteeringInputValue = 0.f;
	float ThrottleInputValue = 0.f;
	float BrakeInputValue = 0.f;

	// the maximum target speed the AI will try to drive (in km/h)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float MaxTargetSpeed = 70.f;
};
