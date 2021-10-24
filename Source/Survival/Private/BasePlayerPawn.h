// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "CombatInterface.h"

#include "BasePlayerPawn.generated.h"

class UCapsuleComponent;
class ABaseEnemy;
class USkeletalMeshComponent;
class UBaseSpellManager;
class UStaticMeshComponent;

class ABaseGameMode;

UCLASS(Blueprintable)
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
		virtual void OnCollidedWithEnemy_Implementation(ABaseEnemy* Enemy) override;
	UFUNCTION()
		virtual FVector GetSpellCastLocation_Implementation() override;
	UFUNCTION()
		virtual TArray<AActor*> GetAliveEnemies_Implementation() override;
	UFUNCTION()
		virtual AActor* GetClosestEnemy_Implementation() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UCapsuleComponent* MainCollider;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		USkeletalMeshComponent* SkeletalMesh;
	UPROPERTY()
		ABaseGameMode* CurrentGameMode;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		UStaticMeshComponent* StaffMesh;
	//
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
		TArray<UBaseSpellManager*> SpellManagers;

	UFUNCTION(BlueprintCallable)
		void AddNewSpell(TSoftClassPtr<UBaseSpellManager> SpellManagerClass);


protected:

	virtual void BeginPlay() override;



	//
	UFUNCTION()
		void SetupComponents();



};
