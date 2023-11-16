// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ClothHUD.generated.h"

/**
 * 
 */
UCLASS()
class CLOTHSIM_API AClothHUD : public AHUD
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintImplementableEvent)
		void SetClothonWidgets(class ACloth* _Cloth);
	
};
