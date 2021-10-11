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
	// Sets default values for this actor's properties
	ABaseEnemy();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* Mesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;

	UFUNCTION()
		void SetupComponents();
	UFUNCTION()
		void MoveTowardsTarget();
	UFUNCTION()
		void Die();

	UPROPERTY()
		FTimerHandle DestroyTimerHandle;
	UPROPERTY()
		bool bIsActive = false;
	UPROPERTY()
		AActor* Target;
	UPROPERTY()
	FVector Velocity = FVector::ZeroVector;
	UPROPERTY()
	FRotator LastRotation = FRotator::ZeroRotator;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FEnemyStats InitialStats;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UDataTable* DataTableToUse;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnCollidedWithSpell_Implementation(ABaseSpell* Spell) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION()
		void Start_Implementation() override;
	UFUNCTION()
		void Reset_Implementation() override;
	UFUNCTION()
		void SetSpawner_Implementation(UObject* Object) override;
	
	UPROPERTY()
		UObject* Spawner;

	UFUNCTION()
		void SetTarget_Implementation(AActor* TargetActor) override;

private:
	UFUNCTION()
	void UpdateStats();
};
