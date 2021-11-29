// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellDatatypes.h"
#include "BaseSpellManager.generated.h"

class UPoolManager;
class ABasePlayerPawn;
class UNiagaraSystem;
class UDataTable;
class USpellVFXComponent;
class UInstancedStaticMeshComponent;
class UMultithreadCalculator;

UCLASS()
class ABaseSpellManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ABaseSpellManager();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY()
		USpellVFXComponent* VFXComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FSpellInfo CurrentSpellInfo;
	UPROPERTY()
		UInstancedStaticMeshComponent* ISMComp;
	//UPROPERTY()
	//	UMultithreadCalculator* MultithreadCalculatorComponent;
	//UPROPERTY()
		//UPoolManager* SpellPoolManager;
	UPROPERTY()
		TArray<SpellModifier> SpellModifiers;
	UPROPERTY()
		AActor* Caster;
	UPROPERTY()
		TArray<FSpellRuntimeInfo> SpellInstances;
	UPROPERTY()
		UNiagaraSystem* HitNS;
	UPROPERTY()
		UDataTable* VFX_DataTable;

	UFUNCTION()
		void ResetInstance(const int Index);
	
	UFUNCTION()
		void InitSpellManager(FSpellInfo NewSpellInfo);
	UFUNCTION()
		void DestroySpellManager();
	UFUNCTION()
		void UpdateCastType(CastType NewCastType);
	UFUNCTION()
		void UpdateSpellModifier(SpellModifier NewSpellModifier);

		FVector GetStartingSpellLocation();
	UFUNCTION()
		void AddSpellModifier(SpellModifier NewSpellModifier);
	UFUNCTION()
		void RemoveSpellModifier(SpellModifier NewSpellModifier);
	UFUNCTION()
		void DebugValues();

	virtual AActor* GetAppropriateTarget(const int32 Index, const int32 OrderIndex);

	virtual TArray<AActor*> GetAppropriateTargets(const int32 NumOfTargets);

	virtual void CheckForCollisions();

	void StartCastSpellTimer(bool ShouldLoop);

	bool IsSingleCastSpell();

	void CollideInstance(int Index, AActor* Actor);


protected:
	virtual void BeginPlay() override;

	void MarkAllSpellsForDestruction();
	
	//Timers
	UPROPERTY()
		FTimerHandle MainSpellCastTimerHandle;
	UPROPERTY()
		FTimerHandle DebugTimerHandle;
	UPROPERTY()
		FTimerHandle WatchdogTimer;

	virtual void UpdateDirection(const int Index);

	void OnSpellFinished(const int32 Index);

	virtual void MoveSpells();

	virtual void UpdateInstanceTransforms();

	void SetVFXDataTable();

	UFUNCTION()
	void GetVFXDataFromDT(UStaticMesh*& Mesh, UMaterialInterface*& Mat);
	//

	void CastSpell(FSpellRuntimeInfo AdditonalInfo);

	int GetAvailableSpellInstanceIndex();

	virtual void CastSpellLoop();

	void SpellLifetimeCheck();

	float SpellLifetime = 10.f;
};
