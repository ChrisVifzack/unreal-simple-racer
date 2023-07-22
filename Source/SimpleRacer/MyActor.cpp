

#include "MyActor.h"


AMyActor::AMyActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

void AMyActor::BeginPlay()
{
	Super::BeginPlay();	
}

void AMyActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// this function will ONLY be called if the BP does not implement the event
void AMyActor::MyBlueprintNativeEvent_Implementation()
{
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("MyBlueprintNativeEvent_Implementation (C++) called!"));
	}
	UE_LOG(LogTemp, Warning, TEXT("MyBlueprintNativeEvent_Implementation (C++) called!")); // logging a "Warning" will make text appear in yellow font
}

void AMyActor::BroadcastDelegate()
{
	MyMulticastDelegate.Broadcast();
}

// this is a CallInEditor function
void AMyActor::CallBlueprintNativeEvent()
{
	MyBlueprintNativeEvent();
}

// this is a CallInEditor function
void AMyActor::CallBlueprintImplementableEvent()
{
	MyBlueprintImplementableEvent();
}
