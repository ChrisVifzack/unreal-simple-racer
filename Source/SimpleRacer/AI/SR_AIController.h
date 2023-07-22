// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Spline/AISplineFollowInterface.h"
#include "SR_AIController.generated.h"

class ASplineActor;
class ASR_BaseVehicle;
class ASR_BaseCharacter;

/**
 * Base AI controller for this project.
 */
UCLASS()
class SIMPLERACER_API ASR_AIController : public AAIController, public IAISplineFollowInterface
{
	GENERATED_BODY()

public:
	ASR_AIController();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnPossess(APawn* InPawn) override;

	// Detect if bot got stuck to respawn if needed.
	void UpdateStuckDetection(float DeltaTime);

public:
	UFUNCTION(BlueprintCallable)
	void Respawn();

protected:
	/* IAISplineFollowInterface begin */
	virtual USplineFollowComponent* GetSplineFollowComponent() override;
	virtual void SetSpline(ASplineActor* NewSpline) override;
	/* IAISplineFollowInterface end */
};
