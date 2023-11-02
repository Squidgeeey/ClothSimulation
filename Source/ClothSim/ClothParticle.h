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
	ClothParticle(FVector _Position, bool _FixedInPlace = false);
	~ClothParticle();

	void AddConstraint(TSharedPtr<ClothConstraint> _ClothConstraint);

	bool SharesConstraint(ClothParticle* _OtherParticle);

	TArray<TWeakPtr<ClothConstraint>> GetConstraints() { return Constraints; }

	FVector GetPosition() { return Position; }

	//forces will be added up as acceleration to be applied for verlet integration.
	void ApplyForce(FVector _Force);

	void ApplyGravity(float _DeltaTime);

	//Do some verlet integration to move particle
	void Update(float _DeltaTime);

	void OffsetPosition(FVector _Offset);

	bool IsFixedInPlace() {return FixedInPlace; }

private:
	TArray<TWeakPtr<ClothConstraint>> Constraints; 

	FVector Position = { 0.0f, 0.0f, 0.0f };
	FVector OldPosition = { 0.0f, 0.0f, 0.0f };
	FVector Acceleration = { 0.0f, 0.0f, 0.0f };
	float Mass = 1.0f;
	float Damping = 0.01f;

	bool FixedInPlace = false;

};
