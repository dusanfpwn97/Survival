// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellDatatypes.h"
#include "BaseSpellManager.generated.h"

class ABaseSpell;
class UPoolManager;
class ABasePlayerPawn;
class UNiagaraSystem;
class UDataTable;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent), Blueprintable)
class UBaseSpellManager : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UBaseSpellManager();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
		AActor* GetCaster() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FSpellInfo CurrentSpellInfo;
	UPROPERTY()
		FTimerHandle MainSpellCastTimerHandle;
	UPROPERTY()
		UPoolManager* SpellPoolManager;
	UPROPERTY()
		TArray<SpellModifier> SpellModifiers;
	UPROPERTY()
		AActor* Caster;
	UPROPERTY()
		UClass* SpellClassToSpawn;

	UFUNCTION()
		bool GetIsStaticLocationSpell() const;
	//
	UFUNCTION()
		void CastSpell(FAdditionalSpellInfo AdditonalInfo);
	UFUNCTION()
		void CastSpellLoop();
	UFUNCTION()
		void InitSpellManager(FSpellInfo NewSpellInfo);
	UFUNCTION()
		void DestroySpellManager();
	UFUNCTION()
		void UpdateCastType(CastType NewCastType);
	UFUNCTION()
		void UpdateSpellModifier(SpellModifier NewSpellModifier);
	UFUNCTION()
		FVector GetStartingSpellLocation();
	UFUNCTION()
		void AddSpellModifier(SpellModifier NewSpellModifier);
	UFUNCTION()
		void RemoveSpellModifier(SpellModifier NewSpellModifier);

	void StartCastSpellTimer(bool ShouldLoop);

	void OnSpellFinished(ABaseSpell* FinishedSpell);

	bool IsSingleCastSpell();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void UpdateSpellClass();

	UFUNCTION()
		void MarkAllSpellsForDestruction();
	
	UPROPERTY()
		bool IsTargetlessSpell = false;


	UFUNCTION()
		void UpdateIsTargetlessSpell();

	UFUNCTION()
		UClass* GetSpellClassForSpawning();
private:
	

};
