// Fill out your copyright notice in the Description page of Project Settings.


#include "MultithreadCalculator.h"


// Sets default values for this component's properties
UMultithreadCalculator::UMultithreadCalculator()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}


void UMultithreadCalculator::EndPlay(EEndPlayReason::Type EndplayReason)
{
	Super::EndPlay(EndplayReason);

	KillThread();
}

void UMultithreadCalculator::InitCalculations(TArray<FVector> _SpellLocations, TArray<FVector> _EnemyLocations)
{

	if (_SpellLocations.Num() > 0 && _EnemyLocations.Num() > 0)
	{
		KillThread();
		CalcThread = new FThreadCalculator(_SpellLocations, _EnemyLocations, this);
		CurrentRunningThread = FRunnableThread::Create(CalcThread, TEXT("Calculation Thread"));
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("Calculations must be > 0"));
	}
}

// Called when the game starts
void UMultithreadCalculator::BeginPlay()
{
	Super::BeginPlay();

	// ...
	UWorld* World = GetWorld();
	if (World)
	{
		World->GetTimerManager().SetTimer(DebugTimerHandle, this, &UMultithreadCalculator::DebugText, 0.1f, true);
	}
}

TMap<int32, int32> UMultithreadCalculator::GetCalcData()
{
	if (!ThreadCalcQueue.IsEmpty() && ThreadCalcQueue.Dequeue(ProcessedCalculationCollisions))
	{
		return ProcessedCalculationCollisions;
	}

	return TMap<int32, int32>();
}

void UMultithreadCalculator::DebugText()
{
	//PrintCalcData();
}

void UMultithreadCalculator::KillThread()
{
	if (CurrentRunningThread && CalcThread)
	{
		CurrentRunningThread->Suspend(true);
		CalcThread->bStopThread = true;
		CurrentRunningThread->Suspend(false);
		CurrentRunningThread->Kill(false);
		CurrentRunningThread->WaitForCompletion();
		delete CalcThread;
	}
}
