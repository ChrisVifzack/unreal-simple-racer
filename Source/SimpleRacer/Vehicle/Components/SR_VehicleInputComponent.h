// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SR_VehicleInputComponent.generated.h"

class ASR_BaseVehicle;

USTRUCT()
struct FSR_VehicleInputConfig
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, Category = "Steering")
	float SteerSensitivity = 2.4;

	UPROPERTY(EditAnywhere, Category = "Steering")
	double CounterteerSensitivity = 5.0;
};

/**
 * This component is responsible to smooth and clamp the input values from the controller before they are applied to the movement component.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMPLERACER_API USR_VehicleInputComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USR_VehicleInputComponent();

	// Returns the component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "VehicleInput")
	static USR_VehicleInputComponent* FindVehicleInputComponent(const AActor* Actor)
	{
		return (Actor ? Actor->FindComponentByClass<USR_VehicleInputComponent>() : nullptr);
	}

protected:
	virtual void BeginPlay() override;

public:
	UFUNCTION()
	void FilterInputValues(const float DeltaTime);

	UFUNCTION()
	void SetSteeringInput(const float InSteeringInput)
	{
		RawSteeringInput = FMath::Clamp(InSteeringInput, -1.f, 1.f);
	}

	UFUNCTION()
	void SetThrottleInput(const float InThrottleInput)
	{
		RawThrottleInput = FMath::Clamp(InThrottleInput, 0.f, 1.f);
	}

	UFUNCTION()
	void SetBrakeInput(const float InBrakeInput)
	{
		RawBrakeInput = FMath::Clamp(InBrakeInput, 0.f, 1.f);
	}

	UFUNCTION()
	float GetSteeringInput(bool bGetRawValue = false) const
	{
		return bGetRawValue ? RawSteeringInput : SmoothSteeringInput;
	}

	UFUNCTION()
	float GetThrottleInput(bool bGetRawValue = false) const
	{
		return RawThrottleInput;
	}

	UFUNCTION()
	float GetBrakeInput(bool bGetRawValue = false) const
	{
		return RawBrakeInput;
	}

protected:
	float RawSteeringInput = 0.f;
	float RawThrottleInput = 0.f;
	float RawBrakeInput = 0.f;

	float SmoothSteeringInput = 0.f;

	UPROPERTY(EditDefaultsOnly)
	FSR_VehicleInputConfig VehicleInputConfig = FSR_VehicleInputConfig();

	UPROPERTY()
	TObjectPtr<ASR_BaseVehicle> OwningVehicle = nullptr;
};
