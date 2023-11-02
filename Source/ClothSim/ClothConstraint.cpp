// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothConstraint.h"
#include "ClothParticle.h"

ClothConstraint::ClothConstraint(TSharedPtr<ClothParticle> _ParticleA, TSharedPtr<ClothParticle> _ParticleB)
{
	ParticleA = _ParticleA;
	ParticleB = _ParticleB;

	RestingDistance = FVector::Dist(_ParticleA->GetPosition(), _ParticleB->GetPosition());
}

ClothConstraint::~ClothConstraint()
{
}

void ClothConstraint::Update(float _DeltaTime)
{
	FVector CurrentOffset = ParticleB.Pin()->GetPosition() - ParticleA.Pin()->GetPosition();

	FVector Correction = CurrentOffset * (1.0f - RestingDistance / CurrentOffset.Size());
	FVector HalfCorrection = Correction * 0.5f;

	if (!ParticleA.Pin()->IsFixedInPlace() && !ParticleB.Pin()->IsFixedInPlace())
	{
		ParticleA.Pin()->OffsetPosition(HalfCorrection);
		ParticleB.Pin()->OffsetPosition(-HalfCorrection);
	}
	else if (!ParticleA.Pin()->IsFixedInPlace())
	{
		ParticleA.Pin()->OffsetPosition(Correction);
	}
	else if (!ParticleB.Pin()->IsFixedInPlace())
	{
		ParticleB.Pin()->OffsetPosition(-Correction);
	}


}
