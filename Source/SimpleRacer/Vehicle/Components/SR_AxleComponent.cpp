// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_AxleComponent.h"


USR_AxleComponent::USR_AxleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void USR_AxleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void USR_AxleComponent::SetAxleOffset(const FVector& InAxleOffset)
{
	AxleOffset = InAxleOffset;
}

FVector USR_AxleComponent::GetAxleOffset() const
{
	return AxleOffset;
}

FVector USR_AxleComponent::GetAxleHalfTrack() const
{
	return FVector::RightVector * AxleWidth * 0.5f;
}
