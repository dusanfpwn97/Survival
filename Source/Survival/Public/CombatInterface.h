// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "SpellDatatypes.h"
#include "CombatInterface.generated.h"

class ABaseSpellManager;

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
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

	virtual void OnCollidedWithSpell(ABaseSpellManager* Spell, SpellModifier SpellModifierThatDamaged);

	virtual void SetTarget(AActor* TargetActor);

	virtual FVector GetSpellCastLocation();

	virtual bool GetIsAlive();

	virtual TArray<AActor*> GetAliveEnemies();

	virtual AActor* GetClosestEnemy();

	virtual TArray<AActor*> GetClosestEnemies(int32 NumOfEnemies);

	virtual void OnHit(float Damage);
	virtual void OnDeath();



	/*
	
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

	//UFUNCTION(BlueprintNativeEvent)
		//virtual bool GetIsAliveCpp();
	
		virtual bool GetIsAlive();

	UFUNCTION(BlueprintNativeEvent)
		TArray<AActor*> GetAliveEnemiesBP();
	UFUNCTION()
		virtual TArray<AActor*> GetAliveEnemiesBP_Implementation();

	UFUNCTION(BlueprintNativeEvent)
		AActor* GetClosestEnemy();
	UFUNCTION()
		virtual AActor* GetClosestEnemy_Implementation();
	
	
	*/

};
