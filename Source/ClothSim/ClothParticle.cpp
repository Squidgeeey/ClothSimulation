// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothParticle.h"



ClothParticle::ClothParticle(FVector _Position, bool _FixedInPlace)
{
	Position = _Position;
	OldPosition = _Position;
	FixedInPlace = _FixedInPlace;
}

ClothParticle::~ClothParticle()
{
}

void ClothParticle::AddConstraint(TSharedPtr<ClothConstraint> _ClothConstraint)
{
	Constraints.Add(_ClothConstraint);
}

bool ClothParticle::SharesConstraint(ClothParticle* _OtherParticle)
{
	for (auto Iter : _OtherParticle->GetConstraints())
	{
		if (Constraints.Contains(Iter))
		{
			return true;
		}
	}

	return false;
}

void ClothParticle::ApplyForce(FVector _Force)
{
	Acceleration += _Force / Mass;
}

void ClothParticle::ApplyGravity(float _DeltaTime)
{
	ApplyForce(FVector(0,0,-981.0f * Mass) * _DeltaTime); //Gravity in cm (981cm, 9.81m)
}

void ClothParticle::Update(float _DeltaTime)
{
	if (!FixedInPlace)
	{
		FVector TempPos = Position;
		Position += (Position - OldPosition) * (1.0f - Damping) + Acceleration * _DeltaTime;
		OldPosition = TempPos;
	}

	Acceleration = FVector::ZeroVector;
	
}

void ClothParticle::OffsetPosition(FVector _Offset)
{
	Position += _Offset;
}


