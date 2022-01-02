// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerPawn.h"
#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseSpellManager.h"
#include "CombatComponent.h"
#include "HelperFunctions.h"
#include "Kismet/GameplayStatics.h"
#include "SurvivalGM.h"
#include "SurvivalGS.h"
#include "EnemySpawner.h"
#include "SpellComponent.h"

// Sets default values
ABasePlayerPawn::ABasePlayerPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SetupComponents();
}

// Called when the game starts or when spawned
void ABasePlayerPawn::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World)
	{
		CurrentGameMode = Cast<ASurvivalGM>(UGameplayStatics::GetGameMode(World));
		if (!CurrentGameMode) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Game mode not valid! baseplayerpawn -> BeginPlay"));
		CurrentGameState = Cast<ASurvivalGS>(UGameplayStatics::GetGameState(World));
		if (!CurrentGameState) GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Red, TEXT("Game state not valid! baseplayerpawn -> BeginPlay"));
	}
	else GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("World not valid! baseplayerpawn.cpp -> BeginPlay"));

	
	FSpellInfo Info;
	Info.Element = Element::WATER;
	Info.CastType = CastType::FLICK;
	Info.Cooldown = 1.1f;
	Info.Speed = 1000.f;
	Info.Radius = 70.f;
	Info.TargetMode = TargetMode::CLOSEST;
	//SpellComponent->AddNewSpell(Info);

	Info.Element = Element::FIRE;
	Info.CastType = CastType::PROJECTILE;
	Info.Cooldown = 2.f;
	Info.Speed = 990.f;
	Info.Radius = 70.f;
	Info.TargetMode = TargetMode::CLOSEST;
	//SpellComponent->AddNewSpell(Info);

	Info.Element = Element::FIRE;
	Info.CastType = CastType::STORM;
	Info.Cooldown = 0.6f;
	Info.Speed = 1400.f;
	Info.Radius = 30.f;
	Info.TargetMode = TargetMode::CLOSEST;
	//SpellComponent->AddNewSpell(Info);
	 
	Info.Element = Element::ICE;
	Info.CastType = CastType::SHIELD;
	Info.Cooldown = 2.11f;
	Info.Speed = 1000.f;
	Info.Radius = 120.f;
	Info.TargetMode = TargetMode::NONE;
//	SpellComponent->AddNewSpell(Info);

	Info.Element = Element::FIRE;
	Info.CastType = CastType::NOVA;
	Info.Cooldown = 1.21f;
	Info.Speed = 1000.f;
	Info.Radius = 620.f;
	Info.TargetMode = TargetMode::NONE;
	//SpellComponent->AddNewSpell(Info);


}

// Called every frame
void ABasePlayerPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ABasePlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABasePlayerPawn::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//OverlappedComp->GetClass
	if (OverlappedComp == MainCollider && OtherActor->Implements<UCombatInterface>())
	{

	}
}

void ABasePlayerPawn::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABasePlayerPawn::SetupComponents()
{
	MainCollider = CreateDefaultSubobject<UCapsuleComponent>(FName(TEXT("MainCollider")));
	RootComponent = MainCollider;
	MainCollider->SetGenerateOverlapEvents(true);
	MainCollider->SetCollisionProfileName(FName(TEXT("Player")));
	MainCollider->OnComponentBeginOverlap.AddDynamic(this, &ABasePlayerPawn::OnOverlapBegin);
	MainCollider->OnComponentEndOverlap.AddDynamic(this, &ABasePlayerPawn::OnOverlapEnd);

	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(FName(TEXT("SkeletalMesh")));
	SkeletalMesh->SetupAttachment(RootComponent);
	SkeletalMesh->SetGenerateOverlapEvents(false);
	SkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));

	StaffMesh = CreateDefaultSubobject<UStaticMeshComponent>(FName(TEXT("StaffMesh")));
	SpellLocationTemp = CreateDefaultSubobject<USceneComponent>(FName(TEXT("SpellCastLocation")));

	if (SkeletalMesh)
	{
		StaffMesh->SetupAttachment(SkeletalMesh, FName("WeaponSocket"));
		SpellLocationTemp->SetupAttachment(StaffMesh);
	}

	StaffMesh->SetGenerateOverlapEvents(false);
	StaffMesh->SetCollisionProfileName(FName(TEXT("NoCollision")));

	SpellComponent  = CreateDefaultSubobject<USpellComponent>(FName(TEXT("SpellComponent")));
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(FName(TEXT("CombatComponent")));
}

FVector ABasePlayerPawn::GetSpellCastLocation()
{
	return SpellLocationTemp->GetComponentLocation();
}

TArray<AActor*> ABasePlayerPawn::GetAliveEnemies()
{
	if (CurrentGameMode)
	{
		//return CurrentGameState->EnemySpawnerComp->GetAliveSpawns();
		return CurrentGameMode->EnemySpawnerComp->CachedAliveSpawns;
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Game mode not valid! CombatComponent.cpp -> GetAllEnemies()"));
		return TArray<AActor*>();
	}
}

AActor* ABasePlayerPawn::GetClosestEnemy()
{
	return UHelperFunctions::GetClosestActor(GetAliveEnemies(), GetActorLocation());
}

void ABasePlayerPawn::OnHit(float Damage)
{
	CombatComponent->TakeDamage(Damage);
}

void ABasePlayerPawn::OnDeath()
{
	GEngine->AddOnScreenDebugMessage(-1, 1.f, FColor::Orange, FString::Printf(TEXT("Player Died!")));

}

TArray<AActor*> ABasePlayerPawn::GetClosestEnemies(int32 NumOfEnemies)
{
	return UHelperFunctions::GetClosestActors(GetAliveEnemies(), GetActorLocation(), NumOfEnemies);
}
