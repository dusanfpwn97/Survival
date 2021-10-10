// Fill out your copyright notice in the Description page of Project Settings.


#include "HelperFunctions.h"

FVector UHelperFunctions::GetRandomPointInCircle(FVector Center, float Radius)
{
	FVector Location;
	float rand = FMath::FRand();
	float RandPoint = rand * 2 * 3.1415f;
	Location.X = Center.X + Radius * FMath::Cos(RandPoint);
	Location.Y = Center.Y + Radius * FMath::Sin(RandPoint);
	return Location;
}