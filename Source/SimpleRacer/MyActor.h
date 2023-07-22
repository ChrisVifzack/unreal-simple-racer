
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyActor.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FMyMulticastDelegateType);

/**
* An example actor class as a reference for students.
*/
UCLASS(Blueprintable)
class AMyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Constructor sets default values for this actor's properties
	AMyActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(CallInEditor)
	void BroadcastDelegate();

	UFUNCTION(CallInEditor)
	void CallBlueprintNativeEvent();

	UFUNCTION(CallInEditor)
	void CallBlueprintImplementableEvent();

protected:
	// cannot be implemented in c++; can be implemented in BP
	UFUNCTION(BlueprintImplementableEvent)
	void MyBlueprintImplementableEvent();

	// cannot be implemented in c++; but generates MyBlueprintNativeEvent_Implementation()
	// the C++ implementation will ONLY be called if the BP does not implement the event
	UFUNCTION(BlueprintNativeEvent)
	void MyBlueprintNativeEvent(); 

	UFUNCTION(BlueprintCallable)
	bool MyBlueprintCallable() { return true; }

	UFUNCTION(BlueprintPure)
	bool MyBlueprintPure() { return true; }

	UPROPERTY(BlueprintAssignable)
	FMyMulticastDelegateType MyMulticastDelegate;

	UPROPERTY()
	TObjectPtr<AActor> MyActorPtr = nullptr;

	UPROPERTY(EditDefaultsOnly)
	TArray<int> MyArray;

	UPROPERTY(EditDefaultsOnly)
	TSet<int> MySet;

	UPROPERTY(EditDefaultsOnly)
	TMap<int, FName> MyMap;

	/** Flags with Edit property specifier */
	UPROPERTY(EditAnywhere)
	bool MyEditAnywhereFlag = false;

	UPROPERTY(EditDefaultsOnly)
	bool MyEditDefaultsOnlyFlag = false;

	UPROPERTY(EditInstanceOnly)
	bool MyEditInstanceOnlyFlag = false;

	/** Flags with Visible property specifier */
	UPROPERTY(VisibleAnywhere)
	bool MyVisibleAnywhereFlag = false;

	UPROPERTY(VisibleDefaultsOnly)
	bool MyVisibleDefaultsOnlyFlag = false;

	UPROPERTY(VisibleInstanceOnly)
	bool MyVisibleInstanceOnlyFlag = false;

	/** Flags with Blueprint property specifier */
	UPROPERTY(BlueprintReadOnly)
	bool MyBlueprintReadOnlyFlag = false;

	UPROPERTY(BlueprintReadWrite)
	bool MyBlueprintReadWriteFlag = false;
};
