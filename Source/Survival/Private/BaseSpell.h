// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolInterface.h"
#include "CombatInterface.h"

class UNiagaraComponent;
class USphereComponent;
class ABaseSpellManager;
class UPoolManager;


#include "BaseSpell.generated.h"

UCLASS(Blueprintable)
class ABaseSpell : public AActor, public IPoolInterface, public ICombatInterface
{
	GENERATED_BODY()
	
public:	
	ABaseSpell();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		ABaseSpellManager* GetSpellManager() const;
	//
	UFUNCTION()
		virtual void Start_Implementation() override;
	UFUNCTION()
		virtual void Reset_Implementation() override;
	UFUNCTION()
		virtual void SetSpellManager_Implementation(ABaseSpellManager* NewSpellManager) override;
	UFUNCTION()
		virtual void SetSpawner_Implementation(UObject* Object) override;
	UFUNCTION()
		virtual void SetOrderIndex_Implementation(int NewOrderIndex) override;
	UFUNCTION()
		virtual void UpdateTarget();

	UPROPERTY()
		bool IsMarkedForDestruction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* BaseCollider;

	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* TargetActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		ABaseSpellManager* SpellManager;

	TArray<AActor*> CollidedActors;

	UFUNCTION()
		virtual FVector GetMoveDirection();
	UPROPERTY()
		FVector CurrentDirection;

	bool HasDeterminedDirection = false;
protected:
	virtual void BeginPlay() override;

	UFUNCTION()
		void SetWatchdogTimers();
	UPROPERTY()
		FTimerHandle ResetTimerHandle;
	UPROPERTY()
		FTimerHandle FinishTimerHandle;
	UPROPERTY()
		FTimerHandle CheckTargetTimerHandle;
	UPROPERTY()
		FTimerHandle UpdateDirectionTimerHandle;

	UFUNCTION()
		virtual void CheckTarget();
	UFUNCTION()
		virtual void Move();
	UFUNCTION()
		void Finish();

	UFUNCTION()
		void ClearAllTimers();
	
	UFUNCTION()
		void OnOverlapBeginDistance(AActor* OtherActor);
	UFUNCTION()
		void CheckOverlapDistance();

	UPROPERTY()
		UObject* CurrentPoolManager;

	float WatchdogTime = 10.f;

	FVector DirectionConstant;

	int OrderIndex = 0;
private:
	UFUNCTION()
		void SetupComponents();

};
