// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CombatInterface.h"
#include "PoolInterface.h"
#include "Engine/DataTable.h"
#include "BaseEnemy.generated.h"

class ABasePlayerPawn;
class UCapsuleComponent;
class ABaseSpell;
class UEnemySpawner;
class USkeletalMeshComponent;
class UAnimSequence;


USTRUCT(BlueprintType)
struct FEnemyStats : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		TSoftClassPtr<ABaseEnemy> Class;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		FText Name;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float DifficultyThresholdForSpawning;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Damage;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Speed;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float Health;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float ExpReward;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		float AttackInterval;

};

UCLASS(Blueprintable)
class ABaseEnemy : public AActor, public ICombatInterface, public IPoolInterface
{
	GENERATED_BODY()
	
public:	

	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//
	UFUNCTION()
		void OnCollidedWithSpell_Implementation(ABaseSpell* Spell) override;
	UFUNCTION()
		virtual void Start_Implementation() override;
	UFUNCTION()
		virtual void Reset_Implementation() override;
	UFUNCTION()
		virtual void SetSpawner_Implementation(UObject* Object) override;
	UFUNCTION()
		virtual void SetTarget_Implementation(AActor* TargetActor) override;
	UFUNCTION()
		virtual bool GetIsAlive_Implementation() override;
	UFUNCTION()
		USkeletalMeshComponent* GetSkeletalMesh();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		UAnimSequence* DeathAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		UAnimSequence* HitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Setup")
		UAnimSequence* RunAnimation;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsAlive = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FEnemyStats InitialStats;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FEnemyStats CurrentStats;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Setup")
		UDataTable* DataTableToUse;
	UPROPERTY()
		FTimerHandle DestroyTimerHandle;
	UPROPERTY()
		FTimerHandle StartDecomposingTimerHandle;
	UPROPERTY()
		FTimerHandle MoveActorDownwardsTimerHandle;
	UPROPERTY()
		FTimerHandle StartRunningTimerHandle;
	UPROPERTY()
		AActor* Target;
	UPROPERTY()
		FVector Velocity = FVector::ZeroVector;
	UPROPERTY()
		FRotator LastRotation = FRotator::ZeroRotator;
	UPROPERTY()
		UObject* CurrentPoolManager;
	UPROPERTY()
		bool CanMove = false;

	UFUNCTION()
		void ClearAllTimers();

	UFUNCTION()
		void MoveActorDownwards();
	UFUNCTION()
		void StartDecomposing();
	UFUNCTION()
		void StartRunning();

	//
	UFUNCTION()
		void SetupComponents();
	UFUNCTION()
		void MoveTowardsTarget();
	UFUNCTION()
		void Die();
	UFUNCTION()
		void RemoveCollision();
	UFUNCTION()
		void SetupCollision();
	UFUNCTION()
		void ReceiveDamage(UBaseSpellManager* SpellManagerThatDamaged);
	UFUNCTION()
		void UpdateStats();
	UFUNCTION()
		void PlayNewAnim(UAnimSequence* AnimToPlay, bool ShouldLoop);

private:

};
