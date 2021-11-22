// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ThreadCalculator.h"
#include "HAL/Runnable.h"
#include "HAL/RunnableThread.h"
#include "MultithreadCalculator.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UMultithreadCalculator : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMultithreadCalculator();

	TQueue<TMap<int32, int32>> ThreadCalcQueue;

	virtual void EndPlay(EEndPlayReason::Type EndplayReason) override;

	void InitCalculations(TArray<FVector> _SpellLocations, TArray<FVector> _EnemyLocations);

	TMap<int32, int32> GetCalcData();

	void KillThread();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	TMap<int32, int32> ProcessedCalculationCollisions;

	class FThreadCalculator* CalcThread = nullptr;

	FRunnableThread* CurrentRunningThread = nullptr;

	FTimerHandle DebugTimerHandle;

	void DebugText();

};
