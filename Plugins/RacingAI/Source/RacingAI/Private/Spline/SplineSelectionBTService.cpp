// Copyright 2023 Christina Piberger. All rights reserved.


#include "Spline/SplineSelectionBTService.h"

#include "AIController.h"
#include "RacingAI.h"
#include "Spline/SplineActor.h"
#include "Spline/AISplineFollowInterface.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

USplineSelectionBTService::USplineSelectionBTService()
{
	NodeName = "Spline Selection Service";
	bNotifyBecomeRelevant = true;
	bNotifyCeaseRelevant = true;
	bNotifyOnSearch = false;
	bNotifyTick = false;
	bCallTickOnSearchStart = false;
}

void USplineSelectionBTService::InitializeFromAsset(UBehaviorTree& Asset)
{
	Super::InitializeFromAsset(Asset);

	const UBlackboardData* BBAsset = GetBlackboardAsset();
	if (ensure(BBAsset))
	{
		SplineSearchTriggerKey.ResolveSelectedKey(*BBAsset);
	}
}

void USplineSelectionBTService::OnBecomeRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnBecomeRelevant(OwnerComp, NodeMemory);

	UE_LOG(LogRacingAI, Verbose, TEXT("%s::OnBecomeRelevant: %s: %s"), *GetNameSafe(this), *GetNodeName(), *GetNameSafe(OwnerComp.GetAIOwner()))

	UBlackboardComponent* Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard && SplineSearchTriggerKey.IsSet())
	{
		const auto KeyID = SplineSearchTriggerKey.GetSelectedKeyID();
		Blackboard->RegisterObserver(KeyID, this, FOnBlackboardChangeNotification::CreateUObject(this, &ThisClass::OnSplineSearchTriggerKeyValueChanged));
	}

	if (!bIsInitialized)
	{
		TArray<AActor*> Splines;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASplineActor::StaticClass(), Splines);
		AvailableSplines.Empty();
		for (const auto Spline : Splines)
		{
			if (const auto PC_Spline = Cast<ASplineActor>(Spline))
			{
				AvailableSplines.Add(PC_Spline);
			}
		}
		bIsInitialized = true;
	}

	FindAndSetSpline(*Blackboard);
}

void USplineSelectionBTService::OnCeaseRelevant(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	Super::OnCeaseRelevant(OwnerComp, NodeMemory);

	UE_LOG(LogRacingAI, Verbose, TEXT("%s::OnCeaseRelevant: %s: %s"), *GetNameSafe(this), *GetNodeName(), *GetNameSafe(OwnerComp.GetAIOwner()))

	if (const auto Blackboard = OwnerComp.GetBlackboardComponent())
	{
		Blackboard->UnregisterObserversFrom(this);
	}
}

EBlackboardNotificationResult USplineSelectionBTService::OnSplineSearchTriggerKeyValueChanged(
	const UBlackboardComponent& BlackboardComponent, FBlackboard::FKey Key)
{
	UE_LOG(LogRacingAI, Verbose, TEXT("%s::OnSplineSearchTriggerKeyValueChanged: %s"), *GetNameSafe(this), *GetNodeName());

	const bool NewValue = BlackboardComponent.GetValueAsBool(BlackboardComponent.GetKeyName(Key));
	if (NewValue)
	{
		FindAndSetSpline(BlackboardComponent);
	}
	return EBlackboardNotificationResult::ContinueObserving;
}

void USplineSelectionBTService::FindAndSetSpline(const UBlackboardComponent& BlackboardComponent)
{
	UE_LOG(LogRacingAI, Verbose, TEXT("%s::FindAndSetSpline: %s"), *GetNameSafe(this), *GetNodeName());

	auto SplineFollowInterface = Cast<IAISplineFollowInterface>(BlackboardComponent.GetOwner());
	if (!ensure(SplineFollowInterface) || !ensure(!AvailableSplines.IsEmpty())) {
		return;
	}

	SplineFollowInterface->SetSpline(AvailableSplines[NextSplineIndex]);
	NextSplineIndex = NextSplineIndex < AvailableSplines.Num() - 1 ? NextSplineIndex + 1 : 0;
	//NextSplineIndex = UKismetMathLibrary::RandomInteger(AvailableSplines.Num());
}
