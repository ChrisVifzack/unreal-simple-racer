// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "InputAction.h"
#include "Engine/DataAsset.h"
#include "SR_InputConfig.generated.h"

/**
 * Data asset that contains input actions.
 */
UCLASS(BlueprintType, Const)
class SIMPLERACER_API USR_InputConfig : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputSteering = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputThrottle = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UInputAction* InputBrake = nullptr;
};