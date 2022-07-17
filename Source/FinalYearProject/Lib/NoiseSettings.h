// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */
class FINALYEARPROJECT_API NoiseSettings
{
public:
	NoiseSettings();
	NoiseSettings(int octaves, float frequency,float strength);
	
	int octaves;
	float frequency;
	float strength;
};
