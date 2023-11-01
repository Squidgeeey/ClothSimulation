// Fill out your copyright notice in the Description page of Project Settings.


#include "Cloth.h"
#include "ClothParticle.h"
#include "ClothConstraint.h"
#include "KismetProceduralMeshLibrary.h"

// Sets default values
ACloth::ACloth()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	RootSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root Component"));
	RootComponent = RootSceneComponent;

	ClothMesh = CreateDefaultSubobject<UProceduralMeshComponent>(TEXT("Procedural Mesh"));
	ClothMesh->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void ACloth::BeginPlay()
{
	Super::BeginPlay();


	ClothMesh->SetMaterial(0, ClothMaterial);

	CreateParticles();
	CreateConstraints();
	
}

void ACloth::CreateParticles()
{
	//place particles evenly
	HorzDist = ClothWidth / (NumHorzParticles - 1);
	VertDist = ClothHeight / (NumVertParticles - 1);

	FVector StartPos = { -((NumHorzParticles - 1) * HorzDist) / 2, 0.0f, ((NumVertParticles - 1) * VertDist) / 2 };

	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		TArray<TSharedPtr<ClothParticle>> ParticleRow;
		for (int Horz = 0; Horz < NumVertParticles; Horz++)
		{
			FVector ParticlePos = { StartPos.X + Horz * HorzDist, StartPos.Y, StartPos.Z - Vert * VertDist };


			//create new particle at position
			TSharedPtr<ClothParticle> NewParticle(new ClothParticle(ParticlePos));

			//add particle to row
			ParticleRow.Add(NewParticle);
		}

		ClothParticles.Add(ParticleRow);
	}
}

void ACloth::CreateConstraints()
{
	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		for (int Horz = 0; Horz < NumVertParticles; Horz++)
		{
			if (Vert < NumVertParticles - 1)
			{
				//Make a verticle Constraint
				TSharedPtr<ClothConstraint> NewConstraint(new ClothConstraint(ClothParticles[Vert][Horz], ClothParticles[Vert + 1][Horz]));

				AllConstraints.Add(NewConstraint);
				ClothParticles[Vert][Horz]->AddConstraint(NewConstraint);
				ClothParticles[Vert + 1][Horz]->AddConstraint(NewConstraint);
			}

			if (Horz < NumHorzParticles - 1)
			{
				//Make a verticle Constraint
				TSharedPtr<ClothConstraint> NewConstraint(new ClothConstraint(ClothParticles[Vert][Horz], ClothParticles[Vert][Horz + 1]));

				AllConstraints.Add(NewConstraint);
				ClothParticles[Vert][Horz]->AddConstraint(NewConstraint);
				ClothParticles[Vert][Horz + 1]->AddConstraint(NewConstraint);
			}
		}
	}


}

void ACloth::GenerateMesh()
{

	ClothVertices.Reset();
	ClothTriangles.Reset();
	ClothNormals.Reset();
	ClothTangents.Reset();
	ClothUVs.Reset();
	ClothColors.Reset();

	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		for (int Horz = 0; Horz < NumVertParticles; Horz++)
		{
			ClothVertices.Add(ClothParticles[Vert][Horz]->GetPosition());

			ClothColors.Add(FLinearColor::White);

			ClothUVs.Add(FVector2D((Horz * HorzDist) / ClothWidth, (Vert * VertDist) / ClothHeight));
		}
	}

	for (int Vert = 0; Vert < NumVertParticles - 1; Vert++)
	{
		for (int Horz = 0; Horz < NumVertParticles - 1; Horz++)
		{
			TryCreateTriangles(ClothParticles[Vert][Horz].Get(),	//top left
				ClothParticles[Vert][Horz + 1].Get(),				//top right
				ClothParticles[Vert + 1][Horz].Get(),				//bot left
				ClothParticles[Vert + 1][Horz + 1].Get(),			//bot right
				Vert * NumHorzParticles + Horz);					//Vert Index
		}
	}

	UKismetProceduralMeshLibrary::CalculateTangentsForMesh(ClothVertices, ClothTriangles, ClothUVs, ClothNormals, ClothTangents);

	ClothMesh->CreateMeshSection_LinearColor(0, ClothVertices, ClothTriangles, ClothNormals, ClothUVs, ClothColors, ClothTangents, false);
}

void ACloth::TryCreateTriangles(ClothParticle* _TopLeft, ClothParticle* _TopRight, ClothParticle* _BottomLeft, ClothParticle* _BottomRight, int _TopLeftIndex)
{
	//Check if each particle shares a constraint
	//if they do, try make triangles

	int TopLeftIndex = _TopLeftIndex;
	int TopRightIndex = _TopLeftIndex + 1;
	int BotLeftIndex = _TopLeftIndex + NumHorzParticles;
	int BotRightIndex = _TopLeftIndex + NumHorzParticles + 1;

	if (_TopLeft->SharesConstraint(_TopRight) && _TopLeft->SharesConstraint(_BottomLeft))
	{
		ClothTriangles.Add(TopLeftIndex);
		ClothTriangles.Add(TopRightIndex);
		ClothTriangles.Add(BotLeftIndex);

		if (_BottomRight->SharesConstraint(_TopRight) && _BottomRight->SharesConstraint(_BottomLeft))
		{
			ClothTriangles.Add(TopRightIndex);
			ClothTriangles.Add(BotRightIndex);
			ClothTriangles.Add(BotLeftIndex);
		}
	}
	else if (_TopLeft->SharesConstraint(_BottomLeft) && _BottomLeft->SharesConstraint(_BottomRight))
	{
		ClothTriangles.Add(TopLeftIndex);
		ClothTriangles.Add(BotRightIndex);
		ClothTriangles.Add(BotLeftIndex);

		if (_TopRight->SharesConstraint(_TopLeft) && _TopRight->SharesConstraint(_BottomRight))
		{
			ClothTriangles.Add(TopRightIndex);
			ClothTriangles.Add(BotRightIndex);
			ClothTriangles.Add(TopLeftIndex);
		}

	}

}

// Called every frame
void ACloth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	GenerateMesh();

}

