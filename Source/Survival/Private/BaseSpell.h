// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PoolInterface.h"
#include "CombatInterface.h"

class UNiagaraComponent;
class USphereComponent;
class UBaseSpellManager;

#include "BaseSpell.generated.h"

UCLASS(Blueprintable)
class ABaseSpell : public AActor, public IPoolInterface, public ICombatInterface
{
	GENERATED_BODY()
	
public:	

	ABaseSpell();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void Start_Implementation() override;
	UFUNCTION()
		void Reset_Implementation() override;
	UFUNCTION()
		virtual void SetTarget_Implementation(AActor* NewTarget) override;
	UFUNCTION()
		virtual void SetSpellManager_Implementation(UBaseSpellManager* NewSpellManager) override;
	UFUNCTION()
		UBaseSpellManager* GetSpellManager() const;


protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USphereComponent* BaseCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UNiagaraComponent* NGParticle;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		AActor* TargetActor;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		UBaseSpellManager* SpellManager;

	UPROPERTY()
		FTimerHandle DestroyTimerHandle;

		FVector StartingDirection;

	UFUNCTION()
		void MoveTowardsTarget();
	UFUNCTION()
		void Finish();

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void RemoveCollision();
	UFUNCTION()
		void SetupCollision();

private:

	UFUNCTION()
		void SetupComponents();

};
