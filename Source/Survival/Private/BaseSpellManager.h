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
		UNiagaraSystem* GetNiagaraSystem(Element Element, CastType CastType, SpellFXType SpellFXType);

	UFUNCTION()
		void SpawnHitParticle(FVector Location);

	UFUNCTION()
		AActor* GetCaster() const;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FSpellInfo CurrentSpellInfo;
	UPROPERTY()
		FTimerHandle MainSpellCastTimerHandle;
	UPROPERTY()
		UPoolManager* SpellPoolManager;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Setup")
		UDataTable* VFX_DataTable;
	UPROPERTY()
		AActor* Caster;
	UPROPERTY()
		UClass* SpellClassToSpawn;
	UPROPERTY()
		TMap<SpellFXType, UNiagaraSystem*> CachedParticles;
	UFUNCTION()
		bool GetIsTargetlessSpell() const;
	//
	UFUNCTION()
		void CastSpell();
	UFUNCTION()
		AActor* GetActorForTarget();
	UFUNCTION()
		void InitSpellManager(FSpellInfo NewSpellInfo);
	UFUNCTION()
		void DestroySpellManager();
	UFUNCTION()
		void UpdateCastType(CastType NewCastType);
	UFUNCTION()
		void UpdateSpellModifier(SpellModifier NewSpellModifier);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void UpdateSpellClass();
	UFUNCTION()
		void SetVFXDataTable();
	UFUNCTION()
		void MarkAllSpellsForDestruction();
	
	UPROPERTY()
		bool IsTargetlessSpell = false;


	UFUNCTION()
		void UpdateIsTargetlessSpell();
private:
	

};
