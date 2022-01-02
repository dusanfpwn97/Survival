// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGM.h"
#include "EnemySpawner.h"
#include "PickupSpawner.h"

ASurvivalGM::ASurvivalGM()
{
	EnemySpawnerComp = CreateDefaultSubobject<UEnemySpawner>(FName(TEXT("EnemySpawnerComp")));
	this->AddOwnedComponent(EnemySpawnerComp);

	PickupSpawnerComp = CreateDefaultSubobject<UPickupSpawner>(FName(TEXT("PickupSpawnerComp")));
	this->AddOwnedComponent(PickupSpawnerComp);
}