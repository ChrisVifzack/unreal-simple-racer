// Copyright 2023 Christina Piberger. All rights reserved.


#include "Spline/SplineFollowComponent.h"

#include "RacingAI.h"
#include "Spline/SplineActor.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Utils/MathUtils.h"
#include "Utils/PIDController.h"

USplineFollowComponent::USplineFollowComponent()
{
	PrimaryComponentTick.bStartWithTickEnabled = false;
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bCanEverTick = true;

	bAutoActivate = false;
}

void USplineFollowComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USplineFollowComponent::Activate(bool bReset)
{
	Super::Activate(bReset);

	UpdateSplineParam();
}

void USplineFollowComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Spline)
	{
		if (Blackboard)
		{
			Blackboard->SetValueAsBool(RequestNewSplineKey, true);
		}
		return;
	}

	UpdateErrors(DeltaTime);
	UpdateInputs(DeltaTime);
	ApplyInputs(DeltaTime);

	const auto SplineLength = Spline->GetLength();
	if (SplineLength - CurrentSplineParam <= 5.f * UMathUtils::Meter2Cm) // end of spline check
	{
		// we almost have reached the end of the current spline and need to request new one via the blackboard
		if (Blackboard)
		{
			Blackboard->SetValueAsBool(RequestNewSplineKey, true);
		}
		//UE_LOG(LogRacingAI, Warning, TEXT("%s::TickComponent: End of spline reached!"), *GetNameSafe(this));
	}
}

void USplineFollowComponent::UpdateSplineParam()
{
	if (Spline)
	{
		CurrentSplineParam = Spline->GetClosestParam(GetOwnerLocation());
		LastSplineParam = CurrentSplineParam;
	}
}

void USplineFollowComponent::SetSpline(ASplineActor* NewSpline, UBlackboardComponent* InBlackboard, const FName InRequestNewSplineKey)
{
	Spline = NewSpline;
	CurrentSplineParam = 0.0f;
	LastSplineParam = 0.0f;
	if (InBlackboard)
	{
		Blackboard = InBlackboard;
		RequestNewSplineKey = InRequestNewSplineKey;
		Blackboard->SetValueAsBool(RequestNewSplineKey, false);
	}
}

void USplineFollowComponent::UpdateErrors(float DeltaTime)
{
	CurrentSplineParam = FMath::Max(LastSplineParam, Spline->GetClosestParamInSegment(GetOwnerLocation(),
			LastSplineParam, FMath::Min(LastSplineParam + 35.f * UMathUtils::Meter2Cm, Spline->GetLength())));
	const auto ClosestSplineLocation = Spline->GetPoint(CurrentSplineParam);

	LastSplineParam = CurrentSplineParam;
	if (bDebugDraw)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), Spline->GetPoint(CurrentSplineParam), 15.0f, 12, FColor::Orange, 0, 3.0f);
	}

	// the actual desired/target location should be a bit ahead of current spline param (for smoother steering). the look ahead distance is a function of velocity.
	float TargetParam = CurrentSplineParam;
	const float SpeedKmh = GetOwner()->GetVelocity().Size() * UMathUtils::Cms2Kmh;
	const float LookAheadDistance = UKismetMathLibrary::MapRangeClamped(SpeedKmh, 30.f, 100.f, 2.f, 15.f);
	const FVector TargetLocation = Spline->MoveParam(TargetParam, LookAheadDistance * UMathUtils::Meter2Cm);
	if (bDebugDraw)
	{
		UKismetSystemLibrary::DrawDebugSphere(GetWorld(), TargetLocation, 15.0f, 12, FColor::Red, 0, 3.0f);
	}

	const auto OwnerToTarget = TargetLocation - GetOwnerLocation(); // this is the current location error
	const auto OwnerToTargetDirection = (TargetLocation - GetOwnerLocation()).GetSafeNormal();
	if (bDebugDraw)
	{
		UKismetSystemLibrary::DrawDebugArrow(GetWorld(), GetOwnerLocation(), GetOwnerLocation() + OwnerToTarget, 10, FColor::Red, 0, 3.0f);
	}

	const FVector OwnerDirection = GetOwner()->GetActorForwardVector();
	const FVector OwnerUpVector = GetOwner()->GetActorUpVector();
	const float Sign = FMath::Sign(FVector::DotProduct(FVector::CrossProduct(OwnerDirection, OwnerToTargetDirection), OwnerUpVector));

	const float Angle = UMathUtils::GetSmallestAngleBetweenVectorsInDegrees(
		FVector::VectorPlaneProject(OwnerDirection, OwnerUpVector),
		FVector::VectorPlaneProject(OwnerToTargetDirection, OwnerUpVector));

	LateralAngleAlignmentError = Sign * Angle;

	const auto OwnerToClosestSplineLocation = ClosestSplineLocation - GetOwnerLocation();
	const auto LocalOwnerToTarget = GetOwner()->GetActorTransform().InverseTransformVector(OwnerToClosestSplineLocation) * UMathUtils::Cm2Meter;
	LateralLocationError = LocalOwnerToTarget.Y;
	VerticalLocationError = LocalOwnerToTarget.Z;
}

void USplineFollowComponent::UpdateInputs(float DeltaTime)
{
	LateralCV = UPIDController::UpdatePID(LateralPIDSettings, LateralPIDState, LateralAngleAlignmentError, DeltaTime);
	if (bDebugDraw && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(102, 0.1f, FColor::Green, FString::Printf(TEXT("AI LateralCV: %f"), LateralCV));
	}
}

void USplineFollowComponent::ApplyInputs(float DeltaTime)
{
	// needs to be implemented specific to your pawn.
}
