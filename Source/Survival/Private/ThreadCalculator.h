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
	FThreadCalculator(int32 _CalculationNum, UMultithreadCalculator* CalculatorComponent);

	virtual bool Init();

	virtual uint32 Run();
	virtual void Stop();

	bool bStopThread = true;
private:

	int32 Calculations;

	int32 CalcCount;

	UMultithreadCalculator* CurrentMultithreadCalculator;

	int32 CurrentCalculation;
};
