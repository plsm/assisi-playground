#include <algorithm>
#include <iostream>

#include "VibrationSensor.h"
#include "VibrationSource.h"
#include "WaveVibrationSource.h"
#include "ExtendedRobot.h"

using namespace Enki;

static bool measureComparator (const VibrationSensor::Measure &a, const VibrationSensor::Measure &b)
{
	return a.id < b.id;
}

VibrationSensor::
VibrationSensor
	(double range, Robot* owner,
	 Vector relativePosition, double orientation,
	 double maxMeasurableFrequency, double amplitudeStandardDeviationGaussianNoise, double frequencyStandardDeviationGaussianNoise)
	:
	LocalInteraction (range, owner),
	Component (owner, relativePosition, orientation), 
	maxMeasurableFrequency (maxMeasurableFrequency),
	amplitudeStandardDeviationGaussianNoise (amplitudeStandardDeviationGaussianNoise),
	frequencyStandardDeviationGaussianNoise (frequencyStandardDeviationGaussianNoise),
	// amplitudeValues (),
	// frequencyValues (),
	measureData (),
	totalElapsedTime (0)
{
}

VibrationSensor::VibrationSensor (const VibrationSensor& orig):
	LocalInteraction (orig.LocalInteraction::r, orig.LocalInteraction::owner),
	Component (orig),
	maxMeasurableFrequency (orig.maxMeasurableFrequency),
	amplitudeStandardDeviationGaussianNoise (orig.amplitudeStandardDeviationGaussianNoise),
	frequencyStandardDeviationGaussianNoise (orig.frequencyStandardDeviationGaussianNoise),
	// amplitudeValues (0),
	// frequencyValues (0),
	measureData (),
	totalElapsedTime (0)
{
}

VibrationSensor::~VibrationSensor ()
{
}

void VibrationSensor::
init (double dt, Enki::World* w)
{
	this->measureData.clear ();
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

#ifdef __DEBUG__
	Enki::Vector my = this->Component::owner->pos + this->relativePosition;
	Enki::Vector ot = po->pos;
	std::cout
		<< "interaction between "
		<< this->value << '@' << my
		<< " and "
		<< vibrationSource->value << '@' << ot
		<< '\n';
#endif

	double value;
	WaveVibrationSource *waveVibrationSource = dynamic_cast<WaveVibrationSource *>(po);
	if (waveVibrationSource != NULL) {
		Measure measure;
		value = waveVibrationSource->getFrequency ();
		value = std::min (value, this->maxMeasurableFrequency);
		value = gaussianRand (value, value * this->frequencyStandardDeviationGaussianNoise);
		measure.frequency = value;
		// this->frequencyValues.push_back (value);
		value = waveVibrationSource->getWaveAt (this->absolutePosition, this->totalElapsedTime);
		value = gaussianRand (value, fabs (value * this->amplitudeStandardDeviationGaussianNoise));
		measure.amplitude = value;
		// this->amplitudeValues.push_back (value);
		measure.maxAbsAmplitude = waveVibrationSource->getMaxAbsoluteAmplitudeAt (this->absolutePosition);

		const ExtendedRobot *owner = dynamic_cast<const ExtendedRobot *> (vibrationSource->Component::owner);
		if (owner != NULL) {
			measure.id = owner->id;
		}
		else {
			measure.id = -1;
		}
		this->measureData.push_back (measure);
	}
}

void VibrationSensor::
finalize (double dt, Enki::World* w)
{
	this->totalElapsedTime += dt;
	std::sort (this->measureData.begin (), this->measureData.end (), measureComparator);
}


