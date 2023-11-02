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

	//run repeatedly on timer
	void Update();

	//called in BeginPlay to create the cloth;
	void CreateParticles();
	void CreateConstraints();

	//called on tick to regenerate the mesh
	void GenerateMesh();

	//Applies wind to particles based on the surface alignment with wind
	void ApplyWind(ClothParticle* _TopLeft, ClothParticle* _TopRight, ClothParticle* _BotLeft, ClothParticle* _BotRight);

	void TryCreateTriangles(ClothParticle* _TopLeft, ClothParticle* _TopRight, ClothParticle* _BottomLeft, ClothParticle* _BottomRight, int _TopLeftIndex);

	//calculate triangle normal used for wind calculation
	FVector CalculateTriangleNormal(ClothParticle* _Particle1, ClothParticle* _Particle2, ClothParticle* _Particle3);

	void ApplyForceToTriangle(ClothParticle* _Particle1, ClothParticle* _Particle2, ClothParticle* _Particle3, FVector _Force);

	void CalculateWindVector();

	void CheckForGroundCollision();

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

	//simulation properties:
	FTimerHandle SimulationTimer;
	float TimeStep = 0.016f; //60 times per sec
	int ConstraintIterations = 3;	//num of times to iterate simulation each timestep

	//Wind properties
	UPROPERTY(BlueprintReadOnly, Category = Wind);
	FVector WindVector = { 0.0f, 30.0f, 0.0f };

	UPROPERTY(EditDefaultsOnly, Category = Wind);
	FRotator WindRotation = { 0.0f, 0.0f, 0.0f };

	UPROPERTY(EditDefaultsOnly, Category = Wind);
	float WinMinBaseStrength = 2;
	UPROPERTY(EditDefaultsOnly, Category = Wind);
	float WinMaxBaseStrength = 6;

	UPROPERTY(EditDefaultsOnly, Category = Wind);
	float WinMinAddedStrength = 0;
	UPROPERTY(EditDefaultsOnly, Category = Wind);
	float WinMaxAddedStrength = 7;

	UPROPERTY(EditDefaultsOnly, Category = Wind);
	float WindBaseStrengthOscillationTime = 0.5f;
	UPROPERTY(EditDefaultsOnly, Category = Wind);
	float WindAddedStrengthOscillationTime = 5.0f;

	//Collision
	UPROPERTY(EditDefaultsOnly, Category = Collision);
	float GroundHeight = 0.0f;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable)
	void ReleaseCloth();

//change the spread of cloth hooks, constrict amount is a multiplier on the cloth width (0 to 1 value)
	UFUNCTION(BlueprintCallable)
		void ConstrictCloth(float _ConstrictAmount);

	UFUNCTION(BlueprintCallable)
		void ResetCloth();

};
