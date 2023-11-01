// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothParticle.h"



ClothParticle::ClothParticle(FVector _Position)
{
	Position = _Position;
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
