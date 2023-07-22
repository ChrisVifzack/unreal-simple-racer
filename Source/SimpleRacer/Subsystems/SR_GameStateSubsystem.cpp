// Copyright 2023 Christina Piberger. All rights reserved.


#include "SR_GameStateSubsystem.h"
#include "GameFramework/GameStateBase.h"
#include "GameFramework/PlayerState.h"


void USR_GameStateSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	SetGameState(ESR_GameStateEnum::LOADING);
	FGameModeEvents::GameModeInitializedEvent.AddUObject(this, &USR_GameStateSubsystem::OnGameModeInitialized);
}

void USR_GameStateSubsystem::Deinitialize()
{	
}

void USR_GameStateSubsystem::SetGameState(ESR_GameStateEnum NewState)
{
	if (NewState != CurrentGameState)
	{
		ESR_GameStateEnum OldState = CurrentGameState;
		CurrentGameState = NewState;
		OnGameStateChanged(CurrentGameState, OldState);
		OnGameStateChangedDelegate.Broadcast(CurrentGameState, OldState);
	}
}

void USR_GameStateSubsystem::OnGameStateChanged(ESR_GameStateEnum NewState, ESR_GameStateEnum OldState)
{

}


void USR_GameStateSubsystem::OnGameModeInitialized(AGameModeBase* FromGameMode)
{

}

