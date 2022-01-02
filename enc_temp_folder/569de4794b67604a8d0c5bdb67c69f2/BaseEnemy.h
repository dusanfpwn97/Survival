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
class ABaseSpellManager;
class UPoolManager;
class UCombatComponent;


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

UCLASS(Blueprintable, Abstract)
class ABaseEnemy : public AActor, public ICombatInterface, public IPoolInterface
{
	GENERATED_BODY()
	
public:	

	ABaseEnemy();

	virtual void Tick(float DeltaTime) override;

	//UFUNCTION()
	//	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		virtual void OnCollidedWithSpell(ABaseSpellManager* Spell, SpellModifier SpellModifierThatDamaged) override;
	UFUNCTION()
		virtual void Start() override;
	UFUNCTION()
		virtual void Reset() override;
	UFUNCTION()
		virtual void SetSpawner(UObject* Object) override;
	UFUNCTION()
		virtual void SetTarget(AActor* TargetActor) override;
	UFUNCTION()
		virtual bool GetIsAlive() override;
	UFUNCTION()
		USkeletalMeshComponent* GetSkeletalMesh();

	UPROPERTY()
		UEnemySpawner* Spawner;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;
	//
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Setup")
		UAnimSequence* DeathAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Setup")
		UAnimSequence* HitAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Setup")
		UAnimSequence* RunAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "_Setup")
		UAnimSequence* AttackAnimation;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		bool IsAlive = false;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		FEnemyStats InitialStats;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "_Setup")
		UDataTable* DataTableToUse;

		FTimerHandle DestroyTimerHandle;
		FTimerHandle StartDecomposingTimerHandle;
		FTimerHandle MoveActorDownwardsTimerHandle;
		FTimerHandle StartRunningTimerHandle;
		FTimerHandle StopAttackTimerHandle;
		FTimerHandle AttackDamageTimerHandle;

	UPROPERTY()
		AActor* Target;
	UPROPERTY()
		FVector Velocity = FVector::ZeroVector;
	UPROPERTY()
		FRotator LastRotation = FRotator::ZeroRotator;
	UPROPERTY()
		UPoolManager* CurrentPoolManager;
	UPROPERTY()
		bool CanMove = false;

	bool IsAttacking = false;

	void ClearAllTimers();
	void MoveActorDownwards();
	void StartDecomposing();
	void StartRunning();
	void StopRunning();

	bool IsTargetInRangeForAttack();
	void CheckAttack();
	void StartAttacking();
	void StopAttacking();
	void DoDamageFromAttack();

	void SetupComponents();
	void MoveTowardsTarget();
	void Die();
	void RemoveCollision();
	void SetupCollision();
	void ReceiveDamage(ABaseSpellManager* SpellManagerThatDamaged);
	void UpdateStats();
	void PlayNewAnim(UAnimSequence* AnimToPlay, bool ShouldLoop);

private:

};
