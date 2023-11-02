// Fill out your copyright notice in the Description page of Project Settings.


#include "Cloth.h"
#include "ClothSphere.h"
#include "ClothParticle.h"
#include "ClothConstraint.h"
#include "KismetProceduralMeshLibrary.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"

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

	//find all clothspheres and store in an array
	TArray<AActor*> FoundActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AClothSphere::StaticClass(), FoundActors);

	for (AActor* ThisActor : FoundActors)
	{
		if (AClothSphere* FoundSphere = Cast<AClothSphere>(ThisActor))
		{
			ClothSpheres.Add(FoundSphere);
		}
	}

	ClothMesh->SetMaterial(0, ClothMaterial);

	CreateParticles();
	CreateConstraints();

	//create looping timer to simulate cloth
	GetWorldTimerManager().SetTimer(SimulationTimer, this, &ACloth::Update, TimeStep, true, 0.0f);
	
}

void ACloth::Update()
{
	//Update all particles
	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		for (int Horz = 0; Horz < NumHorzParticles; Horz++)
		{
			ClothParticles[Vert][Horz]->ApplyGravity(TimeStep);

			//Apply Wind to each particle
			if (Vert < NumVertParticles - 1 && Horz < NumHorzParticles - 1)
			{
				ApplyWind(ClothParticles[Vert][Horz].Get(),			//top left
					ClothParticles[Vert][Horz + 1].Get(),			//top right
					ClothParticles[Vert + 1][Horz].Get(),			//bot left
					ClothParticles[Vert + 1 ][Horz + 1].Get());		//bot right
			}


			ClothParticles[Vert][Horz]->Update(TimeStep);
		}
	}


	for (int i = 0; i < ConstraintIterations; i++)
	{
		for (auto CurrentConstraint : AllConstraints)
		{
			CurrentConstraint->Update(TimeStep);
		}
	}

	CheckForCollision();


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
		for (int Horz = 0; Horz < NumHorzParticles; Horz++)
		{
			FVector ParticlePos = { StartPos.X + Horz * HorzDist, StartPos.Y, StartPos.Z - Vert * VertDist };

			bool FixedInPlace = false;
			
			FixedInPlace = (Vert == 0 && (Horz == 0 || Horz == NumHorzParticles - 1 || Horz % 6 == 0));

			//create new particle at position
			TSharedPtr<ClothParticle> NewParticle(new ClothParticle(ParticlePos, FixedInPlace));

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
		for (int Horz = 0; Horz < NumHorzParticles; Horz++)
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

void ACloth::ApplyWind(ClothParticle* _TopLeft, ClothParticle* _TopRight, ClothParticle* _BotLeft, ClothParticle* _BotRight)
{
	FVector NormalWind = WindVector;
	NormalWind.Normalize();

	if (_TopLeft->SharesConstraint(_TopRight) && _TopLeft->SharesConstraint(_BotLeft))				//top left triangle
	{
		FVector SurfaceNormal = CalculateTriangleNormal(_TopLeft, _TopRight, _BotLeft);

		FVector FinalWindForce = WindVector * abs(FVector::DotProduct(NormalWind, SurfaceNormal));
		ApplyForceToTriangle(_TopLeft, _TopRight, _BotLeft, FinalWindForce);

		if (_BotRight->SharesConstraint(_TopRight) && _BotRight->SharesConstraint(_BotLeft))		//bottom right triangle
		{
			SurfaceNormal = CalculateTriangleNormal(_BotRight, _BotLeft,  _TopRight);
			FinalWindForce = WindVector * abs(FVector::DotProduct(NormalWind, SurfaceNormal));

			ApplyForceToTriangle(_BotRight, _TopRight, _BotLeft, FinalWindForce);
		}
	}
	else if (_TopLeft->SharesConstraint(_BotLeft) && _TopLeft->SharesConstraint(_BotRight))			//bottom left triangle
	{
		FVector SurfaceNormal = CalculateTriangleNormal(_TopLeft, _BotRight, _BotLeft);

		FVector FinalWindForce = WindVector * abs(FVector::DotProduct(NormalWind, SurfaceNormal));
		ApplyForceToTriangle(_TopLeft, _BotLeft, _BotRight, FinalWindForce);

		if (_TopLeft->SharesConstraint(_TopRight) && _TopLeft->SharesConstraint(_BotRight))			//top right triangle
		{
			SurfaceNormal = CalculateTriangleNormal(_TopLeft, _TopRight, _BotRight);
			FinalWindForce = WindVector * abs(FVector::DotProduct(NormalWind, SurfaceNormal));

			ApplyForceToTriangle(_TopLeft, _TopRight, _BotRight, FinalWindForce);
		}
	}
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

FVector ACloth::CalculateTriangleNormal(ClothParticle* _Particle1, ClothParticle* _Particle2, ClothParticle* _Particle3)
{
	FVector Vec1 = _Particle2->GetPosition() - _Particle1->GetPosition();
	FVector Vec2 = _Particle3->GetPosition() - _Particle1->GetPosition();

	FVector Normal = FVector::CrossProduct(Vec1, Vec2);
	Normal.Normalize();

	return Normal;
}

void ACloth::ApplyForceToTriangle(ClothParticle* _Particle1, ClothParticle* _Particle2, ClothParticle* _Particle3, FVector _Force)
{
	_Particle1->ApplyForce(_Force);
	_Particle2->ApplyForce(_Force);
	_Particle3->ApplyForce(_Force);
}

void ACloth::CalculateWindVector()
{
	WindVector = WindRotation.Vector();

	//Oscillate base strength of wind
	float Alpha = (FMath::Sin((GetGameTimeSinceCreation() * PI) / WindBaseStrengthOscillationTime) + 1) / 2;
	float WindBaseStrength = FMath::Lerp(WinMinBaseStrength, WinMaxBaseStrength, Alpha);
	
	//Oscillate added wind strength
	Alpha = (FMath::Sin((GetGameTimeSinceCreation() * PI) / WindAddedStrengthOscillationTime) + 1) / 2;
	float WindAddedStrength = FMath::Lerp(WinMinAddedStrength, WinMaxAddedStrength, Alpha);

	WindVector *= (WindBaseStrength + WindAddedStrength);
}

void ACloth::CheckForCollision()
{
	for (int Vert = 0; Vert < NumVertParticles; Vert++)
	{
		for (int Horz = 0; Horz < NumHorzParticles; Horz++)
		{
			ClothParticles[Vert][Horz]->CheckForGroundCollision(GroundHeight - ClothMesh->GetComponentLocation().Z);

			for (AClothSphere* FoundSphere : ClothSpheres)
			{


				ClothParticles[Vert][Horz]->CheckForSphereCollision(FoundSphere->GetActorLocation() - ClothMesh->GetComponentLocation(), FoundSphere->GetRadius());
			}
		}
	}
}

// Called every frame
void ACloth::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	CalculateWindVector();

	GenerateMesh();

}

void ACloth::ReleaseCloth()
{
	for (int Horz = 0; Horz < NumHorzParticles; Horz++)
	{
		ClothParticles[0][Horz]->SetFixedInPlace(false);
	}
}

void ACloth::ConstrictCloth(float _ConstrictAmount)
{
	float ConstrictedWidth = ClothWidth * _ConstrictAmount;
	float ConstrictedHorzDist = ConstrictedWidth / (NumHorzParticles - 1);

	FVector StartPos = { -(((NumHorzParticles - 1) * ConstrictedHorzDist) / 2), 0.0f, ((NumVertParticles - 1) * VertDist) / 2 };

	for (int Horz = 0; Horz < NumHorzParticles; Horz++)
	{
		FVector ParticlePos = { StartPos.X + Horz * ConstrictedHorzDist, StartPos.Y, StartPos.Z };

		if (ClothParticles[0][Horz]->IsFixedInPlace())
		{
			ClothParticles[0][Horz]->SetPosition(ParticlePos);
		}
	}
}

void ACloth::ResetCloth()
{
	ClothParticles.Empty();
	AllConstraints.Empty();

	CreateParticles();
	CreateConstraints();
}


