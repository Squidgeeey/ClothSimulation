// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothParticle.h"
#include "ClothConstraint.h"



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
	for (TWeakPtr<ClothConstraint> Iter : _OtherParticle->GetConstraints())
	{
		if (Constraints.Contains(Iter) && !Iter.Pin()->GetIsInterwoven())
		{
			return true;
		}
	}

	return false;
}

void ClothParticle::SetPosition(FVector _Position)
{
	Position = _Position;
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
	if (!FixedInPlace && !OnGround)
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

void ClothParticle::SetFixedInPlace(bool _FixedInPlace)
{
	FixedInPlace = _FixedInPlace;
}

void ClothParticle::CheckForGroundCollision(float _GroundHeight)
{
	if (Position.Z <= _GroundHeight)
	{
		Position.Z = _GroundHeight;
		OnGround = true;
	}
	else if(Position.Z > _GroundHeight + GroundStayDistance)
	{
		OnGround = false;
	}
}

void ClothParticle::CheckForSphereCollision(FVector _Location, float _Radius)
{
	FVector VecFromSphere = Position - _Location;
	float DistanceFromSphereCenter = VecFromSphere.Length();

	if (DistanceFromSphereCenter < _Radius)
	{
		//found collision with sphere
		VecFromSphere.Normalize();
		VecFromSphere *= _Radius - DistanceFromSphereCenter + 3;
		OffsetPosition(VecFromSphere);
	}
}


