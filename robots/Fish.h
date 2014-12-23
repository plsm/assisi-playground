/*! \file  Fish.h
    \brief A simplistic fish model implementation.
*/
#ifndef ENKI_FISH_H
#define ENKI_FISH_H

#include <enki/robots/DifferentialWheeled.h>
#include "extensions/ExtendedRobot.h"
#include "interactions/ObjectSensor.h"
#include "interactions/HeatSensor.h"
#include "interactions/LightSensor.h"
	
namespace Enki
{
	
	/**
	 * A simplistic fish model.
	 *
	 * \ingroup robot */
	class Fish :
		public DifferentialWheeled,
		public ExtendedRobot
	{
	public:
		static double SCALE_FACTOR;

	public:
		//! Create a Fish
		Fish (double scaleSize = Fish::SCALE_FACTOR);
        
		//! destructor
		virtual ~Fish ();

		/* Sensors */

		//! Fish color (useful for diagnostic purposes)
		double color_r_;
		double color_g_;
		double color_b_;

		/**
		 * Update the position of this fish.  
		 */
		void step (int dt, const World *);

		/**
		 * Update the fish color (useful for diagnostic purposes).
		 */
		void setColor (double r, double g, double b);
	};
}

#endif
