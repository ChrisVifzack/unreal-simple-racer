// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_VehicleAnimInstance.h"
#include "Vehicle/Components/SR_VehicleInputComponent.h"


void FSR_VehicleAnimInstanceProxy::PreUpdate(UAnimInstance* InAnimInstance, float DeltaSeconds)
{
	FAnimInstanceProxy::PreUpdate(InAnimInstance, DeltaSeconds);

	auto AnimInstance = Cast<USR_VehicleAnimInstance>(InAnimInstance);
	if (!AnimInstance)
	{
		return;
	}

	auto VehicleInputComponent = USR_VehicleInputComponent::FindVehicleInputComponent(AnimInstance->GetOwningActor());
	if (!VehicleInputComponent)
	{
		return;
	}

	auto VehicleMovementComponent = USR_VehicleMovementComponent::FindVehicleMovementComponent(AnimInstance->GetOwningActor());
	if (!VehicleMovementComponent)
	{
		return;
	}

	VehicleThrottleInput = VehicleInputComponent->GetThrottleInput();
	VehicleBrakeInput = VehicleInputComponent->GetBrakeInput();
	VehicleSteeringInputRaw = VehicleInputComponent->GetSteeringInput(true);
	VehicleSteeringInputSmooth = VehicleInputComponent->GetSteeringInput(false);

	VehicleMovementAction = VehicleMovementComponent->GetMovementAction();
	bIsJumping = VehicleMovementAction == ESR_VehicleMovementAction::Jump;
}

void USR_VehicleAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	VehicleMovementComponent = USR_VehicleMovementComponent::FindVehicleMovementComponent(GetOwningActor());
	if (VehicleMovementComponent)
	{
		VehicleMovementComponent->OnMovementActionChanged.AddUniqueDynamic(this, &ThisClass::OnMovementActionChanged);
	}
}

void USR_VehicleAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	ThrottleInput = AnimInstanceProxy.VehicleThrottleInput;
	BrakeInput = AnimInstanceProxy.VehicleBrakeInput;
	SteeringInputRaw = AnimInstanceProxy.VehicleSteeringInputRaw;
	SteeringInputSmooth = AnimInstanceProxy.VehicleSteeringInputSmooth;

	ForwardAcceleration = AnimInstanceProxy.VehicleForwardAcceleration;
	SteeringSpeed = AnimInstanceProxy.VehicleSteeringSpeed;

	VehicleMovementAction = AnimInstanceProxy.VehicleMovementAction;
	bIsJumping = AnimInstanceProxy.bIsJumping;

	CurrentActiveActionTime += DeltaSeconds;

	if (VehicleMovementAction == ESR_VehicleMovementAction::Dash && (CurrentActiveActionTime > DashAnimDuration))
	{
		VehicleMovementComponent->OnDashEnded();
	}
}

FAnimInstanceProxy* USR_VehicleAnimInstance::CreateAnimInstanceProxy()
{
    return &AnimInstanceProxy;
}

void USR_VehicleAnimInstance::DestroyAnimInstanceProxy(FAnimInstanceProxy* InProxy)
{
}

void USR_VehicleAnimInstance::OnMovementActionChanged(ESR_VehicleMovementAction OldAction, ESR_VehicleMovementAction NewAction)
{
	CurrentActiveActionTime = 0;
}
