// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemySpawner.generated.h"

class UPoolManager;
class ABaseEnemy;
class ABasePlayerPawn;
class AFakeShadowDistributer;


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), BlueprintType )
class UEnemySpawner : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemySpawner();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSoftClassPtr<ABaseEnemy> SoftLevel1EnemyClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ABasePlayerPawn* PlayerPawn;
	UPROPERTY()
		UPoolManager* EnemySpawnPoolManager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool IsSpawnEnabled = true;

		void SpawnEnemy();

		void DebugValues();
		FTimerHandle DebugTimerHandle;

	TArray<AActor*> CachedAliveSpawns;

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


	TArray<AActor*> GetAllSpawns();

	TArray<AActor*> GetAliveSpawns();
	//UFUNCTION()
		//void ReleaseToPool_Implementation(AActor* Actor) override;
	//UFUNCTION()
		//TArray<AActor*> GetSpawnedEnemies_Implementation() override;
private:
	void UpdatePlayerPawn();


};
