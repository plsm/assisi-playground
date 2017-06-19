/*
 * File:  HeatActuatorSlope.h
 * Author: Pedro Mariano
 *
 * Created on March 22 2017
 */
#ifndef __HEAT_ACTUATOR_SLOPE_H__
#define __HEAT_ACTUATOR_SLOPE_H__

#include <iostream>

#include <enki/PhysicalEngine.h>
#include <enki/Geometry.h>

#include "extensions/Component.h"
#include "extensions/PhysicInteraction.h"
#include "extensions/PointMesh.h"

#include "WorldHeat.h"

namespace Enki
{
    /**
     * A heat actuator that changes the temperature of heat cells in
     * constant steps.  This heat actuator owns a set of heat cells of the heat
     * model.  Every time step, this actuator will modify the temperature of
     * these cells towards its target temperature.
     */
    class HeatActuatorSlope:
        public PhysicInteraction,
        public Component
    {
    protected:
        /**
         * The coordinates where this actuator will inject heat.  These
         * coordinates are relative to the position of this heat actuator.
         *
         * Note that if two coordinates correspond the same heat cells in the heat
         * model, then this heat actuator will inject heat twice.
         */
        const PointMesh *mesh;
        /**
         * The target temperature that this actuator is going to reach.  Every
         * time step, this actuator will change the cells of the heat model,
         * that is responsible for, towards this value.
         */
        double targetTemperature;
        /**
         * The maximum temperature change that this heat actuator will
         * cause.  The units are degrees per second.
         */
        double slope;
        /**
         * Indicates if this actuator is turned on.
         */
        bool switchedOn;
    public:
        /**
         * Construct a {@code HeatActuatorSlope}.  The heat actuator is
         * initially turned off.
         *
         * @param owner The robot that owns this heat actuator.
         *
         * @param relativePosition The position of this heat actuator relative
         * to the robot position
         *
         * @param targetTemperature The target temperature that this actuator is going to reach.

         * @param slope The amount of heat change per second that this actuator
         * is going to cause.
         */
        HeatActuatorSlope (
            Enki::Robot* owner,
            Enki::Vector relativePosition,
            double targetTemperature,
            double slope,
				PointMesh *mesh);
        virtual ~HeatActuatorSlope ();
        // set and get methods
        void setHeat (double value) {this->targetTemperature = value; }
        double getHeat(void) { return this->targetTemperature; }
        void setSlope (double value) { this->slope = slope; }
        double getSlope (void) { return this->slope; }
        bool isSwitchedOn () const {return this->switchedOn;  }
        void setSwitchedOn (bool value) {this->switchedOn = value; }
        //! Init at each step
        virtual void init (double dt, PhysicSimulation *w);
        virtual void step (double dt, PhysicSimulation* w);
        // debugging methods
        void print (std::ostream &os) const;
        friend std::ostream &operator<< (std::ostream &os, const HeatActuatorSlope &heatActuatorSlope);
    };
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
