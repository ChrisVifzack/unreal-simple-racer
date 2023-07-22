// Copyright 2023 Christina Piberger. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SR_BaseCharacter.generated.h"

// Base class of main character in our game.
UCLASS(Blueprintable)
class SIMPLERACER_API ASR_BaseCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASR_BaseCharacter() {};

	virtual void Tick(float DeltaSeconds);

	UFUNCTION(BlueprintCallable)
	void SetIsRagdoll(bool NewValue)
	{
		if (NewValue != bIsRagdoll) 
		{
			bIsRagdoll = NewValue;
			OnIsRagdollChanged();
		}
	}

protected:
	void OnIsRagdollChanged()
	{
		if (bIsRagdoll) 
		{
			OnRagdollActivated();
		}
		else
		{
			OnRagdollDeactivated();
		}
	}

	void OnRagdollActivated();
	void OnRagdollDeactivated();

	UPROPERTY(BlueprintReadOnly)
	bool bIsRagdoll = false;
};