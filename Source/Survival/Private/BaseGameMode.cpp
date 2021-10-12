// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseGameMode.h"

#include "Engine/World.h"
#include "EnemySpawner.h"

ABaseGameMode::ABaseGameMode()
{
	PrimaryActorTick.bCanEverTick = true;
	EnemySpawner = CreateDefaultSubobject<UEnemySpawner>(FName(TEXT("EnemySpawner")));
	this->AddOwnedComponent(EnemySpawner);
}

void ABaseGameMode::BeginPlay()
{
	Super::BeginPlay();
}

void ABaseGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


}

UEnemySpawner* ABaseGameMode::GetEnemySpawner() const
{
	return EnemySpawner;
}
