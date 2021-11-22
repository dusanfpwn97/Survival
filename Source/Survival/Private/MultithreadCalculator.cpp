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

void UMultithreadCalculator::InitCalculations(int32 _Calculations)
{
	if (_Calculations > 0)
	{
		CalcThread = new FThreadCalculator(_Calculations, this);
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



void UMultithreadCalculator::PrintCalcData()
{
	if (!ThreadCalcQueue.IsEmpty() && ThreadCalcQueue.Dequeue(ProcessedCalculation))
	{
		UE_LOG(LogTemp, Warning, TEXT("Processed Calculation: %d"), ProcessedCalculation)
	}
}

void UMultithreadCalculator::DebugText()
{
	PrintCalcData();
}
