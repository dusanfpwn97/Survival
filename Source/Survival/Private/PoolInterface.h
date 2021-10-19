// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PoolInterface.generated.h"

class UPoolManager;
// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPoolInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class IPoolInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	UFUNCTION(BlueprintNativeEvent)
		void Start();
	UFUNCTION()
		virtual void Start_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void Reset();
	UFUNCTION()
		virtual void Reset_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void ReleaseToPool(AActor* Actor);
	UFUNCTION()
		virtual void ReleaseToPool_Implementation(AActor* Actor);

	UFUNCTION(BlueprintNativeEvent)
		void SetSpawner(UObject* Object);
	UFUNCTION()
		virtual void SetSpawner_Implementation(UObject* Object);


};
