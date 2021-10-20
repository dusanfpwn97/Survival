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
		FSpellInfo GetSpellInfo();

	UFUNCTION()
		void SpawnHitParticle(FVector Location);

	UFUNCTION()
		AActor* GetCaster() const;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FSpellInfo SpellInfo;
	UPROPERTY()
		FTimerHandle MainSpellCastTimerHandle;
	UPROPERTY()
		UPoolManager* SpellPoolManager;
	UPROPERTY()
		AActor* Caster;
	UPROPERTY()
		UClass* SpellClassToSpawn;
	UPROPERTY()
		TMap<SpellFXType, UNiagaraSystem*> CachedParticles;
	//
	UFUNCTION()
		void CastSpell();
	UFUNCTION()
		AActor* GetActorForTarget();
	UFUNCTION()
		void UpdateSpellClass();

	
private:
	
};
