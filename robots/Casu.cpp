/*

 */

#include <cmath>

#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include <PhysicalEngine.h>

#include "Casu.h"

const double pi = boost::math::constants::pi<double>();

namespace Enki
{

	double IR_SENSOR_HEIGHT = 0;  // cm  0.7
	double IR_SENSOR_RANGE = 2;

	const double Casu::RADIUS = 0.01;
	const double Casu::LIGHT_SOURCE_RANGE = 100;
	const Point Casu::LIGHT_SOURCE_RELATIVE_POSITION (0, 0);
	const double Casu::LIGHT_SOURCE_K = 1.0;
	const double Casu::LIGHT_SOURCE_RADIUS = 0.09;
	const double Casu::LIGHT_SOURCE_PEAK_WAVE_LENGTH = 450;
	// const double Casu::LIGHT_SOURCE_MAX_INTENSITY = 100.0;
	const double Casu::LIGHT_SOURCE_SIGMA = 1.0;

	const double Casu::VIBRATION_SOURCE_RANGE = 10;
	const Point Casu::VIBRATION_SOURCE_RELATIVE_POSITION (0, 0);
	const double Casu::VIBRATION_SOURCE_A = 1.0;

	const double Casu::MAX_HEAT = 40;

	Casu::Casu (World *world, Point *position) :
		range_sensors(6)
	{
		this->pos = *position;

		this->lightSource = new LightSourceFromAbove
			(LIGHT_SOURCE_RANGE, this,
			 LIGHT_SOURCE_RELATIVE_POSITION, Component::OMNIDIRECTIONAL,
			 LIGHT_SOURCE_K, LIGHT_SOURCE_RADIUS, LIGHT_SOURCE_PEAK_WAVE_LENGTH, LIGHT_SOURCE_SIGMA);
		this->lightSource->setCylindric (0, 0, -1); // turn the actuators to point objects so that they don't move
		addLocalInteraction (this->lightSource);
		world->addObject (this->lightSource);
		this->addPhysicInteraction (this->lightSource);

		this->vibrationSource = new QuadraticVibrationSource
			(VIBRATION_SOURCE_RANGE, this,
			 VIBRATION_SOURCE_RELATIVE_POSITION, VIBRATION_SOURCE_A);
		this->vibrationSource->setCylindric (0, 0, -1); // turn the actuators to point objects so that they don't move
		addLocalInteraction (this->vibrationSource);
		world->addObject (this->vibrationSource);

		this->heatActuator = new HeatActuator (this, Vector (0, 0), MAX_HEAT);
		this->heatActuator->switchedOn = true;
		this->addPhysicInteraction (this->heatActuator);

		this->electricFieldActuator = new PointElectricFieldActuator (this, Vector (), 4000);
		this->electricFieldActuator = new PointElectricFieldActuator (this, Vector (), this->electricCharge);
		this->electricFieldActuator->switchedOn = true;
		this->addPhysicInteraction (this->electricFieldActuator);

        // Set physical properties
        double radius = 1;
        double height = 2;
        Polygone hex;
        for (double a = pi/6; a < 2*pi; a += pi/3)
        {
            hex.push_back(Point(radius * cos(a), radius * sin(a)));
        }
        PhysicalObject::Hull hull(PhysicalObject::Part(hex, height));
        setCustomHull(hull, 1000);

		  // this->setCylindric (0, 0, -1); // turn the actuators to point objects so that they don't move

        setColor(Color(0.8,0.8,0.8,0.3));
        PhysicalObject::dryFrictionCoefficient = 1000; // Casus are immovable

        // Add range sensors
        range_sensors[0] = new IRSensor(this, Vector(0.866,0), IR_SENSOR_HEIGHT, 0, 
                                        2, 3731, 0.3, 0.7, 0);
        range_sensors[1] = new IRSensor(this, Vector(0.43,0.75), IR_SENSOR_HEIGHT, pi/3, 
                                        2, 3731, 0.3, 0.7, 0);
        range_sensors[2] = new IRSensor(this, Vector(-0.43,0.75), IR_SENSOR_HEIGHT, 2*pi/3, 
                                        2, 3731, 0.3, 0.7, 0);
        range_sensors[3] = new IRSensor(this, Vector(-0.866,0), IR_SENSOR_HEIGHT, pi, 
                                        2, 3731, 0.3, 0.7, 0);
        range_sensors[4] = new IRSensor(this, Vector(-0.43,-0.75), IR_SENSOR_HEIGHT, -2*pi/3, 
                                        2, 3731, 0.3, 0.7, 0);
        range_sensors[5] = new IRSensor(this, Vector(0.43,-0.75), IR_SENSOR_HEIGHT, -pi/3, 
                                        2, 3731, 0.3, 0.7, 0);
                
        BOOST_FOREACH(IRSensor* p, range_sensors)
        {
            addLocalInteraction(p);
        }

        // Add diagnostic led
        top_led = new DiagnosticLed(this);
        
    }

    Casu::~Casu()
    {
        BOOST_FOREACH(IRSensor* p, range_sensors)
        {
            delete p;
        }

        delete top_led;
    }

}
