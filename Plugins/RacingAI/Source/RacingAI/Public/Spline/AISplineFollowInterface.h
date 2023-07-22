// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/Interface.h"
#include "SplineActor.h"
#include "AISplineFollowInterface.generated.h"

class USplineFollowComponent;


/**
 * Interface to set Spline on the controlled pawn
 */
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class UAISplineFollowInterface : public UInterface
{
	GENERATED_BODY()
};


class RACINGAI_API IAISplineFollowInterface
{
	GENERATED_BODY()

public:
	virtual USplineFollowComponent* GetSplineFollowComponent() = 0;

	UFUNCTION(BlueprintCallable)
	virtual void SetSpline(ASplineActor* NewSpline) = 0;
};

