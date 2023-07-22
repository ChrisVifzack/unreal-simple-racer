// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "PIDController.generated.h"

USTRUCT(BlueprintType)
struct FPIDState
{
	GENERATED_BODY()

	float PreviousError = 0.f;
	float AccumulatedError = 0.f;
};

USTRUCT(BlueprintType)
struct FPIDConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Kp = 0.9f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Ki = 0.001f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Kd = 0.05f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinOutput = -1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MaxOutput = 1.f;
};

/**
 * Basic implementation of a PID controller.
 */
UCLASS(BlueprintType)
class RACINGAI_API UPIDController : public UObject
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static float UpdatePID(const FPIDConfig& PIDConfig, FPIDState& PIDState, float Error, float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	static void ResetPID(FPIDState& PIDState);
};
