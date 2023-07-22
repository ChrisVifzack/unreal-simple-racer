// Copyright 2023 Christina Piberger. All rights reserved.

#include "SR_VehicleInputComponent.h"
#include "Vehicle/SR_BaseVehicle.h"


USR_VehicleInputComponent::USR_VehicleInputComponent()
{
    PrimaryComponentTick.bCanEverTick = false;
}

void USR_VehicleInputComponent::BeginPlay()
{
    Super::BeginPlay();
    OwningVehicle = Cast<ASR_BaseVehicle>(GetOwner());
}

void USR_VehicleInputComponent::FilterInputValues(const float DeltaTime)
{
	if (!OwningVehicle)
	{
		SmoothSteeringInput = 0.0f;
		RawSteeringInput = 0.0f;
		return;
	}

	float SteeringInterpSpeed = VehicleInputConfig.SteerSensitivity;
	if (FMath::Sign(SmoothSteeringInput) != FMath::Sign(RawSteeringInput))
	{
		SteeringInterpSpeed = VehicleInputConfig.CounterteerSensitivity;
	}

	SmoothSteeringInput = FMath::FInterpConstantTo(SmoothSteeringInput, RawSteeringInput, DeltaTime, SteeringInterpSpeed);
	SmoothSteeringInput = FMath::Clamp(SmoothSteeringInput, -1.0f, 1.0f);
}
