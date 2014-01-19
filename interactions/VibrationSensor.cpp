#include <algorithm>
#include <iostream>

#include "VibrationSensor.h"
#include "VibrationSource.h"

using namespace Enki;

VibrationSensor::
VibrationSensor (double range, Robot* owner, Vector relativePosition, double orientation, double maxMeasurableAmplitude, double maxMeasurableFrequency):
	LocalInteraction (range, owner),
	Component (owner, relativePosition, orientation), 
	maxMeasurableAmplitude (maxMeasurableAmplitude),
	maxMeasurableFrequency (maxMeasurableFrequency),
	amplitude (0),
	frequency (0)
{
}

VibrationSensor::VibrationSensor (const VibrationSensor& orig):
	LocalInteraction (orig.LocalInteraction::r, orig.LocalInteraction::owner),
	Component (orig),
	maxMeasurableAmplitude (orig.maxMeasurableAmplitude),
	maxMeasurableFrequency (orig.maxMeasurableFrequency),
	amplitude (0),
	frequency (0)
{
}

VibrationSensor::~VibrationSensor ()
{
}

void VibrationSensor::
init (double dt, Enki::World* w)
{
	this->amplitude = 0;
	this->frequency = 0;
	Component::init ();
	// std::cout << "initialisation step for vibration sensor " << this->value << '\n';
}

void VibrationSensor::
objectStep (double dt, Enki::World* w, Enki::PhysicalObject *po)
{
	VibrationSource *vibrationSource = dynamic_cast<VibrationSource *>(po);
	if (vibrationSource == NULL) {
		return ;
	}
	if (vibrationSource->Component::owner == this->Component::owner) {
#ifdef __DEBUG__
		std::cout << "self-inhibit "  << vibrationSource->value << '\n';
#endif
		return ;
	}

	Enki::Vector my = this->Component::owner->pos + this->relativePosition;
	Enki::Vector ot = po->pos;


#ifdef __DEBUG__
	std::cout
		<< "interaction between "
		<< this->value << '@' << my
		<< " and "
		<< vibrationSource->value << '@' << ot
		<< '\n';
#endif

	this->amplitude += vibrationSource->getAmplitude (this->absolutePosition, dt);
	//TODO compute frequency from wave sampling
}

void VibrationSensor::
finalize (double dt, Enki::World* w)
{
	this->amplitude = std::min (this->amplitude, this->maxMeasurableAmplitude);
	this->frequency = std::min (this->frequency, this->maxMeasurableFrequency);
}


