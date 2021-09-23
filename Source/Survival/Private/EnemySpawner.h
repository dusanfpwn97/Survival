// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawner.generated.h"

class ABaseEnemy;
class APawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UEnemySpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemySpawner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<ABaseEnemy> Level1EnemyClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		APawn* PlayerPawn;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
	void SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass);


public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	void UpdatePlayerPawn();
};
