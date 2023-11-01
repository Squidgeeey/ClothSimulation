// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ProceduralMeshComponent.h"
#include "Cloth.generated.h"

class ClothParticle;
class ClothConstraint;

UCLASS()
class CLOTHSIM_API ACloth : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACloth();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//called in BeginPlay to create the cloth;
	void CreateParticles();
	void CreateConstraints();

	//called on tick to regenerate the mesh
	void GenerateMesh();

	void TryCreateTriangles(ClothParticle* _TopLeft, ClothParticle* _TopRight, ClothParticle* _BottomLeft, ClothParticle* _BottomRight, int _TopLeftIndex);

	//Properties

	
	//the cloth particles (forces can be applied)
	TArray<TArray<TSharedPtr<ClothParticle>>> ClothParticles;

	//Constraints between the particles
	TArray<TSharedPtr<ClothConstraint>> AllConstraints;

	//Root scene component
	UPROPERTY();
	USceneComponent* RootSceneComponent;


	//Procedural Mesh used for cloth
	UPROPERTY(EditDefaultsOnly, Category = Mesh);
	UProceduralMeshComponent* ClothMesh;

	UPROPERTY(EditDefaultsOnly, Category = Mesh);
	UMaterial* ClothMaterial;


	//Arrays of variables used to construct the cloth mesh
	TArray<FVector>				ClothVertices;
	TArray<int32>				ClothTriangles;
	TArray<FVector>				ClothNormals;
	TArray<FProcMeshTangent>	ClothTangents;
	TArray<FVector2D>			ClothUVs;
	TArray<FLinearColor>		ClothColors;

	//Cloth properties
	float ClothWidth = 200.0f;			//how wide in unreal units (cm)
	float ClothHeight = 200.0f;			//how tall in unreal units (cm)
	int NumHorzParticles = 30;
	int NumVertParticles = 30;
	float HorzDist;						//distance between each particle (calculated ahead of time)
	float VertDist;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
