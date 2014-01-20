/*

 */

#include <cmath>

#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include <enki/PhysicalEngine.h>

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
	const double Bee::LENGTH = 2;

	const double Bee::WIDTH = 0.8;

	const double Bee::HEIGHT = 0.4;

	const double Bee::MAX_SPEED = 1;

	const int Bee::NUMBER_LIGHT_SENSORS = 4;

	const double Bee::LIGHT_SENSOR_WAVELENGTH = 600;

	const Point Bee::LIGHT_SENSOR_POSITIONS[] = {
		Point (Bee::LENGTH * 1.0, +Bee::WIDTH * 1.0),
		Point (Bee::LENGTH * 0.5, +Bee::WIDTH * 0.7),
		Point (Bee::LENGTH * 0.5, -Bee::WIDTH * 0.7),
		Point (Bee::LENGTH * 1.0, -Bee::WIDTH * 1.0)
	};

	const double Bee::LIGHT_SENSOR_NOISE = 1.0;

	const int Bee::NUMBER_VIBRATION_SENSORS = 6;

	const double Bee::VIBRATION_SENSOR_MAX_AMPLITUDE = 100;

	const double Bee::VIBRATION_SENSOR_MAX_FREQUENCY = 2;

	const Point Bee::VIBRATION_SENSOR_POSITIONS[] = {
		Point (+Bee::LENGTH * 1.0, +Bee::WIDTH * 1.0),
		Point (+Bee::LENGTH * 0.0, +Bee::WIDTH * 1.0),
		Point (-Bee::LENGTH * 1.0, +Bee::WIDTH * 1.0),
		Point (+Bee::LENGTH * 1.0, -Bee::WIDTH * 1.0),
		Point (+Bee::LENGTH * 0.0, -Bee::WIDTH * 1.0),
		Point (-Bee::LENGTH * 1.0, -Bee::WIDTH * 1.0)
	};

	const double Bee::VIBRATION_SENSOR_NOISE_AMPLITUDE = 10.0;

	const double Bee::VIBRATION_SENSOR_NOISE_FREQUENCY = 1.0;

	Bee::Bee (Point *position) :
		DifferentialWheeled(0.4, 2, 0.0),
		range_sensors(5)
	{
		this->pos = *position;
        // Set shape & color

        double m = 1;   // Body mass

        //setRectangular(len, w, h, 1);
        Polygone footprint;
        footprint.push_back (Point (Bee::LENGTH/2,Bee::WIDTH/4));
        footprint.push_back (Point (Bee::LENGTH/2-Bee::WIDTH/(4*sqrt(2)),Bee::WIDTH/2));
        footprint.push_back (Point (-Bee::LENGTH/2+Bee::WIDTH/(4*sqrt(2)),Bee::WIDTH/2));
        footprint.push_back (Point (-Bee::LENGTH/2,Bee::WIDTH/4));
        footprint.push_back (Point (-Bee::LENGTH/2,-Bee::WIDTH/4));
        footprint.push_back (Point (-Bee::LENGTH/2+Bee::WIDTH/(2*sqrt(2)),-Bee::WIDTH/2));
        footprint.push_back (Point (Bee::LENGTH/2-Bee::WIDTH/(2*sqrt(2)),-Bee::WIDTH/2));
        footprint.push_back (Point (Bee::LENGTH/2,-Bee::WIDTH/4));
        PhysicalObject::Hull hull(PhysicalObject::Part(footprint, Bee::HEIGHT));
        setCustomHull(hull, m);
        setColor(Color(0.93,0.79,0,1));

        // Set other physical properties
        PhysicalObject::dryFrictionCoefficient = 2.5;

        int i = 0;
        for (double a = -pi/2; i < 5; i++, a += pi/4) 
        {
            range_sensors[i] = new IRSensor(this, 
                                            Vector(Bee::LENGTH/2-sin(a)*Bee::WIDTH/2, sin(a)*Bee::WIDTH/2),
                                            0, a, 10, 3731, 0, 0.7, 0);
            addLocalInteraction(range_sensors[i]);
        }
		  for (i = 0; i < NUMBER_LIGHT_SENSORS; i++) {
			  this->lightSensors [i] = new LightSensor
				  (std::numeric_limits<double>::max (), this,
					LIGHT_SENSOR_POSITIONS [i], Component::OMNIDIRECTIONAL,
					LIGHT_SENSOR_WAVELENGTH, LIGHT_SENSOR_NOISE);
		  }
		  for (i = 0; i < NUMBER_VIBRATION_SENSORS; i++) {
			  this->vibrationSensors [i] = new VibrationSensor
				  (std::numeric_limits<double>::max (), this,
					VIBRATION_SENSOR_POSITIONS [i], Component::OMNIDIRECTIONAL,
					VIBRATION_SENSOR_MAX_AMPLITUDE, VIBRATION_SENSOR_MAX_FREQUENCY, VIBRATION_SENSOR_NOISE_AMPLITUDE, VIBRATION_SENSOR_NOISE_FREQUENCY);
		  }
    }

    /* virtual */
    Bee::~Bee()
    {
        BOOST_FOREACH(IRSensor* p, range_sensors)
        {
            delete p;
        }
        BOOST_FOREACH(LightSensor* p, lightSensors)
        {
            delete p;
        }
        BOOST_FOREACH(VibrationSensor* p, vibrationSensors)
        {
            delete p;
        }
    }

	void Bee::controlStep (double dt, const World *world)
	{
		controlStep_reaction (dt, world);
	}
	void Bee::controlStep_reaction (double dt, const World *)
	{
		double intensity, amplitude;
		Vector lightGradient, vibrationGradient;
		senseLight (&intensity, &lightGradient);
		senseVibration (&amplitude, &vibrationGradient);
		double weightLight = lightGradient.norm2 ();
		double weightVibration = vibrationGradient.norm2 ();
		double rnd = uniformRand () * (weightLight + weightVibration);
		if (rnd < weightLight) {
			moveUp (&lightGradient);
		}
		else {
			moveUp (&vibrationGradient);
		}
	}
	void Bee::controlStep_behaviourBased (double dt, const World *world)
	{
		double intensity, amplitude;
		Vector lightGradient, vibrationGradient;
		senseLight (&intensity, &lightGradient);
		senseVibration (&amplitude, &vibrationGradient);
		bool endLoop = false;
		for (std::vector<const BehaviourRule *>::iterator ibr =  this->behaviour.begin ();
			  ibr != this->behaviour.end ();
			  ibr++) {
			const BehaviourRule *rule;
			rule = (*ibr);
			switch (rule->stimulus) {
			case LIGHT:
				if ((rule->usesMinimumValue && (intensity >= rule->minimumValue))
					 || (rule->usesMaximumValue && (intensity <= rule->maximumValue))) {
					switch (rule->action) {
					case MOVE_UP:
						moveUp (&lightGradient);
						break;
					case MOVE_DOWN:
						moveDown (&lightGradient);
						break;
					}
					endLoop = true;
				}
				break;
			case VIBRATION:
				if ((rule->usesMinimumValue && (amplitude >= rule->minimumValue))
					 || (rule->usesMaximumValue && (amplitude <= rule->maximumValue))) {
					switch (rule->action) {
					case MOVE_UP:
						moveUp (&vibrationGradient);
						break;
					case MOVE_DOWN:
						moveDown (&vibrationGradient);
						break;
					}
					endLoop = true;
				}
				break;
			}
			if (endLoop) {
				break;
			}
		}
		if (!endLoop) {
			this->rightSpeed = Bee::MAX_SPEED;
			this->leftSpeed = Bee::MAX_SPEED * 0.8;
		}
 	}

	void Bee::senseLight (double *intensity, Vector *gradient) const
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

	void Bee::senseVibration (double *amplitude, Vector *gradient) const
	{
		int i, indexMax, indexMin;
		indexMax = 0;
		indexMin = 0;
		*amplitude = 0;
		for (i = 0; i < NUMBER_VIBRATION_SENSORS; i++) {
			double lsi = this->vibrationSensors [i]->getAmplitude ();
			if (lsi < this->vibrationSensors [indexMin]->getAmplitude ()) {
				indexMin = i;
			}
			else if (lsi > this->vibrationSensors [indexMax]->getAmplitude ()) {
				indexMax = i;
			}
			*amplitude += lsi;
		}
		*amplitude /= NUMBER_VIBRATION_SENSORS;
		*gradient = VIBRATION_SENSOR_POSITIONS [indexMax] - VIBRATION_SENSOR_POSITIONS [indexMin];
	}


	void Bee::moveUp (const Vector *gradient)
	{
		double delta = normalizeAngle (gradient->angle () - this->angle);
		if (delta > 0) {
			this->rightSpeed = Bee::MAX_SPEED;
			this->leftSpeed = Bee::MAX_SPEED * 0.8;
		}
		else {
			this->rightSpeed = Bee::MAX_SPEED * 0.8;
			this->leftSpeed = Bee::MAX_SPEED;
		}
	}

	void Bee::moveDown (const Vector *gradient)
	{
		double delta = normalizeAngle (gradient->angle () - this->angle);
		if (delta > 0) {
			this->rightSpeed = Bee::MAX_SPEED * 0.8;
			this->leftSpeed = Bee::MAX_SPEED;
		}
		else {
			this->rightSpeed = Bee::MAX_SPEED;
			this->leftSpeed = Bee::MAX_SPEED * 0.8;
		}
	}
}
