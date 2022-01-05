// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "DrawDebugHelpers.h"
#include "UObject/ConstructorHelpers.h"
#include "Animation/AnimSequence.h"
#include "BaseSpellManager.h"
#include "Engine/EngineTypes.h"
#include "Components/SkinnedMeshComponent.h"
#include "Engine/DataTable.h"
#include "PoolManager.h"
#include "SpellDatatypes.h"
#include "EnemySpawner.h"
#include "CombatComponent.h"


// Sets default values
ABaseEnemy::ABaseEnemy()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();

}

// Called when the game starts or when spawned
void ABaseEnemy::BeginPlay()
{

	Super::BeginPlay();
	UpdateStats();
}


// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if(CanMove) MoveTowardsTarget();
	MoveTowardsTarget();
	CheckAttack();
}

void ABaseEnemy::ReceiveDamage(ABaseSpellManager* SpellManager)
{

	if (!CombatComponent->IsAlive) return;

	if (!SpellManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, TEXT("SpellManager not valid! Should not happen. TODO! BaseEnemy.cpp -> ReceiveDamage"));
		return;
	}
	CombatComponent->TakeDamage(50.f); //SpellManager->SpellInfo.Damage;
	
	if (CombatComponent->Health <= 0.f)
	{
		Die();
		///GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("SpelsdfsdfsefesfeDamage"));
	}

	if (IsAlive)
	{
		UWorld* World = GetWorld();
		if (!World) return;

		float AnimTime = (HitAnimation->SequenceLength / HitAnimation->RateScale) - 0.05f;
		AnimTime = FMath::Clamp(AnimTime, 0.035f, 1.0f);
		StopAttacking();
		GetWorld()->GetTimerManager().SetTimer(StartRunningTimerHandle, this, &ABaseEnemy::StartRunning, AnimTime, false);

		CanMove = false;
		PlayNewAnim(HitAnimation, false);


	}
}

void ABaseEnemy::OnCollidedWithSpell(ABaseSpellManager* Spell, SpellModifier SpellModifier)
{
	if (!Spell)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spell not valid! Should not happen. BaseEnemy.cpp -> OnCollidedWithSpell_Implementation"));
		return;
	}
	
	ReceiveDamage(Spell);
}
/*
void ABaseEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		//TODO Replace
}
*/
void ABaseEnemy::ClearAllTimers()
{
	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(StartDecomposingTimerHandle);
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
		World->GetTimerManager().ClearTimer(MoveActorDownwardsTimerHandle);
		World->GetTimerManager().ClearTimer(StartRunningTimerHandle);
	}
}

void ABaseEnemy::MoveActorDownwards()
{
	UWorld* World = GetWorld();
	if (!World) return;

	FHitResult Hit;
	AddActorWorldOffset(FVector(0, 0, 1) * World->GetDeltaSeconds() * -250, false, &Hit, ETeleportType::None);
}

void ABaseEnemy::StartDecomposing()
{
	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().SetTimer(MoveActorDownwardsTimerHandle, this, &ABaseEnemy::MoveActorDownwards, 0.035, true);

}

void ABaseEnemy::StartRunning()
{
	CanMove = true;
	PlayNewAnim(RunAnimation, true);

}

void ABaseEnemy::StopRunning()
{
	CanMove = false;
	//PlayNewAnim(RunAnimation, true);

}

bool ABaseEnemy::IsTargetInRangeForAttack()
{
	if (Target)
	{
		if (FVector::Dist(Target->GetActorLocation(), GetActorLocation()) < InitialStats.AttackRange)
		{
			return true;
		}
		return false;
	}
	return false;
}

void ABaseEnemy::CheckAttack()
{
	if (IsTargetInRangeForAttack() && !IsAttacking)
	{
		StartAttacking();
	}
}

void ABaseEnemy::StartAttacking()
{

	UWorld* World = GetWorld();
	if (!World) return;
	StopRunning();

	float AttackAnimTime = (AttackAnimation->SequenceLength / HitAnimation->RateScale);

	World->GetTimerManager().SetTimer(AttackDamageTimerHandle, this, &ABaseEnemy::DoDamageFromAttack, AttackAnimTime * 0.4f, false);
	//GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("%s"), *FString::SanitizeFloat(AttackAnimTime)));

	World->GetTimerManager().SetTimer(StopAttackTimerHandle, this, &ABaseEnemy::StopAttacking, AttackAnimTime-0.3f, false);

	PlayNewAnim(AttackAnimation, false);
	IsAttacking = true;
}

void ABaseEnemy::StopAttacking()
{
	IsAttacking = false;

	if (!IsTargetInRangeForAttack())
	{
		StartRunning();
	}

	UWorld* World = GetWorld();
	if (!World) return;

	World->GetTimerManager().ClearTimer(AttackDamageTimerHandle);
	World->GetTimerManager().ClearTimer(StopAttackTimerHandle);
}

void ABaseEnemy::DoDamageFromAttack()
{
	if (IsTargetInRangeForAttack() && Target)
	{
		if(Target->GetClass()->ImplementsInterface(UCombatInterface::StaticClass()))
		{
			ICombatInterface* TempInterface = Cast<ICombatInterface>(Target);
			if (!TempInterface) return;
			TempInterface->OnHit(InitialStats.Damage);
		}
	}
}

void ABaseEnemy::SetupComponents()
{
	//MainCollider = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("MainCollider")));
	//RootComponent = MainCollider;
	
	//MainCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapBegin);
	//MainCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapEnd);
	/*
	MainCollider->SetCanEverAffectNavigation(false);
	MainCollider->SetGenerateOverlapEvents(false);
	MainCollider->SetCollisionProfileName(FName(TEXT("NoCollision")));
	*/
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	RootComponent = SkeletalMesh;
	//SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	SkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	SkeletalMesh->bEnableUpdateRateOptimizations = true;
	//SkeletalMesh->bDisplayDebugUpdateRateOptimizations = true;
	SkeletalMesh->SkipUpdateOverlapsOptimEnabled = true;
	SkeletalMesh->bComponentUseFixedSkelBounds = true;
	SkeletalMesh->bDeferKinematicBoneUpdate = true;
	SkeletalMesh->bDisableClothSimulation = true;
	SkeletalMesh->bEnableLineCheckWithBounds = false;
	SkeletalMesh->bEnablePerPolyCollision = false;
	SkeletalMesh->SetCanEverAffectNavigation(false);
	SkeletalMesh->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	SkeletalMesh->bSkipBoundsUpdateWhenInterpolating = true;
	SkeletalMesh->bSkipKinematicUpdateWhenInterpolating = true;

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(FName(TEXT("CombatComponent")));
}

void ABaseEnemy::Start()
{
	IsAlive = true;
	CombatComponent->Init(InitialStats.Health);
	SkeletalMesh->SetComponentTickEnabled(true);
	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	ClearAllTimers();
	SetupCollision();
	StartRunning();

}

void ABaseEnemy::Reset()
{
	IsAlive = false;

	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);
	SkeletalMesh->SetComponentTickEnabled(false);

	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
		World->GetTimerManager().ClearTimer(AttackDamageTimerHandle);
		World->GetTimerManager().ClearTimer(StopAttackTimerHandle);
	}
	if (CurrentPoolManager)
	{
		CurrentPoolManager->ReleaseToPool_Implementation(this);
	}
	else GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawner not valid! Should not happen. BaseEnemy.cpp -> Reset implementation"));
	
	RemoveCollision();

	// Stop Animation
	if (SkeletalMesh)
	{
		if (SkeletalMesh->IsPlaying())
		{
			SkeletalMesh->Stop();
		}
	}
}

void ABaseEnemy::SetSpawner(UObject* Object)
{
	if (!Object)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawner not valid! Should not happen. BaseEnemy.cpp -> SetSpawner_Implementation"));
		return;
	}

	CurrentPoolManager = Cast<UPoolManager>(Object);
}

void ABaseEnemy::SetTarget(AActor* TargetActor)
{
	Target = TargetActor;
}

bool ABaseEnemy::GetIsAlive()
{
	return IsAlive;
}

void ABaseEnemy::MoveTowardsTarget()
{
	UWorld* World = GetWorld();
	if (!World || !Target) return;

	FHitResult Hit;
	FVector Direction;
	Direction = Target->GetActorLocation() - GetActorLocation();
	Direction.Normalize();
	Direction.Z = 0.f;


	FHitResult Hits;
	FVector Start = GetActorLocation() - GetActorRightVector()*30;
	
	FCollisionObjectQueryParams ObjectQueryParams;
	ObjectQueryParams.AddObjectTypesToQuery(ECollisionChannel::ECC_GameTraceChannel1);
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	
	TArray<AActor*> HitActors;
	
	LastRotation = FMath::RInterpTo(LastRotation, (Direction.Rotation() + FRotator(0.f, -90.f, 0.f)), World->GetDeltaSeconds(), 6.f);
	SetActorRotation(LastRotation, ETeleportType::None);

	if (!CanMove || IsAttacking) return;

	TArray<AActor*> TempAliveEnemies = Spawner->CachedAliveSpawns;
		
	for (int i = 0; i < TempAliveEnemies.Num(); i++)
	{
		float TempDist = FVector::Dist(TempAliveEnemies[i]->GetActorLocation(), GetActorLocation());

		if (TempDist < InitialStats.Size)
		{
			if (TempAliveEnemies[i] != this)
			{
				ABaseEnemy* TempEnemy = Cast<ABaseEnemy>(TempAliveEnemies[i]);
				if (TempEnemy)
				{
					//float Dot = FVector::DotProduct(TempAliveEnemies[i]->GetActorForwardVector(), GetActorForwardVector());
					//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Dot: %s"), *FString::SanitizeFloat(Dot)));
					
					FVector Temp = TempEnemy->GetActorLocation() - GetActorLocation();
					Temp.Normalize();
					Direction += -Temp * 0.85;
				}
			}
		}
	}

	Direction.Normalize();
	Direction.Z = 0;
	Velocity = FMath::VInterpTo(Velocity, Direction * InitialStats.Speed, World->GetDeltaSeconds(), 1);
	//Velocity = Direction * 200 * World->GetDeltaSeconds();

	//GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, FString::Printf(TEXT("%f"), Velocity.X));
	Velocity = World->GetDeltaSeconds() * Direction * InitialStats.Speed;
	Velocity.Z = 0;
	AddActorWorldOffset(Velocity, false, &Hit, ETeleportType::None);

}

void ABaseEnemy::Die()
{
	UWorld* World = GetWorld();
	if (!World || !IsAlive) return;

	ClearAllTimers();
	IsAlive = false;
	SetActorTickEnabled(false);
	RemoveCollision();
	CanMove = false;

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABaseEnemy::Reset, 5.f, false);
	GetWorld()->GetTimerManager().SetTimer(StartDecomposingTimerHandle, this, &ABaseEnemy::StartDecomposing, 3.5f, false);

	
	PlayNewAnim(DeathAnimation, false);

}

void ABaseEnemy::UpdateStats()
{
	if (!DataTableToUse)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable for Enemy Stats")));
		return;
	}

	for (auto it : DataTableToUse->GetRowMap())
	{
		FEnemyStats* Data = (FEnemyStats*)(it.Value);
		if (Data)
		{
			if (Data->Class == GetClass())
			{
				InitialStats = *Data;
				return;
			}
		}
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable Row for Enemy Stats")));
}

void ABaseEnemy::PlayNewAnim(UAnimSequence* AnimToPlay, bool ShouldLoop)
{
	if (!SkeletalMesh) return;

	if (!AnimToPlay)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Animation asset is null! %s"), *GetClass()->GetName()));
		return;
	}

	if (SkeletalMesh->IsPlaying()) SkeletalMesh->Stop();

	SkeletalMesh->SetAnimation(AnimToPlay);
	SkeletalMesh->Play(ShouldLoop);
}

USkeletalMeshComponent* ABaseEnemy::GetSkeletalMesh()
{
	return SkeletalMesh;
}

void ABaseEnemy::RemoveCollision()
{
	//MainCollider->SetGenerateOverlapEvents(false);
	//MainCollider->SetCollisionProfileName(FName(TEXT("NoCollision")));
}

void ABaseEnemy::SetupCollision()
{
	//MainCollider->SetGenerateOverlapEvents(true);
	//MainCollider->SetCollisionProfileName(FName(TEXT("Enemy")));
}