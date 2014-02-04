/* 
 * File:   LightSensor.cpp
 * Author: Pedro Mariano
 * 
 * Created on 7 de Janeiro de 2014, 16:41
 */

#include <iostream>

#include "LightSensor.h"
#include "LightSource.h"

using namespace Enki;

LightSensor::LightSensor
	(double range, Enki::Robot* owner,
	 Enki::Vector relativePosition, double orientation,
	 double wavelength, double standardDeviationGaussianNoise)
	:
	LocalInteraction (range, owner),
	Component (owner, relativePosition, orientation),
	wavelength (wavelength),
	standardDeviationGaussianNoise (standardDeviationGaussianNoise)
{
}

LightSensor::LightSensor
	(double range, Enki::Robot* owner,
	 Enki::Vector relativePosition,
	 double wavelength, double standardDeviationGaussianNoise)
	:
	LocalInteraction (range, owner),
	Component (owner, relativePosition, Component::OMNIDIRECTIONAL),
	wavelength (wavelength),
	standardDeviationGaussianNoise (standardDeviationGaussianNoise)
{
}

LightSensor::LightSensor (const LightSensor& orig):
	LocalInteraction (orig.LocalInteraction::r, orig.LocalInteraction::owner),
	Component (orig),
	wavelength (orig.wavelength),
	standardDeviationGaussianNoise (orig.standardDeviationGaussianNoise)
{
}

LightSensor::~LightSensor ()
{
}

void LightSensor::
init (double dt, Enki::World* w)
{
	this->intensity = 0;
	Component::init ();
}

void LightSensor::
objectStep (double dt, Enki::World* w, Enki::PhysicalObject *po)
{

	LightSource *lightSource = dynamic_cast<LightSource *>(po);
	if (lightSource == NULL) {
		return ;
	}
	// std::cout << "light sensor interaction 2\n";
	if (lightSource->Component::owner == this->Component::owner) {
		return ;
	}
	// std::cout << "light sensor interaction 3\n";
	this->intensity += lightSource->getIntensityAt (this->absolutePosition, this->wavelength);
}

void LightSensor::
finalize (double dt, World* w)
{
	this->intensity = std::max (0.0, gaussianRand (this->intensity, this->standardDeviationGaussianNoise));
}
