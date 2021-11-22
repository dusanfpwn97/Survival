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

	TQueue<int32> ThreadCalcQueue;

	virtual void EndPlay(EEndPlayReason::Type EndplayReason) override;

	UFUNCTION(BlueprintCallable)
	void InitCalculations(int32 _Calculations);


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void PrintCalcData();
	int32 ProcessedCalculation;

	class FThreadCalculator* CalcThread = nullptr;

	FRunnableThread* CurrentRunningThread = nullptr;

	FTimerHandle DebugTimerHandle;

	void DebugText();

};
