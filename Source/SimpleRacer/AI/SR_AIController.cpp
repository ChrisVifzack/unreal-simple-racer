// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_AIController.h"

#include "Vehicle/SR_BaseVehicle.h"
#include "Spline/SplineActor.h"
#include "Spline/SplineFollowComponent.h"
#include "GameFramework/PlayerState.h"


ASR_AIController::ASR_AIController()
{
	bWantsPlayerState = true;
}

void ASR_AIController::BeginPlay()
{
	Super::BeginPlay();

	if (ensure(PlayerState))
	{
		PlayerState->SetIsABot(true);
	}
}

void ASR_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	UpdateStuckDetection(DeltaTime);
}

void ASR_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
}


void ASR_AIController::UpdateStuckDetection(float DeltaTime)
{
	// TODO: implement
}

void ASR_AIController::Respawn()
{
	// TODO: implement
}


USplineFollowComponent* ASR_AIController::GetSplineFollowComponent()
{
	if (GetPawn())
	{
		return Cast<USplineFollowComponent>(GetPawn()->FindComponentByClass<USplineFollowComponent>());
	}
	return nullptr;
}

void ASR_AIController::SetSpline(ASplineActor* NewSpline)
{
	if (auto SplineFollowComponent = GetSplineFollowComponent())
	{
		SplineFollowComponent->SetSpline(NewSpline, Blackboard, "bWantsNewSpline");
	}
}
