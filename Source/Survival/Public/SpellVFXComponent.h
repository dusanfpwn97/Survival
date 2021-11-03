// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "SpellDatatypes.h"
#include "SpellVFXComponent.generated.h"


class UNiagaraSystem;
class UNiagaraComponent;
class ABaseSpell;
class UBaseSpellManager;
 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
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
		ABaseSpell* SpellOwner;
	UPROPERTY()
		UBaseSpellManager* SpellManagerOwner;

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

	UFUNCTION()
	void StartMainVFX();
	UFUNCTION()
	void StopMainVFX();
	UFUNCTION()
	void StartHitVFX();
	UFUNCTION()
	void SetupVFX(UBaseSpellManager* NewSpellManager, ABaseSpell* NewSpellOwner);
	UFUNCTION()
	void Hibernate();
	
};
