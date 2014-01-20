/*! \file  Bee.h
    \brief A simplistic bee model implementation.
*/
#ifndef ENKI_BEE_H
#define ENKI_BEE_H

#include <enki/robots/DifferentialWheeled.h>
#include <enki/interactions/IRSensor.h>

#include "../interactions/LightSensor.h"
#include "../interactions/VibrationSensor.h"
	
namespace Enki
{
	
	/**
	 * A simplistic bee model.
	 *
	 * Bees react to temperature and vibration thresholds.  During each time
	 * step, they sense temperature and vibration and depending on the
	 * threshold they either move away or they 
	 *
	 * \ingroup robot */
	class Bee : public DifferentialWheeled
	{
		/**
		 * A bee is modelled in Enki by parallelepiped where the length is
		 * the distance from head to abdomen.
		 */
		static const double LENGTH;
		/**
		 * A bee is modelled in Enki by parallelepiped where the width is
		 * the distance between closed wings.
		 */
		static const double WIDTH;
		/**
		 * A bee is modelled in Enki by parallelepiped where the width is
		 * the distance between closed wings.
		 */
		static const double HEIGHT;
		static const double MAX_SPEED;
		/**
		 * Number of light sensors in a bee.
		 */
		static const int NUMBER_LIGHT_SENSORS;
		/**
		 * Light sensor positions relative to bee centre. 
		 */
		static const Point LIGHT_SENSOR_POSITIONS [];
		static const Point LIGHT_SENSOR_ORIENTATIONS [];
		static const double LIGHT_SENSOR_RANGE;
		static const double LIGHT_SENSOR_WAVELENGTH;
		/**
		 * Actions that a bee can do after sensing some temperature,
		 * vibration or light value.  The bee senses some gradient, and can
		 * either move down the gradient or move up the gradient.
		 */
		enum Action {MOVE_DOWN, MOVE_UP, NUMBER_ACTIONS};
		/**
		 * Stimulus that a bee reacts to
		 */
		enum Stimulus {LIGHT, VIBRATION};
		/**
		 * A simple reactive bee behaviour.  A bee may react to some value
		 * and change its behaviour.  Behaviour rules are characterised by a
		 * sensor range.  The range can have a lower and upper limit.  A rule
		 * range must have at least one limit otherwise it will not trigger
		 * an action.
		 */
		struct BehaviourRule
		{
			/**
			 * Minimum value the stimulus should have in order to perform this
			 * rule action.
			 */
			double minimumValue;
			/**
			 * Whether this rule range has a lower limit.
			 */
			bool usesMinimumValue;
			/**
			 * Maximum value the stimulus should have in order to perform this
			 * rule action.
			 */
			double maximumValue;
			/**
			 * Whether this rule range has a upper limit.
			 */
			bool usesMaximumValue;
			/**
			 * The stimulus assigned to this behaviour rule.
			 */
			Stimulus stimulus;
			/**
			 * The action to perform.
			 */
			Action action;
		};
		typedef std::vector<const BehaviourRule *> Behaviour;
		/**
		 * Bee behaviour sorted from highest priority to lowest priority.
		 */
		 Behaviour behaviour;

	public:
        //! Create a Bee
        Bee(void);
        
        //! destructor
        virtual ~Bee();

        typedef std::vector<IRSensor*> IRSensorVector;
        IRSensorVector range_sensors;

		typedef std::vector<LightSensor *> LightSensorVector;
		/**
		 * Bee light sensors.
		 */
		LightSensorVector lightSensors;

		/**
		 * Update the position of this bee.  
		 */
		void controlStep (double dt, const World *);

	private:
		void senseLight (double *intensity, Vector *gradient) const;
		void moveUp (const Vector *gradient);
		void moveDown (const Vector *gradient);
	};
}

#endif
