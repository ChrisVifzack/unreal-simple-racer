// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "SR_BaseVehicle.generated.h"

class USR_AutoDriveComponent;
class USR_AxleComponent;
class USR_VehicleInputComponent;
class USR_VehicleMovementComponent;

UCLASS()
class SIMPLERACER_API ASR_BaseVehicle : public APawn
{
	GENERATED_BODY()

public:
	ASR_BaseVehicle();

protected:
	virtual void BeginPlay() override;

	virtual void PossessedBy(AController* NewController) override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void CalcCamera(float DeltaTime, struct FMinimalViewInfo& OutResult) override;

	UFUNCTION()
	void SetInput(const float SteeringInput, const float ThrottleInput, const float BrakeInput);

	UFUNCTION()
	FVector GetFrontAxleOffset() const;

	UFUNCTION()
	FVector GetRearAxleOffset() const;

	UFUNCTION()
	FVector GetFrontAxleHalfTrack() const;

	UFUNCTION()
	FVector GetRearAxleHalfTrack() const;

protected:
	// Top-level mesh used for collision checks.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> CollisionMeshComponent = nullptr;

	// Place to attach cosmetic components.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USceneComponent> VisibleComponents = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USR_VehicleInputComponent> VehicleInput = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USR_VehicleMovementComponent> VehicleMovement = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USR_AutoDriveComponent> AutoDrive = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USR_AxleComponent> FrontAxle = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<USR_AxleComponent> RearAxle = nullptr;

	// Variable for smooth FOV modifier
	float PrevFOVModifier = 0.f;
};
