// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utils/PIDController.h"
#include "SplineFollowComponent.generated.h"

class ASplineActor;
class UBlackboardComponent;

/**
 * Component handles spline following and other automatic behavior.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class RACINGAI_API USplineFollowComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USplineFollowComponent();

protected:
	virtual void BeginPlay() override;

	virtual void Activate(bool bReset) override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void UpdateSplineParam();

	UFUNCTION(BlueprintCallable)
	virtual void SetSpline(ASplineActor* NewSpline, UBlackboardComponent* InBlackboard = nullptr, const FName InRequestNewSplineKey = NAME_None);

	UFUNCTION(BlueprintCallable)
	ASplineActor* GetSpline()
	{
		return Spline;
	}

	UFUNCTION(BlueprintCallable)
	virtual FVector GetOwnerLocation()
	{
		return GetOwner()->GetActorLocation();
	}

protected:
	// Update error based on current location and desired location on spline.
	virtual void UpdateErrors(float DeltaTime);

	virtual void UpdateInputs(float DeltaTime);

	virtual void ApplyInputs(float DeltaTime);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bDebugDraw = false;

	// The associated blackboard component (relevant if AI controls spline selection)
	UPROPERTY(Transient)
	UBlackboardComponent* Blackboard = nullptr;

	// The blackboard key used to request new splines (relevant if AI controls spline selection)
	UPROPERTY(Transient)
	FName RequestNewSplineKey = NAME_None;

	// The spline to follow.
	UPROPERTY(Transient)
	ASplineActor* Spline = nullptr;

	// parameters to keep track of where we currently are on spline
	float CurrentSplineParam = 0.0f;
	float LastSplineParam = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = PID)
	FPIDConfig LateralPIDSettings;

	UPROPERTY(BlueprintReadOnly, Category = PID)
	FPIDState LateralPIDState;

	// In most cases the lateral control variable is "steering input"
	float LateralCV = 0.f;

	// current angular error (used to update PID controller)
	float LateralAngleAlignmentError = 0.0f;

	// current local y distance to spline
	float LateralLocationError = 0.0f;

	// current local z distance to spline
	float VerticalLocationError = 0.0f;
};
