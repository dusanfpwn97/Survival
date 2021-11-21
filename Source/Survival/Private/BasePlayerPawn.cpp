// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerPawn.h"
#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "BaseSpell.h"
#include "BaseSpellManager.h"
//#include "CombatComponent.h"
#include "HelperFunctions.h"
#include "BaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BaseGameMode.h"
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
		CurrentGameMode = Cast<ABaseGameMode>(UGameplayStatics::GetGameMode(World));
		if (!CurrentGameMode) GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Game mode not valid! CombatComponent.cpp -> BeginPlay"));
	}
	else GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("World not valid! CombatComponent.cpp -> BeginPlay"));

	FSpellInfo Info;
	Info.Element = Element::AIR;
	Info.CastType = CastType::FLICK;
	Info.Cooldown = 0.35f;
	Info.Speed = 1000.f;
	Info.Radius = 20.f;
	Info.TargetMode = TargetMode::CLOSEST;
	//SpellComponent->AddNewSpell(Info);

	Info.Element = Element::ELECTRICITY;
	Info.CastType = CastType::PROJECTILE;
	Info.Cooldown = 0.12f;
	Info.Speed = 990.f;
	Info.Radius = 50.f;
	Info.TargetMode = TargetMode::CLOSEST;
	SpellComponent->AddNewSpell(Info);


	Info.Element = Element::FIRE;
	Info.CastType = CastType::STORM;
	Info.Cooldown = 0.12f;
	Info.Speed = 1400.f;
	Info.Radius = 30.f;
	Info.TargetMode = TargetMode::CLOSEST;
	SpellComponent->AddNewSpell(Info);
	 
	Info.Element = Element::ICE;
	Info.CastType = CastType::SHIELD;
	Info.Cooldown = 2.11f;
	Info.Speed = 1000.f;
	Info.Radius = 70.f;
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

	SpellComponent = CreateDefaultSubobject<USpellComponent>(FName(TEXT("SpellComponent")));

	//CombatComponent = CreateDefaultSubobject<UCombatComponent>(FName(TEXT("CombatComponent")));
}

FVector ABasePlayerPawn::GetSpellCastLocation_Implementation()
{
	return SpellLocationTemp->GetComponentLocation();
}


TArray<AActor*> ABasePlayerPawn::GetAliveEnemies()
{
	if (CurrentGameMode)
	{
		return UHelperFunctions::GetAllAliveActors(CurrentGameMode->GetEnemySpawner()->GetAllSpawns());
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
