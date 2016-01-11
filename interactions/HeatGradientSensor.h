/*
 * File: HeatGradientSensor.h
 * Author: Pedro Mariano
 */

#ifndef __HEAT_GRADIENT_SENSOR_H
#define __HEAT_GRADIENT_SENSOR_H

#include "interactions/HeatSensor.h"

#include "extensions/Component.h"
#include "extensions/PhysicInteraction.h"

namespace Enki
{
	class HeatGradientSensor:
        public HeatSensor
    {
        /**
         * How far (in number of cells) this sensor goes in order to search
         * for a the cell with highest value.
         */
        const int radius;

        Enki::Vector measuredGradient;
    public:
        HeatGradientSensor (
			Enki::Robot* owner, Enki::Vector relativePosition,
			double minMeasurableHeat, double maxMeasurableHeat,
			double thermalResponseTime,
			double ambientTemperature,
            int radius);
		HeatGradientSensor (
			Enki::Robot* owner, Enki::Vector relativePosition,
			double minMeasurableHeat, double maxMeasurableHeat,
            int radius);

        Enki::Vector getMeasuredGradient () const
        {
            return this->measuredGradient;
        }
		/**
		 * Update the measured heat if the object is a heat actuator.
		 *
		 * @param dt time step.
		 *
		 * @param w world where the interaction takes place.
		 */
		virtual void init (double dt, PhysicSimulation* w);
		virtual void step (double dt, PhysicSimulation* w);
    };
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// indent-tabs-mode: nil
// tab-width: 4
// c-basic-offset: 4
// End: 

