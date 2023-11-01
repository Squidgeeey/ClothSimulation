// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class ClothParticle;

/**
 * 
 */
class CLOTHSIM_API ClothConstraint
{
public:
	ClothConstraint(TSharedPtr<ClothParticle> _ParticleA, TSharedPtr<ClothParticle> _ParticleB);
	~ClothConstraint();

private:

	TWeakPtr<ClothParticle> ParticleA;
	TWeakPtr<ClothParticle> ParticleB;

	float RestingDistance = 10.0f;

};
