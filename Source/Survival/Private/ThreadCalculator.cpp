// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadCalculator.h"
#include "MultithreadCalculator.h"


// Sets default values
FThreadCalculator::FThreadCalculator(int32 _CalculationNum, UMultithreadCalculator* CalculatorComponent)
{
	if (_CalculationNum > 0 && CalculatorComponent)
	{
		Calculations = _CalculationNum;
		CurrentMultithreadCalculator = CalculatorComponent;
	}
}

bool FThreadCalculator::Init()
{
	bStopThread = false;
	CalcCount = 0;

	return true;
}

uint32 FThreadCalculator::Run()
{
	while (!bStopThread)
	{
		if (CalcCount < Calculations && CurrentMultithreadCalculator)
		{
			CurrentCalculation += FMath::RandRange(20, 300);
			CurrentCalculation *= FMath::RandRange(2, 3);
			CurrentCalculation -= FMath::RandRange(10, 50);

			CurrentMultithreadCalculator->ThreadCalcQueue.Enqueue(CurrentCalculation);

			CalcCount++;
		}
		else
		{
			bStopThread = true;
		}
	}

	return 0;
}

void FThreadCalculator::Stop()
{
	bStopThread = true;
}

