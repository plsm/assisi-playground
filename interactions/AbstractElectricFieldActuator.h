/* 
 * File:   AbstractElectricFieldActuator.h
 * Author: pedro
 *
 * Created on 3 de Março de 2014, 19:09
 */

#ifndef ABSTRACTELECTRICFIELDACTUATOR_H
#define	ABSTRACTELECTRICFIELDACTUATOR_H

#include <enki/PhysicalEngine.h>
#include "Component.h"
#include "PhysicInteraction.h"

namespace Enki {

	/**
	 * Abstract class of all electric field actuators.  Implementations must
	 * define a method that returns the electric field value and direction
	 * at a given point.  An electric field sensor computes its measured on
	 * the value returned by this method.

	 * <p> An electric field is a global interaction.
	 */
	class AbstractElectricFieldActuator:
		public Component,
		public PhysicInteraction
	{
	public:
		static double PERMITTIVITY;
		bool switchedOn;
	public:
		AbstractElectricFieldActuator (Robot *owner, Vector relativePosition, double relativeOrientation);
		AbstractElectricFieldActuator (const AbstractElectricFieldActuator& orig);
		virtual ~AbstractElectricFieldActuator ();
	public:
		/**
		 * Return the value and direction of the electric field actuator
		 * caused by this actuator at the given point.
		 */
		virtual void measureAt (const Point &, double *value, Point *direction) const = 0;
		/**
		 * Initialise the state of this actuator.
		 */
		virtual void init (double dt, PhysicSimulation *w) = 0;
		/**
		 * Update the state of the electric field model.
		 */
		virtual void step (double dt, PhysicSimulation* w);
		/**
		 * Toggles this electric field actuator. 
		 */
		void toogle ();
	};
}

#endif	/* ABSTRACTELECTRICFIELDACTUATOR_H */



// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
