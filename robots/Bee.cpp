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

	const double Bee::LIGHT_SENSOR_RANGE = 10;

	const double Bee::LIGHT_SENSOR_WAVELENGTH = 600;

	const Point Bee::LIGHT_SENSOR_POSITIONS[] = {
		Point (Bee::LENGTH, +Bee::WIDTH * 1.0),
		Point (Bee::LENGTH, +Bee::WIDTH * 0.8),
		Point (Bee::LENGTH, -Bee::WIDTH * 0.8),
		Point (Bee::LENGTH, -Bee::WIDTH * 1.0)
	};

    Bee::Bee(void) :
        DifferentialWheeled(0.4, 2, 0.0),
        range_sensors(5)
    {
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
				  (LIGHT_SENSOR_RANGE, this,
					LIGHT_SENSOR_POSITIONS [i], 0,
					LIGHT_SENSOR_WAVELENGTH);
		  }
    }

    /* virtual */
    Bee::~Bee()
    {
        BOOST_FOREACH(IRSensor* p, range_sensors)
        {
            delete p;
        }
    }

	void Bee::controlStep (double dt, const World *world)
	{
		double intensity;
		Vector gradient;
		senseLight (&intensity, &gradient);
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
						moveUp (&gradient);
						break;
					case MOVE_DOWN:
						moveDown (&gradient);
						break;
					}
					endLoop = true;
				}
				break;
			case VIBRATION:
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
		indexMin = 1;
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
