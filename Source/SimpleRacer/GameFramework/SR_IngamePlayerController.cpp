// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_IngamePlayerController.h"

#include "SimpleRacer.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "Input/SR_InputConfig.h"
#include "Vehicle/SR_BaseVehicle.h"


ASR_IngamePlayerController::ASR_IngamePlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ASR_IngamePlayerController::BeginPlay()
{
	Super::BeginPlay();
}

void ASR_IngamePlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}

void ASR_IngamePlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	PassProcessedInputValues();
}

void ASR_IngamePlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	Super::PostProcessInput(DeltaTime, bGamePaused);
	//PassProcessedInputValues();
}

void ASR_IngamePlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	if (!IsValid(InputMapping))
	{
		UE_LOG(LogSimpleRacer, Error, TEXT("InputMapping is not valid, unable to setup input."));
		return;
	}

	if (!IsValid(InputConfig))
	{
		UE_LOG(LogSimpleRacer, Error, TEXT("InputConfig is not valid, unable to setup input."));
		return;
	}

	// Get the local player subsystem
	UEnhancedInputLocalPlayerSubsystem* EnhancedInputSubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());

	if (!IsValid(EnhancedInputSubsystem))
	{
		UE_LOG(LogSimpleRacer, Error, TEXT("EnhancedInputSubsystem is not valid, unable to setup input."));
		return;
	}

	EnhancedInputSubsystem->ClearAllMappings();
	EnhancedInputSubsystem->AddMappingContext(InputMapping, 0);

	UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(InputComponent);

	if (!IsValid(EnhancedInputComponent))
	{
		UE_LOG(LogSimpleRacer, Error, TEXT("EnhancedInputComponent is not valid, unable to setup input."));
		return;
	}

	EnhancedInputComponent->BindAction(InputConfig->InputSteering, ETriggerEvent::Triggered, this, &ThisClass::ProcessSteeringInput);
	EnhancedInputComponent->BindAction(InputConfig->InputSteering, ETriggerEvent::Completed, this, &ThisClass::ProcessSteeringInput);

	EnhancedInputComponent->BindAction(InputConfig->InputThrottle, ETriggerEvent::Triggered, this, &ThisClass::ProcessThrottleInput);
	EnhancedInputComponent->BindAction(InputConfig->InputThrottle, ETriggerEvent::Completed, this, &ThisClass::ProcessThrottleInput);

	EnhancedInputComponent->BindAction(InputConfig->InputBrake, ETriggerEvent::Triggered, this, &ThisClass::ProcessBrakeInput);
	EnhancedInputComponent->BindAction(InputConfig->InputBrake, ETriggerEvent::Completed, this, &ThisClass::ProcessBrakeInput);

}

void ASR_IngamePlayerController::Respawn(const FPC_RespawnInfo& RespawnInfo)
{
	if (GetPawn())
	{
		GetPawn()->SetActorLocationAndRotation(RespawnInfo.Location, RespawnInfo.Rotation, false, nullptr, ETeleportType::ResetPhysics);
		OnRespawn.Broadcast(this);
	}
}

void ASR_IngamePlayerController::ProcessSteeringInput(const FInputActionValue& Value)
{
	ProcessInputFloatValue(Value, SteeringInputValue);
}

void ASR_IngamePlayerController::ProcessThrottleInput(const FInputActionValue& Value)
{
	ProcessInputFloatValue(Value, ThrottleInputValue);
}

void ASR_IngamePlayerController::ProcessBrakeInput(const FInputActionValue& Value)
{
	ProcessInputFloatValue(Value, BrakeInputValue);
}

void ASR_IngamePlayerController::ProcessInputFloatValue(const FInputActionValue& InValue, float& OutValue)
{
	OutValue = InValue.Get<float>();
	PassProcessedInputValues();
}

void ASR_IngamePlayerController::PassProcessedInputValues()
{
	if (auto Vehicle = Cast<ASR_BaseVehicle>(GetPawn()))
	{
		Vehicle->SetInput(SteeringInputValue, ThrottleInputValue, BrakeInputValue);
	}
}
