// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "SR_AxleComponent.generated.h"

/**
 *
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIMPLERACER_API USR_AxleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	USR_AxleComponent();

protected:
	virtual void BeginPlay() override;

public:
	void SetAxleOffset(const FVector& InAxleOffset);
	FVector GetAxleOffset() const;
	FVector GetAxleHalfTrack() const;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FVector AxleOffset = FVector::ZeroVector;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float AxleWidth = 100.f;
};
