// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatInterface.h"

#include "BasePlayerPawn.generated.h"

class UCapsuleComponent;
class ABaseEnemy;
class USkeletalMeshComponent;
class ABaseSpellManager;
class UStaticMeshComponent;
class USpellComponent;
class USceneComponent;
class ASurvivalGM;
class ASurvivalGS;
class UCombatComponent;

UCLASS(Blueprintable, Abstract)
class ABasePlayerPawn : public APawn, public ICombatInterface
{
	GENERATED_BODY()

public:
	ABasePlayerPawn();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnOverlapEnd(class UPrimitiveComponent* OverlappedComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	//

	UFUNCTION()
		virtual FVector GetSpellCastLocation() override;
		virtual TArray<AActor*> GetAliveEnemies() override;
		virtual AActor* GetClosestEnemy() override;
		virtual void OnHit(float Damage) override;
		virtual void OnDeath() override;
		virtual TArray<AActor*> GetClosestEnemies(int32 NumOfEnemies) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaffMesh;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USpellComponent* SpellComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCombatComponent* CombatComponent;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USceneComponent* SpellLocationTemp;


	UPROPERTY()
		ASurvivalGM* CurrentGameMode;
	UPROPERTY()
		ASurvivalGS* CurrentGameState;
protected:

	virtual void BeginPlay() override;



	//
	UFUNCTION()
		void SetupComponents();



};
