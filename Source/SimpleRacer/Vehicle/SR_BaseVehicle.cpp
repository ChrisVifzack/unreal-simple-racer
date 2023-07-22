// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_BaseVehicle.h"

#include "Components/SR_AutoDriveComponent.h"
#include "Components/SR_AxleComponent.h"
#include "Components/SR_VehicleInputComponent.h"
#include "Components/SR_VehicleMovementComponent.h"
#include <Kismet/KismetMathLibrary.h>


ASR_BaseVehicle::ASR_BaseVehicle()
{	
	PrimaryActorTick.bCanEverTick = true;

	CollisionMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Collision Mesh"));
	SetRootComponent(CollisionMeshComponent);

	VisibleComponents = CreateDefaultSubobject<USceneComponent>(TEXT("Visible Components"));
	VisibleComponents->SetupAttachment(RootComponent);

	VehicleInput = CreateDefaultSubobject<USR_VehicleInputComponent>(TEXT("Vehicle Input Component"));
	VehicleMovement = CreateDefaultSubobject<USR_VehicleMovementComponent>(TEXT("Vehicle Movement Component"));
	AutoDrive = CreateDefaultSubobject<USR_AutoDriveComponent>(TEXT("AutoDrive Component"));

	FrontAxle = CreateDefaultSubobject<USR_AxleComponent>(TEXT("Front Axle"));
	FrontAxle->SetAxleOffset(FVector::ForwardVector * 100.0);

	RearAxle = CreateDefaultSubobject<USR_AxleComponent>(TEXT("Rear Axle"));
	RearAxle->SetAxleOffset(FVector::BackwardVector * 100.0);
}

void ASR_BaseVehicle::BeginPlay()
{
	Super::BeginPlay();
}

void ASR_BaseVehicle::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	if (VehicleMovement && IsBotControlled())
	{
		VehicleMovement->bDebug = false;
	}
}

void ASR_BaseVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (VehicleInput)
	{
		VehicleInput->FilterInputValues(DeltaTime);
	}

	// set control rotation for springarm
	if (GetController())
	{
		float MaxRotationOffset = 45;
		float RotationOffset = MaxRotationOffset * VehicleInput->GetSteeringInput();

		auto NewRotation = FMath::RInterpTo(GetControlRotation(), GetActorRotation() + FRotator(0, RotationOffset, 0), DeltaTime, 1.f);
		GetController()->SetControlRotation(NewRotation);
	}
}

void ASR_BaseVehicle::CalcCamera(float DeltaTime, FMinimalViewInfo& OutResult)
{
	Super::CalcCamera(DeltaTime, OutResult);

	float CameraYaw = GetActorRotation().Yaw;

	if (VehicleMovement)
	{
		auto Speed = VehicleMovement->GetLocalVelocity().X;
		auto FOVModifier = UKismetMathLibrary::MapRangeClamped(Speed, 50.f, 1800.f, -10.f, 20.f);
		FOVModifier = FMath::FInterpTo(PrevFOVModifier, FOVModifier, DeltaTime, 2.f);
		PrevFOVModifier = FOVModifier;

		OutResult.FOV += FOVModifier;
	}
}

void ASR_BaseVehicle::SetInput(const float SteeringInput, const float ThrottleInput, const float BrakeInput)
{
	if (VehicleInput)
	{
		VehicleInput->SetSteeringInput(SteeringInput);
		VehicleInput->SetThrottleInput(ThrottleInput);
		VehicleInput->SetBrakeInput(BrakeInput);
	}
}

FVector ASR_BaseVehicle::GetFrontAxleOffset() const
{
	if (IsValid(FrontAxle))
	{
		return FrontAxle->GetAxleOffset();
	}

	return FVector::ForwardVector * 100.0;
}

FVector ASR_BaseVehicle::GetRearAxleOffset() const
{
	if (IsValid(RearAxle))
	{
		return RearAxle->GetAxleOffset();
	}

	return FVector::BackwardVector * 100.0;
}

FVector ASR_BaseVehicle::GetFrontAxleHalfTrack() const
{
	if (IsValid(FrontAxle))
	{
		return FrontAxle->GetAxleHalfTrack();
	}

	return FVector::RightVector * 100.0;
}

FVector ASR_BaseVehicle::GetRearAxleHalfTrack() const
{
	if (IsValid(RearAxle))
	{
		return RearAxle->GetAxleHalfTrack();
	}

	return FVector::RightVector * 100.0;
}
