// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PoolInterface.h"
#include "EnemySpawner.generated.h"

class UPoolManager;
class ABaseEnemy;
class APawn;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UEnemySpawner : public UActorComponent, public IPoolInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemySpawner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		TSubclassOf<ABaseEnemy> Level1EnemyClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		APawn* PlayerPawn;
	UPROPERTY()
		UPoolManager* PoolManager;

	UPROPERTY()
		TArray<AActor*> EnemyPool;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	UFUNCTION()
	void SpawnEnemy(TSubclassOf<ABaseEnemy> EnemyClass);


	//UPROPERTY()
		//TMap<TSubclassOf<ABaseEnemy>, TArray<ABaseEnemy*>> Map; // Future use
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION()
		void ReleaseToPool_Implementation(AActor* Actor) override;

private:
	void UpdatePlayerPawn();
};
