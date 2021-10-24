// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawner.generated.h"

class UPoolManager;
class ABaseEnemy;
class APawn;
class AFakeShadowDistributer;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Blueprintable )
class UEnemySpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemySpawner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<ABaseEnemy> SoftLevel1EnemyClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		APawn* PlayerPawn;
	UPROPERTY()
		UPoolManager* EnemySpawnPoolManager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool IsSpawnEnabled = true;

	UFUNCTION()
		void SpawnEnemy();


protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UFUNCTION()
		void SpawnFakeShadowDistributer();

	UPROPERTY()
		FTimerHandle CommonEnemySpawnTimer;
	UPROPERTY()
		AFakeShadowDistributer* FakeShadowDistributer;
	uint32 spawnnum = 0;
	//UPROPERTY()
		//TMap<TSubclassOf<ABaseEnemy>, TArray<ABaseEnemy*>> Map; // Future use
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	UFUNCTION()
		TArray<AActor*> GetAllSpawns();
	//UFUNCTION()
		//void ReleaseToPool_Implementation(AActor* Actor) override;
	//UFUNCTION()
		//TArray<AActor*> GetSpawnedEnemies_Implementation() override;
private:
	void UpdatePlayerPawn();


};
