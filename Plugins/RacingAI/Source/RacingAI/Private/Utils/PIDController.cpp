// Copyright 2023 Christina Piberger. All rights reserved.

#include "Utils/PIDController.h"

float UPIDController::UpdatePID(const FPIDConfig& PIDConfig, FPIDState& PIDState, float Error, float DeltaSeconds)
{
	if (!ensure(DeltaSeconds > 0.f))
	{
		return 0.f;
	}

	float CV = 0.f; // control variable

	// Proportional term
	CV += PIDConfig.Kp * Error;

	// Integral term
	PIDState.AccumulatedError += Error * DeltaSeconds;
	CV += PIDConfig.Ki * PIDState.AccumulatedError;

	// Derivative term
	const float ErrorRate = (Error - PIDState.PreviousError) / DeltaSeconds;
	CV += PIDConfig.Kd * ErrorRate;
	PIDState.PreviousError = Error;

	return FMath::Clamp(CV, PIDConfig.MinOutput, PIDConfig.MaxOutput);
}

void UPIDController::ResetPID(FPIDState& PIDState)
{
	PIDState.PreviousError = 0.f;
	PIDState.AccumulatedError = 0.f;
}
