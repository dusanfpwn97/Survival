// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseGameMode.generated.h"

class UEnemySpawner;

/**
 * 
 */
UCLASS(Blueprintable)
class ABaseGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	ABaseGameMode();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UEnemySpawner* EnemySpawner;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
