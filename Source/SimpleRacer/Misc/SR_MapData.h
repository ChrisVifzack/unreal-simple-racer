// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "UObject/SoftObjectPtr.h"
#include "SR_MapData.generated.h"


USTRUCT(BlueprintType)
struct SIMPLERACER_API FSR_MapDataEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayName = FText::FromString("Name");

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UTexture2D> MacIcon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftObjectPtr<UWorld> MapAsset;
};

UCLASS(BlueprintType)
class SIMPLERACER_API USR_MapData : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FSR_MapDataEntry> MapDataEnties;
};
