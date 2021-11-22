// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/Public/HAL/Runnable.h"

class FRunnableThread;
class UMultithreadCalculator;


class FThreadCalculator : public FRunnable
{

public:	
	// Sets default values for this actor's properties
	FThreadCalculator(TArray<FVector> _SpellLocations, TArray<FVector> _EnemyLocations, UMultithreadCalculator* CalculatorComponent);

	virtual bool Init();

	virtual uint32 Run();
	virtual void Stop();

	bool bStopThread = true;


private:

	UMultithreadCalculator* CurrentMultithreadCalculator;

	TMap<int32, int32> CurrentCalculation;

	TArray<FVector> SpellLocations;
	TArray<FVector> EnemyLocations;


};
