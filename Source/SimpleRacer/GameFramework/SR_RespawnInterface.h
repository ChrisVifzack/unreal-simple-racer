// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SR_RespawnInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnRespawn, AController*, Controller);

USTRUCT(BlueprintType)
struct FPC_RespawnInfo
{
	GENERATED_BODY()

public:
	// The location to respawn
	UPROPERTY(BlueprintReadWrite)
	FVector Location = FVector::ZeroVector;

	// The location to respawn
	UPROPERTY(BlueprintReadWrite)
	FRotator Rotation = FRotator::ZeroRotator;
};

UINTERFACE(BlueprintType, MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class USR_RespawnInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Interface that adds respawning functionality to a class.
 */
class SIMPLERACER_API ISR_RespawnInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual void Respawn(const FPC_RespawnInfo& RespawnInfo) = 0;

	UFUNCTION()
	virtual FOnRespawn& GetRespawnDelegate() = 0;
};
