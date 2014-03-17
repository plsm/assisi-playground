/*

 */

#include <cmath>

#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include <enki/PhysicalEngine.h>

#include <iostream>

#include "Bee.h"

const double pi = boost::math::constants::pi<double>();

// Signum function
template <typename T>
int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

namespace Enki
{
	const double Bee::LENGTH = 0.016;

	const double Bee::WIDTH = 0.008;

	const double Bee::HEIGHT = 0.4;

	const double Bee::MAX_SPEED = 0.001;

	const int Bee::NUMBER_LIGHT_SENSORS = 4;

	const double Bee::LIGHT_SENSOR_WAVELENGTH = 600;

	const Point Bee::LIGHT_SENSOR_POSITIONS[] = {
		Point (Bee::LENGTH * 1.0, +Bee::WIDTH * 1.0),
		Point (Bee::LENGTH * 0.5, +Bee::WIDTH * 0.7),
		Point (Bee::LENGTH * 0.5, -Bee::WIDTH * 0.7),
		Point (Bee::LENGTH * 1.0, -Bee::WIDTH * 1.0)
	};

	const double Bee::LIGHT_SENSOR_NOISE = 0.0;

	const int Bee::NUMBER_VIBRATION_SENSORS = 6;

	const double Bee::VIBRATION_SENSOR_MAX_AMPLITUDE = 100;

	const double Bee::VIBRATION_SENSOR_MAX_FREQUENCY = 2;

	const double Bee::VIBRATION_INTENSITY_SENSOR_MIN_MEASURABLE_INTENSITY = 0.00001;

	const double Bee::VIBRATION_INTENSITY_SENSOR_MAX_MEASURABLE_INTENSITY = 1;

	const Point Bee::VIBRATION_SENSOR_POSITIONS[] = {
		Point (+Bee::LENGTH * 1.0, +Bee::WIDTH * 1.0),
		Point (+Bee::LENGTH * 0.0, +Bee::WIDTH * 1.0),
		Point (-Bee::LENGTH * 1.0, +Bee::WIDTH * 1.0),
		Point (+Bee::LENGTH * 1.0, -Bee::WIDTH * 1.0),
		Point (+Bee::LENGTH * 0.0, -Bee::WIDTH * 1.0),
		Point (-Bee::LENGTH * 1.0, -Bee::WIDTH * 1.0)
	};


	//	const double Bee::VIBRATION_INTENSITY_SENSOR_NOISE = 0.001;
	//const double Bee::VIBRATION_INTENSITY_SENSOR_NOISE = 0.002;
	const double Bee::VIBRATION_INTENSITY_SENSOR_NOISE = 0.004;

	const double Bee::HYSTERESIS_DECISION = 30;

	Bee::Bee (Point *position) :
		DifferentialWheeled(0.4, 2, 0.0),
		range_sensors(5),
		clockDecision (-1)
	{
		this->pos = *position;
		// Set shape & color

		double m = 1;   // Body mass

		//setRectangular(len, w, h, 1);
		Polygone footprint;
		footprint.push_back (Point ( Bee::LENGTH/2,                           Bee::WIDTH/4));
		footprint.push_back (Point ( Bee::LENGTH/2 - Bee::WIDTH/(4*sqrt(2)),  Bee::WIDTH/2));
		footprint.push_back (Point (-Bee::LENGTH/2 + Bee::WIDTH/(4*sqrt(2)),  Bee::WIDTH/2));
		footprint.push_back (Point (-Bee::LENGTH/2,                           Bee::WIDTH/4));
		footprint.push_back (Point (-Bee::LENGTH/2,                          -Bee::WIDTH/4));
		footprint.push_back (Point (-Bee::LENGTH/2 + Bee::WIDTH/(2*sqrt(2)), -Bee::WIDTH/2));
		footprint.push_back (Point ( Bee::LENGTH/2 - Bee::WIDTH/(2*sqrt(2)), -Bee::WIDTH/2));
		footprint.push_back (Point ( Bee::LENGTH/2,                          -Bee::WIDTH/4));
		PhysicalObject::Hull hull (PhysicalObject::Part (footprint, Bee::HEIGHT));
		setCustomHull (hull, m);
		setColor(Color(0.93,0.79,0,1));

		// Set other physical properties
		PhysicalObject::dryFrictionCoefficient = 2.5;

		this->range_sensors.resize (5); 
		int i = 0;
		for (double a = -pi/2; i < 5; i++, a += pi/4) 
			{
            range_sensors[i] = new IRSensor(this, 
                                            Vector(Bee::LENGTH/2-sin(a)*Bee::WIDTH/2, sin(a)*Bee::WIDTH/2),
                                            0, a, 10, 3731, 0, 0.7, 0);
            addLocalInteraction(range_sensors[i]);
			}
		this->lightSensors.resize (NUMBER_LIGHT_SENSORS);
		for (i = 0; i < NUMBER_LIGHT_SENSORS; i++) {
			this->lightSensors [i] = new LightSensor
				(std::numeric_limits<double>::max (), this,
				 LIGHT_SENSOR_POSITIONS [i], Component::OMNIDIRECTIONAL,
				 LIGHT_SENSOR_WAVELENGTH, LIGHT_SENSOR_NOISE);
			this->addLocalInteraction (this->lightSensors [i]);
		}
		this->vibrationSensors.resize (NUMBER_VIBRATION_SENSORS);
		for (i = 0; i < NUMBER_VIBRATION_SENSORS; i++) {
			this->vibrationSensors [i] = new VibrationIntensitySensor
				(std::numeric_limits<double>::max (),
				 this,
				 VIBRATION_SENSOR_POSITIONS [i],
				 Bee::VIBRATION_INTENSITY_SENSOR_MIN_MEASURABLE_INTENSITY,
				 Bee::VIBRATION_INTENSITY_SENSOR_MAX_MEASURABLE_INTENSITY,
				 Bee::VIBRATION_INTENSITY_SENSOR_NOISE
				 );
			// this->vibrationSensors [i] = new VibrationSensor
			// 	(
			// 	 std::numeric_limits<double>::max (),
			// 	 this,
			// 	 VIBRATION_SENSOR_POSITIONS [i], Component::OMNIDIRECTIONAL,
			// 	 VIBRATION_SENSOR_MAX_AMPLITUDE, VIBRATION_SENSOR_MAX_FREQUENCY, VIBRATION_SENSOR_NOISE_AMPLITUDE, VIBRATION_SENSOR_NOISE_FREQUENCY);
			this->addLocalInteraction (this->vibrationSensors [i]);
		}
	}

    /* virtual */
    Bee::~Bee()
    {
        BOOST_FOREACH(IRSensor* p, range_sensors)
        {
            delete p;
        }
        BOOST_FOREACH (LightSensor* p, lightSensors) {
			  delete p;
        }
        BOOST_FOREACH (VibrationIntensitySensor* p, vibrationSensors) {
			  delete p;
        }
    }

	void Bee::controlStep (double dt, const World *world)
	{
		controlStep_reaction (dt, world);
	}

	void Bee::controlStep_reaction (double dt, const World *)
	{
		double intensity, vibration;
		double minVibration, maxVibration;
		Vector lightGradient, vibrationGradient;
		senseLight (&intensity, &lightGradient, NULL, NULL);
		senseVibration (&vibration, &vibrationGradient, &minVibration, &maxVibration);
		moveUp (&vibrationGradient, -1);

			// if (intensity > 0 || frequency > 0) {
			// 	double weightLight = lightGradient.norm2 ();
			// 	double weightVibration = frequencyGradient.norm2 ();
			// 	double rnd = uniformRand () * (weightLight + weightVibration);
			// 	double curveness;
			// 	if (rnd < weightLight) {
			// 		moveUp (&lightGradient, 0);
			// 	}
			// 	else {
			// 		// if (frequency > 0.5 * Bee::VIBRATION_SENSOR_MAX_FREQUENCY) {
			// 		// 	curveness = 0;
			// 		// }
			// 		// else {
			// 		// 	curveness = 1 - 2 * frequency / Bee::VIBRATION_SENSOR_MAX_FREQUENCY;
			// 		// }
			// 		moveUp (&frequencyGradient, curveness);
			// 	}
			// }
			// else {
			// 	moveRandomly ();
			// }
		// }
	}

	void Bee::controlStep_behaviourBased (double dt, const World *world)
	{
		// double intensity, frequency;
		// Vector lightGradient, frequencyGradient;
		// senseLight (&intensity, &lightGradient);
		// senseFrequency (&frequency, &frequencyGradient);
		// bool endLoop = false;
		// for (std::vector<const BehaviourRule *>::iterator ibr =  this->behaviour.begin ();
		// 	  ibr != this->behaviour.end ();
		// 	  ibr++) {
		// 	const BehaviourRule *rule;
		// 	rule = (*ibr);
		// 	switch (rule->stimulus) {
		// 	case LIGHT:
		// 		if ((rule->usesMinimumValue && (intensity >= rule->minimumValue))
		// 			 || (rule->usesMaximumValue && (intensity <= rule->maximumValue))) {
		// 			switch (rule->action) {
		// 			case MOVE_UP:
		// 				moveUp (&lightGradient);
		// 				break;
		// 			case MOVE_DOWN:
		// 				moveDown (&lightGradient);
		// 				break;
		// 			}
		// 			endLoop = true;
		// 		}
		// 		break;
		// 	case VIBRATION:
		// 		if ((rule->usesMinimumValue && (frequency >= rule->minimumValue))
		// 			 || (rule->usesMaximumValue && (frequency <= rule->maximumValue))) {
		// 			switch (rule->action) {
		// 			case MOVE_UP:
		// 				moveUp (&frequencyGradient);
		// 				break;
		// 			case MOVE_DOWN:
		// 				moveDown (&frequencyGradient);
		// 				break;
		// 			}
		// 			endLoop = true;
		// 		}
		// 		break;
		// 	}
		// 	if (endLoop) {
		// 		break;
		// 	}
		// }
		// if (!endLoop) {
		// 	this->rightSpeed = Bee::MAX_SPEED;
		// 	this->leftSpeed = Bee::MAX_SPEED * 0.8;
		// }
 	}

	void Bee::senseLight (double *intensity, Vector *gradient, double *min, double *max) const
	{
		int i, indexMax, indexMin;
		indexMax = 0;
		indexMin = 0;
		*intensity = 0;
		for (i = 0; i < NUMBER_LIGHT_SENSORS; i++) {
			double lsi = this->lightSensors [i]->getIntensity ();
			if (lsi < this->lightSensors [indexMin]->getIntensity ()) {
				indexMin = i;
			}
			else if (lsi > this->lightSensors [indexMax]->getIntensity ()) {
				indexMax = i;
			}
			*intensity += lsi;
		}
		*intensity /= NUMBER_LIGHT_SENSORS;
		*gradient = LIGHT_SENSOR_POSITIONS [indexMax] - LIGHT_SENSOR_POSITIONS [indexMin];
	}

	void Bee::senseVibration (double *vibration, Vector *gradient, double *min, double *max)
	{
		int i, indexMax, indexMin;
		indexMax = 0;
		indexMin = 0;
		*vibration = 0;
		for (i = 0; i < NUMBER_VIBRATION_SENSORS; i++) {
			double lsi = this->vibrationSensors [i]->getIntensity ();
			if (lsi < this->vibrationSensors [indexMin]->getIntensity ()) {
				indexMin = i;
			}
			else if (lsi > this->vibrationSensors [indexMax]->getIntensity ()) {
				indexMax = i;
			}
			*vibration += lsi;
		}
		*vibration /= NUMBER_VIBRATION_SENSORS;
		*gradient = VIBRATION_SENSOR_POSITIONS [indexMax] - VIBRATION_SENSOR_POSITIONS [indexMin];
		*min = this->vibrationSensors [indexMin]->getIntensity ();
		*max = this->vibrationSensors [indexMax]->getIntensity ();
		this->vibrationGradientAlpha = normalizeAngle (gradient->angle ());
	}

	void Bee::moveUp (const Vector *gradient, double curveness)
	{
		double delta = normalizeAngle (gradient->angle ());

		if (fabs (delta) < 0.31) {
			this->rightSpeed = Bee::MAX_SPEED;
			this->leftSpeed = Bee::MAX_SPEED;
		}
		else if (delta > 0) {
			this->rightSpeed =  2*Bee::MAX_SPEED/3;
			this->leftSpeed  = -2*Bee::MAX_SPEED/3;
		}
		else {
			this->rightSpeed = -2*Bee::MAX_SPEED/3;
			this->leftSpeed  =  2*Bee::MAX_SPEED/3;
		}


		// works with
		// const double Bee::VIBRATION_INTENSITY_SENSOR_NOISE = 0.004;
		// if (fabs (delta) < 0.31) {
		// 	this->rightSpeed = Bee::MAX_SPEED;
		// 	this->leftSpeed = Bee::MAX_SPEED;
		// }
		// else if (delta > 0) {
		// 	this->rightSpeed =  3*Bee::MAX_SPEED/4;
		// 	this->leftSpeed  = -2*Bee::MAX_SPEED/3;
		// }
		// else {
		// 	this->rightSpeed = -2*Bee::MAX_SPEED/3;
		// 	this->leftSpeed  =  3*Bee::MAX_SPEED/4;
		// }


		// works with
		// const double Bee::VIBRATION_INTENSITY_SENSOR_NOISE = 0.002;
		// if (fabs (delta) < 0.31) {
		// 	this->rightSpeed = Bee::MAX_SPEED;
		// 	this->leftSpeed = Bee::MAX_SPEED;
		// }
		// else if (delta > 0) {
		// 	this->rightSpeed =  Bee::MAX_SPEED/4;
		// 	this->leftSpeed  = -Bee::MAX_SPEED/5;
		// }
		// else {
		// 	this->rightSpeed = -Bee::MAX_SPEED/5;
		// 	this->leftSpeed  =  Bee::MAX_SPEED/4;
		// }
	}

	void Bee::moveDown (const Vector *gradient)
	{
		double delta = normalizeAngle (gradient->angle ());
		if (delta > 0) {
			this->rightSpeed = Bee::MAX_SPEED * 0.1;
			this->leftSpeed = Bee::MAX_SPEED;
		}
		else {
			this->rightSpeed = Bee::MAX_SPEED;
			this->leftSpeed = Bee::MAX_SPEED * 0.1;
		}
	}

	void Bee::moveRandomly ()
	{
		this->rightSpeed = std::max (0.0, std::min (gaussianRand (this->rightSpeed, Bee::MAX_SPEED * 0.4), Bee::MAX_SPEED));
		this->leftSpeed  = std::max (0.0, std::min (gaussianRand (this->leftSpeed,  Bee::MAX_SPEED * 0.3), Bee::MAX_SPEED));
	}
}
