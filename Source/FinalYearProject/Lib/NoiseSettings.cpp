// Fill out your copyright notice in the Description page of Project Settings.


#include "NoiseSettings.h"

NoiseSettings::NoiseSettings()
{
	octaves = 3;
	frequency = 0.01f;
	strength = 1.0f;
}

NoiseSettings::NoiseSettings(int octaves, float frequency, float strength)
{
	this->octaves = octaves;
	this->frequency = frequency;
	this->strength = strength;
}
