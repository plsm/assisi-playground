/*! \file  Casu.h
    \brief CASU "robot" declaration.

 */

#ifndef ENKI_CASU_H
#define ENKI_CASU_H

#include <vector>

//#include <PhysicalEngine.h>

#include <interactions/IRSensor.h>
#include "interactions/DiagnosticLed.h"

#include <enki/PhysicalEngine.h>

#include "ExtendedWorld.h"
#include "ExtendedRobot.h"

#include "interactions/LightSourceFromAbove.h"
#include "interactions/QuadraticVibrationSource.h"
#include "interactions/HeatActuator.h"
#include "interactions/PointElectricFieldActuator.h"

namespace Enki
{
	// Forward declarations
	class Robot;
	class IRSensor;
    
	class Casu : public ExtendedRobot
	{
	public:
		static const double RADIUS;
		static const double LIGHT_SOURCE_RANGE;
		static const Point LIGHT_SOURCE_RELATIVE_POSITION;
		static const double LIGHT_SOURCE_K;
		static const double LIGHT_SOURCE_RADIUS;
		static const double LIGHT_SOURCE_PEAK_WAVE_LENGTH;
		// static const double LIGHT_SOURCE_MAX_INTENSITY;
		static const double LIGHT_SOURCE_SIGMA;

		static const double VIBRATION_SOURCE_RANGE;
		static const Point VIBRATION_SOURCE_RELATIVE_POSITION;
		static const double VIBRATION_SOURCE_A;
		/**
		 * Maximum heat emitted by this CASU.  This field is also used in the
		 * {@code HeatActuator} constructor.
		 */
		static const double MAX_HEAT;
	public:
		LightSourceFromAbove *lightSource;
		QuadraticVibrationSource *vibrationSource;
		/**
		 * CASU heat actuator that is located at its centre of mass (in 2D).
		 */
		HeatActuator *heatActuator;
		/**
		 * CASU electric field actuator that is located at its centre of mass
		 * (in 2D).
		 */
		PointElectricFieldActuator *electricFieldActuator;
		double electricCharge;
	public:
		//! Create a CASU
		Casu (World *world, Point *position);

		//! Destructor
		~Casu();

		//! Range
		typedef std::vector<IRSensor*> IRSensorVector;
		IRSensorVector range_sensors;
        
		DiagnosticLed* top_led;
	};
}

#endif
