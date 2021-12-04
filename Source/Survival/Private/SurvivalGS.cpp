// Fill out your copyright notice in the Description page of Project Settings.


#include "SurvivalGS.h"
#include "EnemySpawner.h"


ASurvivalGS::ASurvivalGS()
{
	EnemySpawnerComp = CreateDefaultSubobject<UEnemySpawner>(FName(TEXT("EnemySpawnerComp")));
	this->AddOwnedComponent(EnemySpawnerComp);
}