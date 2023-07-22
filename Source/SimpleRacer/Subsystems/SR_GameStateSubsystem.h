// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SR_GameStateSubsystem.generated.h"


UENUM(BlueprintType)
enum class ESR_GameStateEnum : uint8
{
	INVALID = 0,
	LOADING,
	PREPARATION,
	PLAYING,
	PAUSED,
	ENDED
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameStateEnumChanged, ESR_GameStateEnum, NewState, ESR_GameStateEnum, OldState);


/**
 *
 */
UCLASS()
class SIMPLERACER_API USR_GameStateSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintPure)
	ESR_GameStateEnum GetCurrentGameState() const
	{
		return CurrentGameState;
	}

	UFUNCTION(BlueprintCallable)
	void SetGameState(ESR_GameStateEnum NewState);

	UPROPERTY(BlueprintAssignable)
	FOnGameStateEnumChanged OnGameStateChangedDelegate;

protected:
	UFUNCTION()
	void OnGameModeInitialized(AGameModeBase* FromGameMode);

	void OnGameStateChanged(ESR_GameStateEnum NewState, ESR_GameStateEnum OldState);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ESR_GameStateEnum CurrentGameState = ESR_GameStateEnum::INVALID;
};
