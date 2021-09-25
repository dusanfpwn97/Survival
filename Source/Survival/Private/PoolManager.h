// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameDatatypes.h"
#include "PoolManager.generated.h"

class IPoolInterface;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UPoolManager : public UActorComponent
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
		int32 RemoveFromPool(TSubclassOf<AActor> ActorClass);

	UFUNCTION()
		void ReleaseActorToPool(AActor* Actor);

	UFUNCTION()
		void RegisterNewActor(AActor* Actor);

private:
	UPROPERTY()
		TMap<TSubclassOf<AActor>, FPooledActors> PooledActors;

};
