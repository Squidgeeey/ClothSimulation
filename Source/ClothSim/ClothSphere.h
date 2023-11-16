// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ClothSphere.generated.h"

UCLASS()
class CLOTHSIM_API AClothSphere : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClothSphere();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = SphereDetails)
	float Radius = 100;

	void RescaleMesh();

	UPROPERTY(EditDefaultsOnly, Category = SphereDetails)
		UStaticMeshComponent* StaticMesh = nullptr;

public:	

	float GetRadius() { return Radius; }

	void SetRadius(float _Radius);

};
