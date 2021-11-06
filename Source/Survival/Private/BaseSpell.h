// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolInterface.h"
#include "CombatInterface.h"

class UNiagaraComponent;
class USphereComponent;
class UBaseSpellManager;
class UPoolManager;
class USpellMovementComponent;
class USpellVFXComponent;

#include "BaseSpell.generated.h"

UCLASS(Blueprintable)
class ABaseSpell : public AActor, public IPoolInterface, public ICombatInterface
{
	GENERATED_BODY()
	
public:	
	ABaseSpell();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		UBaseSpellManager* GetSpellManager() const;
	//
	UFUNCTION()
		virtual void Start_Implementation() override;
	UFUNCTION()
		virtual void Reset_Implementation() override;
	UFUNCTION()
		virtual void SetTarget_Implementation(AActor* NewTarget) override;
	UFUNCTION()
		virtual void SetSpellManager_Implementation(UBaseSpellManager* NewSpellManager) override;
	UFUNCTION()
		virtual void SetSpawner_Implementation(UObject* Object) override;

	UPROPERTY()
		bool IsMarkedForDestruction = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* BaseCollider;
	UPROPERTY()
		USpellVFXComponent* VFXComponent;
	
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* TargetActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBaseSpellManager* SpellManager;
	UPROPERTY()
		USpellMovementComponent* SpellMovementComponent;

	TArray<AActor*> CollidedActors;

	UFUNCTION()
		void UpdateMoveDirection();
	UPROPERTY()
		FVector LastDirection;
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
	UPROPERTY()
		FTimerHandle SetupCollisionTimerHandle;

	UFUNCTION()
		void CheckTarget();
	UFUNCTION()
		void Move();
	UFUNCTION()
		void Finish();
	UFUNCTION()
		void RemoveCollision();
	UFUNCTION()
		void SetupCollision();
	UFUNCTION()
		void ClearAllTimers();
	
	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UPROPERTY()
		UObject* CurrentPoolManager;

	float WatchdogTime = 10.f;

private:
	UFUNCTION()
		void SetupComponents();

};
