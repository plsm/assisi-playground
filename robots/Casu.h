/*! \file  Casu.h
    \brief CASU "robot" declaration.

 */

#ifndef ENKI_CASU_H
#define ENKI_CASU_H

#include <vector>

#include <PhysicalEngine.h>

#include <interactions/IRSensor.h>
#include "interactions/DiagnosticLed.h"
#include "interactions/LightSourceFromAbove.h"
#include "interactions/HeatActuatorMesh.h"
#include "interactions/WaveVibrationSource.h"

namespace Enki
{

    class Casu : public ExtendedRobot
    {
    public:
       static /*const*/ double VIBRATION_RANGE;
       static const Vector VIBRATION_POSITION;
       static /*const*/ double VIBRATION_MAXIMUM_AMPLITUDE;
       static /*const*/ double VIBRATION_PHASE;
       static /*const*/ double VIBRATION_FREQUENCY;
       static const double VIBRATION_VELOCITY;
       static /*const*/ double VIBRATION_AMPLITUDE_QUADRATIC_DECAY;
       static /*const*/ double VIBRATION_NOISE;

    public:
        //! Create a CASU
        Casu(World* world);

        //! Destructor
        ~Casu();

        /* Sensors */

        typedef std::vector<IRSensor*> IRSensorVector;
        //! Proximity sensors.
        IRSensorVector range_sensors;                
        
        /* Actuators */

        //! Light source
        LightSourceFromAbove* light_source_blue;

        //! Diagnostic LED
        DiagnosticLed* top_led;

        //! Peltier heater
        HeatActuatorMesh *peltier;

        //! Vibration actuator
        WaveVibrationSource *vibration;

		/**
		 * Save the state of this robot to the given stream.
		 */
		virtual void saveState (std::ostream &os) const;
    private:
        World* world_;
        
    };
}

#endif
