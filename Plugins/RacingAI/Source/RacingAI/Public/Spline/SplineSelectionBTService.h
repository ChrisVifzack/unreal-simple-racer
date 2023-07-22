// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "SplineSelectionBTService.generated.h"

class ASplineActor;

/**
 * A behavior tree service that determines which spline the AI should follow.
 */
UCLASS()
class RACINGAI_API USplineSelectionBTService : public UBTService
{
	GENERATED_BODY()

public:
	USplineSelectionBTService();

	virtual void InitializeFromAsset(UBehaviorTree& Asset) override;
	virtual void OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
	virtual void OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

	EBlackboardNotificationResult OnSplineSearchTriggerKeyValueChanged(const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key);

	void FindAndSetSpline(const UBlackboardComponent& BlackboardComponent);

protected:
	// Trigger key that will start a new spline search for this instance.
	UPROPERTY(EditAnywhere, Category = Blackboard)
	FBlackboardKeySelector SplineSearchTriggerKey;

	UPROPERTY(Transient)
	TArray<ASplineActor*> AvailableSplines;

	// Indicates if the service has been initialized already.
	bool bIsInitialized = false;

	// The next bot who requests a spline will get the one at this index.
	int32 NextSplineIndex = 0;
};
