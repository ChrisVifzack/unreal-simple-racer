// Copyright 2023 Christina Piberger. All rights reserved.


#include "Spline/SplineActor.h"

#include "Runtime/Engine/Classes/Components/SplineComponent.h"
#include "Runtime/Engine/Public/DrawDebugHelpers.h"
#include "Math/Box.h"
#include "Utils/MathUtils.h"


ASplineActor::ASplineActor()
	: SplineComp(nullptr)
	, bDebug(false)
	, DebugColor(FColor::Blue)
	, DebugThickness(3.0f)
	, DebugIterator(3.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("Spline Component"));
	SplineComp->SetClosedLoop(false);
	SetRootComponent(SplineComp);
}

float ASplineActor::GetClosestParam(FVector p)
{
	if (!IsValid(SplineComp)) {
		return 0.0f;
	}

	const float inputKey = SplineComp->FindInputKeyClosestToWorldLocation(p);
	const int pointIndex = static_cast<int32>(inputKey);

	float lowDistBound = SplineComp->GetDistanceAlongSplineAtSplinePoint(pointIndex);
	float highDistBound = SplineComp->GetDistanceAlongSplineAtSplinePoint(pointIndex + 1);
	float middleDistEstimate = (lowDistBound + highDistBound) * 0.5f;

	const FInterpCurveFloat& distToInputMapping = SplineComp->SplineCurves.ReparamTable;
	for (int i = 0; i < 10; ++i) {
		const float middleInputKey = distToInputMapping.Eval(middleDistEstimate);

		if (inputKey < middleInputKey)
			highDistBound = middleDistEstimate;
		else
			lowDistBound = middleDistEstimate;
		middleDistEstimate = (lowDistBound + highDistBound) * 0.5f;
	}
	return middleDistEstimate;
}

float ASplineActor::GetClosestParamInSegment(FVector p, float StartDistance, float EndDistance)
{
	if (!IsValid(SplineComp)) {
		return 0.0f;
	}

	const FVector LocalLocation = SplineComp->GetComponentTransform().InverseTransformPosition(p);
	StartDistance = FMath::Clamp(StartDistance, 0.0f, SplineComp->GetSplineLength());
	EndDistance = FMath::Clamp(EndDistance, StartDistance, SplineComp->GetSplineLength());

	if (SplineComp->SplineCurves.Position.Points.Num() > 1) {
		const float StartKey = SplineComp->SplineCurves.ReparamTable.Eval(StartDistance, 0.0f);
		const auto StartIdx = SplineComp->GetSplinePointsPosition().GetPointIndexForInputValue(StartKey);
		const float EndKey = SplineComp->SplineCurves.ReparamTable.Eval(EndDistance, 0.0f);
		const auto EndIdx = FMath::Min(SplineComp->GetSplinePointsPosition().GetPointIndexForInputValue(EndKey),
			SplineComp->SplineCurves.Position.Points.Num() - 2);

		float BestDistanceSq;
		float BestResult = SplineComp->SplineCurves.Position.
			InaccurateFindNearestOnSegment(LocalLocation, StartIdx, BestDistanceSq);
		int32 BestSegment = StartIdx;
		for (int32 Segment = StartIdx + 1; Segment <= EndIdx; ++Segment) {
			float LocalDistanceSq;
			const float LocalResult = SplineComp->SplineCurves.Position.InaccurateFindNearestOnSegment(LocalLocation, Segment,
				LocalDistanceSq);
			if (LocalDistanceSq < BestDistanceSq) {
				BestDistanceSq = LocalDistanceSq;
				BestResult = LocalResult;
				BestSegment = Segment;
			}
		}

		const int32 pointIndex = BestSegment;

		float lowDistBound = SplineComp->GetDistanceAlongSplineAtSplinePoint(pointIndex);
		float highDistBound = SplineComp->GetDistanceAlongSplineAtSplinePoint(pointIndex + 1);
		float middleDistEstimate = (lowDistBound + highDistBound) * 0.5f;

		const FInterpCurveFloat& distToInputMapping = SplineComp->SplineCurves.ReparamTable;
		for (int i = 0; i < 10; ++i) {
			const float middleInputKey = distToInputMapping.Eval(middleDistEstimate);

			if (BestResult < middleInputKey)
				highDistBound = middleDistEstimate;
			else
				lowDistBound = middleDistEstimate;
			middleDistEstimate = (lowDistBound + highDistBound) * 0.5f;
		}
		return middleDistEstimate;
	}
	return 0.0f;
}

float ASplineActor::GetClosestStartParam(FVector p)
{
	return GetClosestParamInSegment(p, 0.0f, GetLength());
}

float ASplineActor::GetClosestStartParam(FVector p, const FVector& Direction)
{
	return GetClosestParamInSegmentWithDirection(p, 0.0f, GetLength(), Direction);
}

float ASplineActor::GetClosestParamInSegmentWithDirection(FVector p, float StartDistance, float EndDistance,
	const FVector& Direction)
{
	if (!IsValid(SplineComp)) {
		return 0.0f;
	}

	const FVector LocalLocation = SplineComp->GetComponentTransform().InverseTransformPosition(p);
	StartDistance = FMath::Clamp(StartDistance, 0.0f, static_cast<float>(SplineComp->GetSplineLength()));
	EndDistance = FMath::Clamp(EndDistance, StartDistance, static_cast<float>(SplineComp->GetSplineLength()));

	if (SplineComp->SplineCurves.Position.Points.Num() > 1) {
		const float StartKey = SplineComp->SplineCurves.ReparamTable.Eval(StartDistance, 0.0f);
		const auto StartIdx = SplineComp->GetSplinePointsPosition().GetPointIndexForInputValue(StartKey);
		const auto EndKey = SplineComp->SplineCurves.ReparamTable.Eval(EndDistance, 0.0f);
		const auto EndIdx = SplineComp->GetSplinePointsPosition().GetPointIndexForInputValue(EndKey);

		float BestDistanceSq;
		float BestResult = SplineComp->SplineCurves.Position.
			InaccurateFindNearestOnSegment(LocalLocation, StartIdx, BestDistanceSq);
		int32 BestSegment = 0;
		for (int32 Segment = StartIdx + 1; Segment < EndIdx; ++Segment) {
			const auto& SegmentDirection = SplineComp->SplineCurves.Position.EvalDerivative(Segment, FVector::ZeroVector).
				GetSafeNormal();

			if (FVector::DotProduct(Direction, SegmentDirection) <= 0) {
				continue;
			}
			float LocalDistanceSq;
			const float LocalResult = SplineComp->SplineCurves.Position.InaccurateFindNearestOnSegment(LocalLocation, Segment,
				LocalDistanceSq);
			if (LocalDistanceSq < BestDistanceSq) {
				BestDistanceSq = LocalDistanceSq;
				BestResult = LocalResult;
				BestSegment = Segment;
			}
		}

		const int32 pointIndex = BestSegment;

		float lowDistBound = SplineComp->GetDistanceAlongSplineAtSplinePoint(pointIndex);
		float highDistBound = SplineComp->GetDistanceAlongSplineAtSplinePoint(pointIndex + 1);
		float middleDistEstimate = (lowDistBound + highDistBound) * 0.5f;

		const FInterpCurveFloat& distToInputMapping = SplineComp->SplineCurves.ReparamTable;
		for (int i = 0; i < 10; ++i) {
			const float middleInputKey = distToInputMapping.Eval(middleDistEstimate);

			if (BestResult < middleInputKey)
				highDistBound = middleDistEstimate;
			else
				lowDistBound = middleDistEstimate;
			middleDistEstimate = (lowDistBound + highDistBound) * 0.5f;
		}
		return middleDistEstimate;
	}
	return 0.0f;
}

float ASplineActor::GetLength() const
{
	if (!IsValid(SplineComp)) {
		return 0.0f;
	}

	return SplineComp->GetSplineLength();
}

FVector ASplineActor::MoveParam(float& p, float distance)
{
	if (!IsValid(SplineComp)) {
		return FVector::ZeroVector;
	}

	p = FMath::Fmod(p + distance, SplineComp->GetSplineLength());
	return SplineComp->GetLocationAtDistanceAlongSpline(p, ESplineCoordinateSpace::World);
}

FVector ASplineActor::GetPoint(float p)
{
	if (!IsValid(SplineComp)) {
		return FVector::ZeroVector;
	}

	return SplineComp->GetLocationAtDistanceAlongSpline(p, ESplineCoordinateSpace::World);
}

FRotator ASplineActor::GetRotation(float p)
{
	if (!IsValid(SplineComp)) {
		return FRotator::ZeroRotator;
	}

	return SplineComp->GetRotationAtDistanceAlongSpline(p, ESplineCoordinateSpace::World);
}

FVector ASplineActor::GetTangent(float p)
{
	if (!IsValid(SplineComp)) {
		return FVector::ZeroVector;
	}

	return SplineComp->GetTangentAtDistanceAlongSpline(p, ESplineCoordinateSpace::World);
}

FVector ASplineActor::GetDirection(float p)
{
	if (!IsValid(SplineComp)) {
		return FVector::ZeroVector;
	}

	return SplineComp->GetDirectionAtDistanceAlongSpline(p, ESplineCoordinateSpace::World);
}

bool ASplineActor::IsInRange(const FVector& Position, float Range)
{
	const auto WorldBounds = LocalBounds.TransformBy(GetActorTransform());
	if (WorldBounds.GetBox().IsInside(Position)) {
		return true;
	}
	return WorldBounds.ComputeSquaredDistanceFromBoxToPoint(Position) < FMath::Square(Range);
}

void ASplineActor::BeginPlay()
{
	Super::BeginPlay();

	UpdateLocalBounds();

	if (bDebug) {
		BuildDebugLine();
	}
}

void ASplineActor::SetDebug(bool bEnable)
{
	bDebug = bEnable;
	if (bDebug) {
		BuildDebugLine();
	}
}

void ASplineActor::AddSplineWorldPoint(const FVector& Position)
{
	if (!IsValid(SplineComp)) {
		return;
	}

	SplineComp->AddSplinePoint(Position, ESplineCoordinateSpace::World);

	const float InKey = SplineComp->SplineCurves.Position.Points.Num()
		? SplineComp->SplineCurves.Position.Points.Last().InVal
		: 0.0f;
}

void ASplineActor::UpdateLocalBounds(bool bFastCalculation)
{
	if (!IsValid(SplineComp)) {
		return;
	}

	if (bFastCalculation) {
		// Copy from USplineComponent::CalcBounds
		FBox BoundingBox(EForceInit::ForceInit);
		for (const auto& InterpPoint : SplineComp->SplineCurves.Position.Points) {
			BoundingBox += InterpPoint.OutVal;
		}
	}
	else {
		FBox BoundingBox(EForceInit::ForceInit);
		const auto len = SplineComp->GetSplineLength();
		for (float i = 0.0f; i < len; i += 1.0f * UMathUtils::Meter2Cm) {
			BoundingBox += SplineComp->GetLocationAtDistanceAlongSpline(i, ESplineCoordinateSpace::Local);
		}

		LocalBounds = FBoxSphereBounds(BoundingBox);
	}
}

void ASplineActor::BuildDebugLine()
{
	if (!IsValid(SplineComp)) {
		return;
	}

	float len = SplineComp->GetSplineLength();

	const auto WorldBounds = LocalBounds.TransformBy(GetActorTransform());
	DrawDebugBox(GetWorld(), WorldBounds.Origin, WorldBounds.BoxExtent, FColor::Blue, true);

	const FTransform LastTransform = SplineComp->GetTransformAtDistanceAlongSpline(0.0f, ESplineCoordinateSpace::World, true);
	FVector LastCenter = LastTransform.GetLocation();
	const FVector LastRight = LastTransform.GetRotation().GetRightVector() * LastTransform.GetScale3D().Y;
	FVector LastP0 = LastCenter - LastRight;
	FVector LastP1 = LastCenter + LastRight;

	for (float i = 0.0f; i < len; i += DebugIterator * UMathUtils::Meter2Cm) {
		const FTransform Transform = SplineComp->GetTransformAtDistanceAlongSpline(i, ESplineCoordinateSpace::World, true);
		FVector Center = Transform.GetLocation();
		const FVector Right = Transform.GetRotation().GetRightVector() * Transform.GetScale3D().Y;
		FVector P0 = Center - Right;
		FVector P1 = Center + Right;

		FColor Color = FColor::Black;

		DrawDebugLine(GetWorld(), LastCenter, Center, Color, true, -1.0f, 0, DebugThickness);
		DrawDebugLine(GetWorld(), LastP0, P0, Color, true, -1.0f, 0, DebugThickness);
		DrawDebugLine(GetWorld(), LastP1, P1, Color, true, -1.0f, 0, DebugThickness);
		DrawDebugLine(GetWorld(), P0, P1, Color, true, -1.0f, 0, DebugThickness);

		LastCenter = Center;
		LastP0 = P0;
		LastP1 = P1;
	}
}

void ASplineActor::RemoveIdx(int32 i)
{
	if (!IsValid(SplineComp)) {
		return;
	}

	if (SplineComp->GetNumberOfSplinePoints() > i) {
		SplineComp->RemoveSplinePoint(i);
	}

	SplineComp->UpdateSpline();
}
