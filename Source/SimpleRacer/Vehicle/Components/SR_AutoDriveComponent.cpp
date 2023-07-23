// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_AutoDriveComponent.h"
#include "Vehicle/SR_BaseVehicle.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Spline/SplineActor.h"
#include "Utils/MathUtils.h"


USR_AutoDriveComponent::USR_AutoDriveComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;

    bAutoActivate = false;
}

void USR_AutoDriveComponent::BeginPlay()
{
    Super::BeginPlay();

    Vehicle = Cast<ASR_BaseVehicle>(GetOwner());
}

void USR_AutoDriveComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void USR_AutoDriveComponent::UpdateErrors(float DeltaTime)
{
	Super::UpdateErrors(DeltaTime);

	// speed control (aka "cruise control") is done by separate PID controller that regulates throttle based on speed.
	// look ahead distance is a function of velocity.
	const float SpeedKmh = GetOwner()->GetVelocity().Size() * UMathUtils::Cms2Kmh;
	const float LookAheadDistance = UKismetMathLibrary::MapRangeClamped(SpeedKmh, 20.f, MaxTargetSpeed, 5.f, 15.f);

	float TargetParam = CurrentSplineParam;
	const FVector TargetLocation = Spline->MoveParam(TargetParam, LookAheadDistance * UMathUtils::Meter2Cm);
	if (bDebugDraw)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), TargetLocation, 25.0f, 12, FColor::Cyan, 0, 3.0f);
	}

	const FVector SplineDirection = Spline->GetDirection(TargetParam);
	const FVector OwnerDirection = GetOwner()->GetActorForwardVector();
	const FVector OwnerUpVector = GetOwner()->GetActorUpVector();
	const float AngleError = UMathUtils::GetSmallestAngleBetweenVectorsInDegrees(
		FVector::VectorPlaneProject(OwnerDirection, OwnerUpVector),
		FVector::VectorPlaneProject(SplineDirection, OwnerUpVector));

	float TargetSpeed = UKismetMathLibrary::MapRangeClamped(FMath::Abs(AngleError), 10.f, 80.f, MaxTargetSpeed, 10.f);

	//if (bDebugDraw && GEngine)
	//{
	//	GEngine->AddOnScreenDebugMessage(112, 0.1f, FColor::Green, FString::Printf(TEXT("AI TargetSpeed: %2.f"), TargetSpeed));
	//	GEngine->AddOnScreenDebugMessage(113, 0.1f, FColor::Green, FString::Printf(TEXT("AI AngleError: %2.1f"), AngleError));
	//}
	
	const FVector LocalVelocity = GetOwner()->GetActorTransform().InverseTransformVector(Vehicle->GetVelocity()) * UMathUtils::Cms2Kmh;

	SpeedError = TargetSpeed - LocalVelocity.X;
}

void USR_AutoDriveComponent::UpdateInputs(float DeltaTime)
{
	Super::UpdateInputs(DeltaTime);

	SteeringInputValue = LateralCV;

	const auto SpeedCV = UPIDController::UpdatePID(SpeedPIDConfig, SpeedPIDState, SpeedError, DeltaTime);
	if (bDebugDraw && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(114, 0.1f, FColor::Green, FString::Printf(TEXT("AI SpeedCV: %.2f"), SpeedCV));
	}
	ThrottleInputValue = (SpeedCV > 0.f) ? SpeedCV : 0.f;
	BrakeInputValue = (SpeedCV < -0.7f) ? -SpeedCV : 0.f;
}

void USR_AutoDriveComponent::ApplyInputs(float DeltaTime)
{
	if (ensure(Vehicle))
	{
		Vehicle->SetInput(SteeringInputValue, ThrottleInputValue, BrakeInputValue);
	}
}
