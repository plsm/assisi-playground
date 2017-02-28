#ifndef __HEAT_ACTUATOR_H
#define __HEAT_ACTUATOR_H

#include <enki/PhysicalEngine.h>
#include <enki/Geometry.h>

#include "extensions/Component.h"
#include "extensions/PhysicInteraction.h"

#include "WorldHeat.h"

namespace Enki
{
	/**
	 * Represents a heat point source.  The source is relative to the robot
	 * that owns this actuator as specified by class {@code Component}.
	 */
	class HeatActuatorPointSource :
		public PhysicInteraction,
		public Component
	{
	protected:
		/**
		 * Heat emitted by this actuator.
		 */
		double heat;
		/**
		 * The maximum temperature change that this heat point source will cause.
		 */
		double slope;
		/**
		 * How much does this heat actuator takes to heat the environment.
		 */
		double thermalResponseTime;
		/**
		 * Is this actuator turned on.
		 */
		bool switchedOn;
		/**
		 * Whether we should recompute the heat distribution in the world.
		 */
		bool recomputeHeatDistribution;
		/**
		 * Return the real heat that this actuator is able to produce.
		 */
		double getRealHeat (double dt, WorldHeat *worldHeat) const
		{
			double factor = std::min (1.0, this->thermalResponseTime * dt);
			double heat_now = worldHeat->getHeatAt (this->absolutePosition);
			double heat_target = factor * this->heat
				+ (1 - factor) * heat_now;
			double heat_diff = fabs (heat_now - heat_target);
			if (heat_diff > dt * this->slope) {
				if (heat_now > heat_target)
					heat_target = heat_now - dt * this->slope;
				else {
					heat_target = heat_now + dt * this->slope;
				}
			}
			return heat_target;
				
		}
	public:
		HeatActuatorPointSource (
			Enki::Robot* owner,
			Enki::Vector relativePosition,
			double thermalResponseTime,
			double ambientTemperature);
		void setHeat (double value);
		double getHeat(void) { return this->heat; }
		void setSlope (double value) { this->slope = slope; }
		double getSlope (void) { return this->slope; }

		bool isSwitchedOn () const
		{
			return this->switchedOn;
		}
		void setSwitchedOn (bool value);
		void toogleSwitchedOn ();
		//! Init at each step
		virtual void init (double dt, PhysicSimulation *w);
		virtual void step (double dt, PhysicSimulation* w);
	};
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
