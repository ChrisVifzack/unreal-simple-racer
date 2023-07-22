// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPtr.h"
#include "SR_MapData.h"
#include "SR_GameData.generated.h"


/**
 * Non-mutable primary data asset that contains global game data.
 */
UCLASS(BlueprintType, Const)
class SIMPLERACER_API USR_GameData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	USR_GameData();

	// Returns the loaded game data.
	static const USR_GameData& Get();

	// Available Maps
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftObjectPtr<USR_MapData> AvailableMaps;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<TSoftObjectPtr<UWorld>> Levels;
};
