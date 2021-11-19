// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseEnemy.h"
#include "BaseSpell.h"
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
	UpdateStats();
	Super::BeginPlay();

}


// Called every frame
void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//if(CanMove) MoveTowardsTarget();
	MoveTowardsTarget();

}

void ABaseEnemy::ReceiveDamage(UBaseSpellManager* SpellManager)
{

	if (!IsAlive) return;

	if (!SpellManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("SpellManager not valid! Should not happen. BaseEnemy.cpp -> ReceiveDamage"));
		return;
	}

	CurrentStats.Health -= 50.f;//SpellManager->SpellInfo.Damage;
	if (CurrentStats.Health <= 0)
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
		GetWorld()->GetTimerManager().SetTimer(StartRunningTimerHandle, this, &ABaseEnemy::StartRunning, AnimTime, false);

		CanMove = false;
		PlayNewAnim(HitAnimation, false);

	}
}

void ABaseEnemy::OnCollidedWithSpell_Implementation(ABaseSpell* Spell)
{
	if (!Spell)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spell not valid! Should not happen. BaseEnemy.cpp -> OnCollidedWithSpell_Implementation"));
		return;
	}
	UBaseSpellManager* SpellManager = Spell->GetSpellManager();
	if (!SpellManager)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("SpellManager not valid! Should not happen. BaseEnemy.cpp -> OnCollidedWithSpell_Implementation"));
		return;
	}

	ReceiveDamage(SpellManager);
}

void ABaseEnemy::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
		//TODO Replace
}

void ABaseEnemy::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

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

	GetWorld()->GetTimerManager().SetTimer(MoveActorDownwardsTimerHandle, this, &ABaseEnemy::MoveActorDownwards, 0.035, true);

}

void ABaseEnemy::StartRunning()
{
	CanMove = true;
	PlayNewAnim(RunAnimation, true);

}

void ABaseEnemy::SetupComponents()
{
	MainCollider = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("MainCollider")));
	RootComponent = MainCollider;
	
	MainCollider->OnComponentBeginOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapBegin);
	MainCollider->OnComponentEndOverlap.AddDynamic(this, &ABaseEnemy::OnOverlapEnd);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));
	SkeletalMesh->bEnableUpdateRateOptimizations = true;
	//SkeletalMesh->bDisplayDebugUpdateRateOptimizations = true;
	SkeletalMesh->SkipUpdateOverlapsOptimEnabled = true;
	SkeletalMesh->bComponentUseFixedSkelBounds = true;
	SkeletalMesh->bDeferKinematicBoneUpdate = true;
	SkeletalMesh->bDisableClothSimulation = true;
	SkeletalMesh->bEnableLineCheckWithBounds = false;
	SkeletalMesh->bEnablePerPolyCollision = false;
	

}

void ABaseEnemy::Start_Implementation()
{
	IsAlive = true;

	SetActorTickEnabled(true);
	SetActorHiddenInGame(false);
	ClearAllTimers();
	SetupCollision();
	StartRunning();

}

void ABaseEnemy::Reset_Implementation()
{
	IsAlive = false;

	SetActorTickEnabled(false);
	SetActorHiddenInGame(true);

	UWorld* World = GetWorld();
	{
		World->GetTimerManager().ClearTimer(DestroyTimerHandle);
	}
	if (CurrentPoolManager)
	{
		IPoolInterface::Execute_ReleaseToPool(CurrentPoolManager, this);
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

void ABaseEnemy::SetSpawner_Implementation(UObject* Object)
{
	if (!Object)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Spawner not valid! Should not happen. BaseEnemy.cpp -> SetSpawner_Implementation"));
		return;
	}
	CurrentPoolManager = Object;
}

void ABaseEnemy::SetTarget_Implementation(AActor* TargetActor)
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

	if (!CanMove) return;
	for (int i = -2; i < 3; i++)
	{
		FRotator TempRot = (Direction.Rotation() + FRotator(0.f, i * -45.f, 0.f));
		FVector End = Start + TempRot.Vector() * MainCollider->GetUnscaledCapsuleRadius()*2.5;

		World->LineTraceSingleByObjectType(Hits, Start, End, ObjectQueryParams, Params);
		//DrawDebugLine(World, Start, End, FColor::Green, false, 0.f);
		if (Hits.IsValidBlockingHit())
		{
			HitActors.Add(Hits.GetActor());

			
		}
	}
	if (HitActors.Num() > 0)
	{
		for (AActor* TempActor : HitActors)
		{
			FVector Temp = TempActor->GetActorLocation() - GetActorLocation();
			Temp.Normalize();
			//Temp.
			Direction += -Temp * 0.85;
		}
		Direction.Normalize();
		Direction.Z = 0;
	}

	Velocity = FMath::VInterpTo(Velocity, Direction * InitialStats.Speed, World->GetDeltaSeconds(), 6);
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

	GetWorld()->GetTimerManager().SetTimer(DestroyTimerHandle, this, &ABaseEnemy::Reset_Implementation, 5.f, false);
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
				CurrentStats = InitialStats;
				return;
			}
		}
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Couldn't find Datatable Row for Enemy Stats")));
	}
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
	MainCollider->SetGenerateOverlapEvents(false);
	MainCollider->SetCollisionProfileName(FName(TEXT("NoCollision")));
}

void ABaseEnemy::SetupCollision()
{
	MainCollider->SetGenerateOverlapEvents(true);
	MainCollider->SetCollisionProfileName(FName(TEXT("Enemy")));
}