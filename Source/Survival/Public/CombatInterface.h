// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"

#include "CombatInterface.generated.h"

class ABaseSpell;
class ABaseEnemy;
class USpellManager;

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

	UFUNCTION(BlueprintNativeEvent)
		FVector GetSpellCastLocation();
	UFUNCTION()
		virtual FVector GetSpellCastLocation_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		void SetSpellManager(UBaseSpellManager* SpellManager);
	UFUNCTION()
		virtual void SetSpellManager_Implementation(UBaseSpellManager* SpellManager);

	UFUNCTION(BlueprintNativeEvent)
		bool GetIsAlive();
	UFUNCTION()
		virtual bool GetIsAlive_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		TArray<AActor*> GetAliveEnemies();
	UFUNCTION()
		virtual TArray<AActor*> GetAliveEnemies_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		AActor* GetClosestEnemy();
	UFUNCTION()
		virtual AActor* GetClosestEnemy_Implementation();


};
