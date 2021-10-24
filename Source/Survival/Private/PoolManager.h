// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameDatatypes.h"
#include "PoolInterface.h"
#include "PoolManager.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPoolManager : public UActorComponent, public IPoolInterface
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPoolManager();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		AActor* GetAvailableActor(TSubclassOf<AActor> ActorClass, bool& IsCached);
	UFUNCTION()
		void ReleaseToPool_Implementation(AActor* Actor);
	UFUNCTION()
		TArray<AActor*> GetAllSpawnedActors() const;
private:
	UPROPERTY()
		TMap<TSubclassOf<AActor>, FPooledActors> PooledActorsMap;

	UPROPERTY()
		TArray<AActor*> AllSpawnedActors;
};
