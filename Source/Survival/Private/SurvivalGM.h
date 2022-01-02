// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SurvivalGM.generated.h"

class UEnemySpawner;
class UPickupSpawner;



UCLASS()
class ASurvivalGM : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ASurvivalGM();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UEnemySpawner* EnemySpawnerComp;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		UPickupSpawner* PickupSpawnerComp;

};
