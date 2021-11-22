// Fill out your copyright notice in the Description page of Project Settings.


#include "ThreadCalculator.h"
#include "MultithreadCalculator.h"


// Sets default values
FThreadCalculator::FThreadCalculator(TArray<FVector> _SpellLocations, TArray<FVector> _EnemyLocations, UMultithreadCalculator* CalculatorComponent)
{
	if (_SpellLocations.Num() > 0 && CalculatorComponent && _EnemyLocations.Num() > 0)
	{
		SpellLocations = _SpellLocations;
		EnemyLocations = _EnemyLocations;
		CurrentMultithreadCalculator = CalculatorComponent;
	}
}

bool FThreadCalculator::Init()
{
	bStopThread = false;

	return true;
}

uint32 FThreadCalculator::Run()
{
	TArray<int32> CollidedIndexes;

	while (!bStopThread)
	{
		if (CurrentMultithreadCalculator)
		{
			//CurrentCalculation.Empty();
			for (int i = 0; i < SpellLocations.Num(); i++)
			{
				for (int j = 0; j < EnemyLocations.Num(); j++)
				{
					const FVector SpellLoc = SpellLocations[i];
					if (SpellLoc.Z < 150.f) // Optimisation for storm type
					{
						float Dist = FVector::Dist(SpellLoc, EnemyLocations[j] + FVector(0.f, 0.f, 100.f));

						if (Dist < 80)
						{
							//OnInstanceCollided(i, j);
							CurrentCalculation.Add(i, j);
							CurrentMultithreadCalculator->ThreadCalcQueue.Enqueue(CurrentCalculation);

						}
					}
				}
			}
			CurrentCalculation.Empty();
		}

		bStopThread = true;

		if (CurrentMultithreadCalculator)
		{
			//CurrentMultithreadCalculator->KillThread();
		}
		/*
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
		}*/
	}

	return 0;
}

void FThreadCalculator::Stop()
{
	bStopThread = true;
}

