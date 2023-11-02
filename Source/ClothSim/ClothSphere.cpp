// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothSphere.h"

// Sets default values
AClothSphere::AClothSphere()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("Sphere Mesh");
	RootComponent = StaticMesh;

	RescaleMesh();

}

// Called when the game starts or when spawned
void AClothSphere::BeginPlay()
{
	Super::BeginPlay();
	
}

void AClothSphere::RescaleMesh()
{
	
	StaticMesh->SetRelativeScale3D(FVector(Radius / 90));
	
}

