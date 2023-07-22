// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SplineComponent.h"
#include "GameFramework/Actor.h"
#include "SplineActor.generated.h"

class USplineComponent;

/**
 * An actor with a spline component and some helper functions. Not specific to any project.
 */
UCLASS()
class RACINGAI_API ASplineActor : public AActor
{
	GENERATED_BODY()

public:
	ASplineActor();

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	float GetClosestParam(FVector p);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	float GetClosestParamInSegment(FVector p, float StartDistance, float EndDistance);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	float GetClosestParamInSegmentWithDirection(FVector p, float StartDistance, float EndDistance, const FVector& Direction);

	virtual float GetClosestStartParam(FVector p);
	virtual float GetClosestStartParam(FVector p, const FVector& Direction);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	FVector MoveParam(float& p, float distance);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	FVector GetPoint(float p);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	FRotator GetRotation(float p);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	FVector GetTangent(float p);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	FVector GetDirection(float p);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	float GetLength() const;

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	USplineComponent* GetSplineComponent()
	{
		return SplineComp;
	}

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	void SetDebug(bool bEnable);

	UFUNCTION(BlueprintCallable, Category = "RacingAI")
	void AddSplineWorldPoint(const FVector& Position);

	virtual void RemoveIdx(int32 i);

	bool IsInRange(const FVector& Position, float Range);

	void UpdateLocalBounds(bool bFastCalculation = true);

	UPROPERTY()
	FBoxSphereBounds LocalBounds;

	virtual void BuildDebugLine();

protected:
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Spline", DisplayName = "Spline")
	USplineComponent* SplineComp = nullptr;

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, Category = "Spline", DisplayName = "Debug")
	bool bDebug;
	UPROPERTY(EditAnywhere, Category = "Spline", DisplayName = "Debug Color", meta = (EditCondition = "bDebug"))
	FColor DebugColor;
	UPROPERTY(EditAnywhere, Category = "Spline", DisplayName = "Debug Thickness", meta = (EditCondition = "bDebug"))
	float DebugThickness;
	UPROPERTY(EditAnywhere, Category = "Spline", DisplayName = "Debug Iterator", meta = (EditCondition = "bDebug"))
	float DebugIterator;

	UPROPERTY()
	uint8 SplineType = 0;

private:
#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& Event) override;
#endif
};

#if WITH_EDITOR
inline void ASplineActor::PostEditChangeProperty(FPropertyChangedEvent& Event)
{
	if (Event.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ASplineActor, bDebug)) {
		if (Event.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ASplineActor, bDebug)) {
			SetDebug(bDebug);
		}
	}
	Super::PostEditChangeProperty(Event);
}
#endif
