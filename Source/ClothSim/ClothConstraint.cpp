// Fill out your copyright notice in the Description page of Project Settings.


#include "ClothConstraint.h"
#include "ClothParticle.h"

ClothConstraint::ClothConstraint(TSharedPtr<ClothParticle> _ParticleA, TSharedPtr<ClothParticle> _ParticleB)
{
	ParticleA = _ParticleA;
	ParticleB = _ParticleB;
}

ClothConstraint::~ClothConstraint()
{
}
