// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SR_RespawnInterface.h"
#include "GameFramework/PlayerController.h"
#include "SR_IngamePlayerController.generated.h"

struct FInputActionValue;
class UInputMappingContext;
class USR_InputConfig;


UCLASS()
class SIMPLERACER_API ASR_IngamePlayerController : public APlayerController, public ISR_RespawnInterface
{
	GENERATED_BODY()

public:
	ASR_IngamePlayerController();

protected:
	virtual void BeginPlay() override;

public:
	virtual void OnPossess(APawn* InPawn) override;

	virtual void Tick(float DeltaTime) override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	virtual void SetupInputComponent() override;

	/* Respawn Interface - Start */
	UFUNCTION(BlueprintCallable)
	virtual void Respawn(const FPC_RespawnInfo& RespawnInfo) override;

	UFUNCTION()
	virtual FOnRespawn& GetRespawnDelegate() override
	{
		return OnRespawn;
	}

	UPROPERTY(BlueprintAssignable)
	FOnRespawn OnRespawn;
	/* Respawn Interface - End */

protected:
	UFUNCTION()
	void ProcessSteeringInput(const FInputActionValue& Value);

	UFUNCTION()
	void ProcessThrottleInput(const FInputActionValue& Value);

	UFUNCTION()
	void ProcessBrakeInput(const FInputActionValue& Value);

	void ProcessInputFloatValue(const FInputActionValue& InValue, float& OutValue);
	void PassProcessedInputValues();

	// Maps keys/buttons to input actions
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UInputMappingContext> InputMapping = nullptr;

	// Contains input actions used to setup bindings 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USR_InputConfig> InputConfig = nullptr;

	float SteeringInputValue = 0.0f;
	float ThrottleInputValue = 0.0f;
	float BrakeInputValue = 0.0f;
};