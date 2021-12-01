// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellDatatypes.h"
#include "SpellVFXComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;
class ABaseSpellManager;
 
UCLASS()
class SURVIVAL_API USpellVFXComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	USpellVFXComponent();

	UPROPERTY()
		UNiagaraComponent* MainVFX;
	UPROPERTY()
		UNiagaraSystem* HitVFX;
	UPROPERTY()
		ABaseSpellManager* SpellManagerOwner;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	void WakeUp();

	UFUNCTION()
		UNiagaraSystem* GetNiagaraSystem(SpellFXType SpellFXType);
	UPROPERTY()
		UDataTable* VFX_DataTable;

	UFUNCTION()
		void SetVFXDataTable();
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;


	void StartMainVFX();

	void StopMainVFX();

	void SpawnHitVFX(FVector Location);

	void SetupVFX(ABaseSpellManager* NewSpellManager);

	void Hibernate();

	void GetVFXDataFromDT(UStaticMesh*& Mesh, UMaterialInterface*& Mat);
	
};
