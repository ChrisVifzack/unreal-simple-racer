// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MathUtils.generated.h"

/**
 * Frequently needed constants and functions. 
 */
UCLASS(BlueprintType)
class UMathUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	static constexpr float Ms2Kmh = 3.6f;
	static constexpr float Kmh2Ms = 0.277778f;
	static constexpr float Meter2Cm = 100.0f;
	static constexpr float Cm2Meter = 0.01f;

	// Kilometers per hour -> Centimeters per second
	static constexpr float Kmh2Cms = 27.7777777777778f;
	static constexpr float Cms2Kmh = 0.036f;

	static float RandomBinomial()
	{
		return (FMath::RandRange(-1, 1) - FMath::RandRange(-1, 1)) * 0.5f;
	}

	static float GetSmallestAngleBetweenVectorsInDegrees(const FVector& Vec1, const FVector& Vec2)
	{
		float Angle = 0.0f;
		FVector nAxis;
		FQuat BetweenQuat = FQuat::FindBetweenVectors(Vec1, Vec2);
		BetweenQuat.ToAxisAndAngle(nAxis, Angle);
		return FMath::RadiansToDegrees(Angle);
	}
};
