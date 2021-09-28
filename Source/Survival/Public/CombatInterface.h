// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatInterface.generated.h"

class ABaseSpell;
class ABaseEnemy;

// This class does not need to be modified.
UINTERFACE(MinimalAPI, Blueprintable)
class UCombatInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SURVIVAL_API ICombatInterface
{
	GENERATED_BODY()

		
	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
		void OnCollidedWithSpell(ABaseSpell* Spell);
	UFUNCTION()
		virtual void OnCollidedWithSpell_Implementation(ABaseSpell* Spell);
	UFUNCTION(BlueprintNativeEvent)
		void OnCollidedWithEnemy(ABaseEnemy* Spell);
	UFUNCTION()
		virtual void OnCollidedWithEnemy_Implementation(ABaseEnemy* Enemy);
	UFUNCTION(BlueprintNativeEvent)
		void SetTarget(AActor* TargetActor);
	UFUNCTION()
		virtual void SetTarget_Implementation(AActor* TargetActor);
	/*UFUNCTION(BlueprintNativeEvent)
		TArray<AActor*> GetSpawnedEnemies();
	UFUNCTION()
		virtual TArray<AActor*> GetSpawnedEnemies_Implementation();
	*/




};
