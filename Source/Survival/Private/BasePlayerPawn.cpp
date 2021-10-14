// Fill out your copyright notice in the Description page of Project Settings.


#include "BasePlayerPawn.h"
#include "BaseEnemy.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "BaseSpell.h"
#include "BaseSpellManager.h"
//#include "CombatComponent.h"
#include "HelperFunctions.h"
#include "BaseGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "BaseGameMode.h"
#include "EnemySpawner.h"

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

		//ICombatInterface::Execute_OnCollidedWithSpell(OtherActor, this);
	}
}

void ABasePlayerPawn::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{

}

void ABasePlayerPawn::OnCollidedWithEnemy_Implementation(ABaseEnemy* Enemy)
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

	//CombatComponent = CreateDefaultSubobject<UCombatComponent>(FName(TEXT("CombatComponent")));
}

FVector ABasePlayerPawn::GetSpellCastLocation_Implementation()
{
	return MainCollider->GetComponentLocation();
}
/*
* 
UCombComponent* ABasePlayerPawn::GetCombatComponent() const
{
	return CombatComponent;
}
*/
void ABasePlayerPawn::AddNewSpell(TSoftClassPtr<UBaseSpellManager> SpellManagerClass)
{
	UClass* SpellManagerClassToUse;

	if (SpellManagerClass)
	{
		SpellManagerClassToUse = SpellManagerClass.LoadSynchronous();
		if (!SpellManagerClassToUse)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SpellManager class is null! Pawn.cpp -> AddSpell"));
			return;
		}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("SpellManager class is null! Pawn.cpp -> AddSpell"));
		return;
	}
	
	UBaseSpellManager* NewSpell = NewObject<UBaseSpellManager>(this, SpellManagerClassToUse); //*
	
	//NewSpell->OnComponentCreated();
	NewSpell->RegisterComponent();
	SpellManagers.Add(NewSpell);
}

TArray<AActor*> ABasePlayerPawn::GetAliveEnemies_Implementation()
{
	
	if (!CurrentGameMode)
	{
		GEngine->AddOnScreenDebugMessage(-1, 1.0f, FColor::Yellow, TEXT("Game mode not valid! CombatComponent.cpp -> GetAllEnemies()"));
		return TArray<AActor*>();
	}

	return UHelperFunctions::GetAllAliveActors(CurrentGameMode->GetEnemySpawner()->GetAllSpawns());
}

AActor* ABasePlayerPawn::GetClosestEnemy_Implementation()
{
	return UHelperFunctions::GetClosestActor(GetAliveEnemies_Implementation(), GetActorLocation());
}
