// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimInstanceProxy.h"
#include "Vehicle/Components/SR_VehicleMovementComponent.h"
#include "SR_VehicleAnimInstance.generated.h"


USTRUCT()
struct SIMPLERACER_API FSR_VehicleAnimInstanceProxy : public FAnimInstanceProxy
{
	GENERATED_BODY()

	/** FAnimInstanceProxy interface begin*/
	virtual void PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds) override;
	/** FAnimInstanceProxy interface end*/

	ESR_VehicleMovementAction VehicleMovementAction = ESR_VehicleMovementAction::None;

	float VehicleForwardAcceleration = 0.0f;
	float VehicleSteeringSpeed = 0.0f;
	float VehicleThrottleInput = 0.0f;
	float VehicleBrakeInput = 0.0f;
	float VehicleSteeringInputRaw = 0.0f;
	float VehicleSteeringInputSmooth = 0.0f;

	bool bIsJumping = false;
};

UCLASS()
class SIMPLERACER_API USR_VehicleAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

private:
	/** UAnimInstance interface begin*/
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	virtual FAnimInstanceProxy* CreateAnimInstanceProxy() override;
	virtual void DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy) override;
	/** UAnimInstance interface end*/

	FSR_VehicleAnimInstanceProxy AnimInstanceProxy;

protected:
	// Track time that has passed since action was started.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentActiveActionTime = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "Forward Acceleration (Normalized)")
	float ForwardAcceleration = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, DisplayName = "Steering Speed (Normalized)")
	float SteeringSpeed = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SteeringInputRaw = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float SteeringInputSmooth = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ThrottleInput = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float BrakeInput = 0.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	bool bIsJumping = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	ESR_VehicleMovementAction VehicleMovementAction = ESR_VehicleMovementAction::None;

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ForwardAccelerationSmoothingRate = 2.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float ForwardAccelerationMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SteeringSpeedSmoothingRate = 10.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float SteeringSpeedMultiplier = 0.1f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DashAnimDuration = 0.5f;

protected:
	UFUNCTION()
	void OnMovementActionChanged(ESR_VehicleMovementAction OldAction, ESR_VehicleMovementAction NewAction);

	UPROPERTY(Transient)
	TObjectPtr<USR_VehicleMovementComponent> VehicleMovementComponent = nullptr;
};
