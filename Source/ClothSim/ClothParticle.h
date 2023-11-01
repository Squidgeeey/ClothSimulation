// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

class ClothConstraint;

/**
 * 
 */
class CLOTHSIM_API ClothParticle
{
public:
	ClothParticle(FVector _Position);
	~ClothParticle();

	void AddConstraint(TSharedPtr<ClothConstraint> _ClothConstraint);

	bool SharesConstraint(ClothParticle* _OtherParticle);

	TArray<TWeakPtr<ClothConstraint>> GetConstraints() { return Constraints; }

	FVector GetPosition() { return Position; }

private:
	TArray<TWeakPtr<ClothConstraint>> Constraints; 

	FVector Position = { 0.0f, 0.0f, 0.0f };
	FVector OldPosition = { 0.0f, 0.0f, 0.0f };
	FVector Acceleration = { 0.0f, 0.0f, 0.0f };
	float Mass = 1.0f;
	float Damping = 0.01f;

};
