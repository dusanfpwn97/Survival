// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpellDatatypes.h"
#include "BaseSpellManager.generated.h"

class ABaseSpell;
class UPoolManager;
class ABasePlayerPawn;
class UNiagaraSystem;
class UDataTable;
class USpellVFXComponent;
class UInstancedStaticMeshComponent;



UCLASS()
class ABaseSpellManager : public AActor
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	ABaseSpellManager();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		AActor* GetCaster() const;
	UPROPERTY()
		USpellVFXComponent* VFXComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FSpellInfo CurrentSpellInfo;
	UPROPERTY()
		FTimerHandle MainSpellCastTimerHandle;
	UPROPERTY()
		FTimerHandle DebugTimerHandle;
	UPROPERTY()
		UInstancedStaticMeshComponent* ISMComp;
	UPROPERTY()
		UPoolManager* SpellPoolManager;
	UPROPERTY()
		TArray<SpellModifier> SpellModifiers;
	UPROPERTY()
		AActor* Caster;
	UPROPERTY()
		UClass* SpellClassToSpawn;
	UPROPERTY()
		TArray<FSpellRuntimeInfo> SpellInstances;
	UFUNCTION()
		bool GetIsStaticLocationSpell() const;
	UPROPERTY()
		UDataTable* VFX_DataTable;
	UFUNCTION()
		void SetVFXDataTable();
	UFUNCTION()
		void GetVFXDataFromDT(UStaticMesh*& Mesh, UMaterialInterface*& Mat);
	//
	UFUNCTION()
		void CastSpell(FSpellRuntimeInfo AdditonalInfo);

	UFUNCTION()
		void ResetInstance(const int Index);
	UFUNCTION()
		int GetAvailableSpellInstanceIndex();
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
	UFUNCTION()
		void DebugValues();

	AActor* IsColliding(const int Index, TArray<AActor*>& ActorsToCheck);

	void UpdateSpellLocations();

	void StartCastSpellTimer(bool ShouldLoop);

	void OnSpellFinished(ABaseSpell* FinishedSpell);

	bool IsSingleCastSpell();

	void OnInstanceCollided(int Index, AActor* Actor);

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
