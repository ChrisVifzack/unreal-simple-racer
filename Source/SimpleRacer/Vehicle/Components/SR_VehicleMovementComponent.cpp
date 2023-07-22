// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_VehicleMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Vehicle/SR_BaseVehicle.h"
#include "Vehicle/Components/SR_VehicleInputComponent.h"


USR_VehicleMovementComponent::USR_VehicleMovementComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	// TODO: use async physics tick for physic calculations rather than game-thread tick
	//SetAsyncPhysicsTickEnabled(true);
}

void USR_VehicleMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	const auto* OwningVehicle = Cast<ASR_BaseVehicle>(GetOwner());
	if (!ensure(OwningVehicle))
	{
		return;
	}

	VehicleInput = OwningVehicle->FindComponentByClass<USR_VehicleInputComponent>();
	
	const FVector FrontAxleOffset = OwningVehicle->GetFrontAxleOffset();
	const FVector FrontAxleTrack = OwningVehicle->GetFrontAxleHalfTrack();
	const FVector RearAxleOffset = OwningVehicle->GetRearAxleOffset();
	const FVector RearAxleTrack = OwningVehicle->GetRearAxleHalfTrack();

	AxleOriginFL = FrontAxleOffset - FrontAxleTrack;
	AxleOriginFR = FrontAxleOffset + FrontAxleTrack;
	AxleOriginBL = RearAxleOffset - RearAxleTrack;
	AxleOriginBR = RearAxleOffset + RearAxleTrack;

	AxleDistance = FMath::Abs(FrontAxleOffset.X - RearAxleOffset.X);

	TireTraceQueryParams.AddIgnoredActor(GetOwner());
	TireTraceQueryParams.bTraceComplex = true;
}

void USR_VehicleMovementComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(JumpCooldownTimerHandle);
	}

	Super::EndPlay(EndPlayReason);
}

void USR_VehicleMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TODO: move all movement calculation into async physics tick
	TickMovement(DeltaTime);
}

void USR_VehicleMovementComponent::AsyncPhysicsTickComponent(float delta, float sim)
{
	Super::AsyncPhysicsTickComponent(delta, sim);
}

void USR_VehicleMovementComponent::TickMovement(float DeltaTime)
{
	if (!VehicleInput)
	{
		return;
	}

	const AController* Controller = PawnOwner->GetController();
	if (Controller && Controller->IsLocalController())
	{
		UpdateGroundDetection();
		if (bIsGrounded)
		{
			//UKismetSystemLibrary::DrawDebugArrow(this, GetOwner()->GetActorLocation(), GetOwner()->GetActorLocation() + 200 * GroundNormal, 10, FLinearColor::Blue, 0.1, 5.f);
			const auto NewRotation = FRotationMatrix::MakeFromZX(GroundNormal, GetOwner()->GetActorForwardVector()).ToQuat();
			const FQuat SmoothAlignRotation = FQuat::Slerp(GetOwner()->GetActorQuat(), NewRotation, 5 * DeltaTime);
			GetOwner()->SetActorRotation(SmoothAlignRotation);
		}
		if (bIsGrounded && (MovementAction == ESR_VehicleMovementAction::Jump))
		{
			SetMovementAction(ESR_VehicleMovementAction::None);
		}

		ApplySuspension(DeltaTime);
		ApplyGravity(DeltaTime);

		ApplySteering(DeltaTime, VehicleInput->GetSteeringInput());
		ApplyThrust(DeltaTime, VehicleInput->GetThrottleInput(), VehicleInput->GetSteeringInput());
		ApplyEngineBrake(DeltaTime, VehicleInput->GetThrottleInput(), VehicleInput->GetBrakeInput());
		ApplyBrake(DeltaTime, VehicleInput->GetBrakeInput());

		if (GEngine && bDebug)
		{
			GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Orange, FString::Printf(TEXT("Throttle: %f"), VehicleInput->GetThrottleInput()));
			GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Orange, FString::Printf(TEXT("Brake: %f"), VehicleInput->GetBrakeInput()));
			GEngine->AddOnScreenDebugMessage(3, 0.1f, FColor::Orange, FString::Printf(TEXT("Steering: %f"), VehicleInput->GetSteeringInput()));
			GEngine->AddOnScreenDebugMessage(4, 0.1f, FColor::Blue, FString::Printf(TEXT("Velocity (Size): %f"), Velocity.Size() * 0.036));
			GEngine->AddOnScreenDebugMessage(5, 0.1f, FColor::Yellow, FString::Printf(TEXT("Movement Action: %s"), *UEnum::GetValueAsString(MovementAction)));
		}

		// Move actor
		FVector OldLocation = UpdatedComponent->GetComponentLocation();
		FVector DeltaLocation = Velocity * DeltaTime;
		if (!DeltaLocation.IsNearlyZero(1e-6f))
		{
			const FQuat Rotation = UpdatedComponent->GetComponentQuat();

			FHitResult Hit(1.f);
			SafeMoveUpdatedComponent(DeltaLocation, Rotation, true, Hit);

			if (Hit.IsValidBlockingHit())
			{
				HandleImpact(Hit, DeltaTime, DeltaLocation);
				// Try to slide the remaining distance along the surface.
				SlideAlongSurface(DeltaLocation, 1.f - Hit.Time, Hit.Normal, Hit, true);
			}
		}

		Velocity = (UpdatedComponent->GetComponentLocation() - OldLocation) / DeltaTime;
		UpdateComponentVelocity();

		ApplyLateralDamping(DeltaTime);
		ApplySpeedLimit();
	}
}

void USR_VehicleMovementComponent::ApplyGravity(const float DeltaTime)
{
	float GravityMultiplier = 1.f;
	if (MovementAction == ESR_VehicleMovementAction::Jump)
	{
		// special treatment when close to jump apex
		if (GetLocalVelocity().Z > 0 && GetLocalVelocity().Z < 100)
		{
			GravityMultiplier = 0.5f;
		}
		else
		{
			GravityMultiplier = 3.f;
		}
	}
	Velocity.Z -= GravityMultiplier * VehicleMovementConfig.GravityZ * DeltaTime;
}

void USR_VehicleMovementComponent::ApplySuspension(const float DeltaTime)
{
	// A spring-damper system to keep the vehicle above the ground
	if (bIsGrounded)
	{
		float SpringRate = 200; 
		float DampingRate = 10; 

		float Error = DesiredGroundDistance - GroundDistance;
		float ErrorRate = -(Error - PrevSuspensionError) / DeltaTime;
		PrevSuspensionError = Error;

		const float SpringForce = SpringRate * Error;
		const float DampingForce = DampingRate * ErrorRate;
		const float Force = SpringForce - DampingForce;

		Velocity += GetOwner()->GetActorUpVector() * Force * DeltaTime;
	}
}

void USR_VehicleMovementComponent::ApplyThrust(const float DeltaTime, const float ThrottleInput, const float SteeringInput)
{
	const FVector VehicleWorldForward = GetOwner()->GetActorTransform().TransformVector(FVector::ForwardVector);

	const double ForwardThrustInput = ThrottleInput; //+FMath::Abs(SteeringInput * ThrottleInput) * 0.5;
	const FVector ForwardThrust = VehicleWorldForward * VehicleMovementConfig.EngineThrust * ForwardThrustInput;

	Velocity += ForwardThrust * DeltaTime;
}

void USR_VehicleMovementComponent::ApplySteering(const float DeltaTime, const float SteeringInput)
{
	if (MovementAction == ESR_VehicleMovementAction::Dash)
	{
		return;
	}
	float ForwardVelocity = GetLocalVelocity().X;		
	auto SteeringAngle = VehicleMovementConfig.MaxSteeringAngle * SteeringInput;
	auto DeltaYaw = (FMath::Tan(FMath::DegreesToRadians(SteeringAngle)) / AxleDistance) * ForwardVelocity * DeltaTime;

	if (GEngine && bDebug)
	{
		GEngine->AddOnScreenDebugMessage(6, 0.1f, FColor::Magenta, FString::Printf(TEXT("DeltaYaw: %f"), FMath::RadiansToDegrees(DeltaYaw)));
	}

	auto NewForwardVector = GetOwner()->GetActorForwardVector().RotateAngleAxis(FMath::RadiansToDegrees(DeltaYaw), GetOwner()->GetActorUpVector());

	const auto NewRotation = FRotationMatrix::MakeFromZX(GetOwner()->GetActorUpVector(), NewForwardVector).ToQuat();
	const FQuat SmoothAlignRotation = FQuat::Slerp(GetOwner()->GetActorQuat(), NewRotation, 0.5f);
	GetOwner()->SetActorRotation(SmoothAlignRotation);
}

void USR_VehicleMovementComponent::ApplyBrake(const float DeltaTime, float BrakeInput)
{
	const FVector VehicleWorldForward = GetOwner()->GetActorTransform().TransformVector(FVector::ForwardVector);
	const auto Brake = VehicleWorldForward * VehicleMovementConfig.BrakeForce * BrakeInput;
	Velocity -= Brake * DeltaTime;
}

void USR_VehicleMovementComponent::ApplyEngineBrake(const float DeltaTime, float ThrottleInput, const float BrakeInput)
{
	const double ForwardVelocity = GetLocalVelocity().X;
	if ((FMath::IsNearlyZero(ThrottleInput, KINDA_SMALL_NUMBER) && ForwardVelocity > 0.f) ||
		(FMath::IsNearlyZero(BrakeInput, KINDA_SMALL_NUMBER) && ForwardVelocity < 0.f))
	{
		const float Direction2 = (ForwardVelocity > 0.f) ? -1.f : 1.f;
		const float Direction = -FMath::Sign(ForwardVelocity);
		const FVector VehicleWorldForward = GetOwner()->GetActorTransform().TransformVector(FVector::ForwardVector);
		const double EngineBrakeMultiplier = FMath::Min(1.0, FMath::Abs(ForwardVelocity));
		const FVector EngineBrake = Direction * VehicleWorldForward * EngineBrakeMultiplier * VehicleMovementConfig.EngineBrake;
		Velocity += EngineBrake * DeltaTime;
	}	
}

void USR_VehicleMovementComponent::ApplyLateralDamping(const float DeltaTime)
{
	// Nullify any lateral velocity to prevent vehicle from moving sideways.
	auto RightVelocity = Velocity.ProjectOnTo(GetOwner()->GetActorRightVector());
	Velocity -= RightVelocity;
}

void USR_VehicleMovementComponent::ApplySpeedLimit()
{
	if (MovementAction != ESR_VehicleMovementAction::None)
	{
		return;
	}

	FVector HorizontalVelocity = FVector::VectorPlaneProject(Velocity, GetOwner()->GetActorUpVector());
	FVector VerticalVelocity = Velocity.ProjectOnToNormal(GetOwner()->GetActorUpVector());
	auto Direction = HorizontalVelocity.GetSafeNormal();
	auto Speed = HorizontalVelocity.Size();
	if (Speed > VehicleMovementConfig.MaxSpeed)
	{
		Velocity = Direction * VehicleMovementConfig.MaxSpeed + VerticalVelocity;
	}	
}

FVector USR_VehicleMovementComponent::GetLocalVelocity() const
{
	if (GetOwner())
	{
		if (Velocity.GetSafeNormal().IsNearlyZero())
		{
			return FVector::ZeroVector;
		}
		const FVector LocalVelocity = GetOwner()->GetActorTransform().InverseTransformVector(Velocity);
		return LocalVelocity;
	}
	return FVector::ZeroVector;
}

void USR_VehicleMovementComponent::UpdateGroundDetection()
{
	if (IsJumpInCooldown)
	{
		return;
	}

	const FTransform& VehicleTransform = GetOwner()->GetActorTransform();
	if (VehicleTransform.ContainsNaN())
	{
		return;
	}
	const FVector WorldAxleOriginFL = VehicleTransform.TransformPosition(AxleOriginFL);
	const FVector WorldAxleOriginFR = VehicleTransform.TransformPosition(AxleOriginFR);
	const FVector WorldAxleOriginBL = VehicleTransform.TransformPosition(AxleOriginBL);
	const FVector WorldAxleOriginBR = VehicleTransform.TransformPosition(AxleOriginBR);

	TArray<FVector> Locations = { WorldAxleOriginFL, WorldAxleOriginFR, WorldAxleOriginBL, WorldAxleOriginBR };

	bIsGrounded = false;
	TArray<FVector> Normals;
	TArray<float> Distances;
	for (int32 i = 0; i < 4; ++i)
	{
		FHitResult Hit;
		Hit = DetectGround(Locations[i]);
		if (Hit.bBlockingHit)
		{
			bIsGrounded = true;
			Normals.Add(Hit.Normal);
			Distances.Add(Hit.Distance);
		}
	}

	GroundNormal = UKismetMathLibrary::GetVectorArrayAverage(Normals);
	GroundNormal = GroundNormal.GetSafeNormal();
	if (GroundNormal.IsNearlyZero(KINDA_SMALL_NUMBER))
	{
		GroundNormal = FVector::UpVector;
	}

	GroundDistance = FMath::Min(Distances);
}

FHitResult USR_VehicleMovementComponent::DetectGround(const FVector& Location)
{
	FHitResult HitResult;
	if (GetWorld() && GetOwner())
	{
		const FVector TraceStart = Location + GetOwner()->GetActorUpVector() * 50;
		const FVector TraceEnd = TraceStart - GetOwner()->GetActorUpVector() * (50 + DesiredGroundDistance + 10);
		FCollisionShape SweepSphere = FCollisionShape::MakeSphere(10.f);

		GetWorld()->SweepSingleByChannel(HitResult, TraceStart, TraceEnd, FQuat::Identity, ECC_Visibility, SweepSphere,
			TireTraceQueryParams);
	}
	return HitResult;
}

bool USR_VehicleMovementComponent::TryJump()
{
	if (!IsJumpInCooldown && bIsGrounded)
	{
		Jump();
		IsJumpInCooldown = true;
		if (GetWorld())
		{
			GetWorld()->GetTimerManager().SetTimer(JumpCooldownTimerHandle, this, &ThisClass::ResetJumpCooldown, VehicleMovementConfig.JumpCooldown);
		}
		return true;
	}
	return false;
}

bool USR_VehicleMovementComponent::TryDash()
{
	if (MovementAction == ESR_VehicleMovementAction::None)
	{
		Dash();
		return true;
	}
	return false;
}

void USR_VehicleMovementComponent::Jump()
{
	Velocity.Z = VehicleMovementConfig.JumpForce;
	bIsGrounded = false;
	SetMovementAction(ESR_VehicleMovementAction::Jump);
}

void USR_VehicleMovementComponent::Dash()
{
	FVector DashDirection = FVector::VectorPlaneProject(GetOwner()->GetActorForwardVector(), FVector::UpVector);
	Velocity += DashDirection * VehicleMovementConfig.DashForce;
	SetMovementAction(ESR_VehicleMovementAction::Dash);
}
