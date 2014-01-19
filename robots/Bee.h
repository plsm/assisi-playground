/*! \file  Bee.h
    \brief A simplistic bee model implementation.
*/
#ifndef ENKI_BEE_H
#define ENKI_BEE_H

#include <enki/robots/DifferentialWheeled.h>
#include <enki/interactions/IRSensor.h>

	
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
		 * Number of light sensors in a bee.
		 */
		static const int NUMBER_LIGHT_SENSORS;
		/**
		 * Light sensor positions relative to bee centre. 
		 */
		static const Point LIGHT_SENSOR_POSITIONS [];
		/**
		 * Actions that a bee can do after sensing some temperature,
		 * vibration or light value.
		 */
		enum Action {APPROACH, MOVE_AWAY, NUMBER_ACTIONS};
		/**
		 * Stimulus that a bee reacts to
		 */
		enum Stimulus {LIGHT, VIBRATION};
		/**
		 * A simple reactive bee behaviour.  A bee may react to some value and change its behaviour.
		 */
		struct Behaviour
		{
			double minimumValue;
			bool usesMinimumValue;
			double maximumValue;
			bool usesMaximumValue;
			Action action;
			Stimulus stimulus;
		};
		/**
		 * Bee behaviour sorted from highest priority to lowest priority.
		 */
		const Behaviour *behaviour;

	public:
        //! Create a Bee
        Bee(void);
        
        //! destructor
        virtual ~Bee();

        typedef std::vector<IRSensor*> IRSensorVector;
        IRSensorVector range_sensors;

		typedef std::vector<LightSensor> LightSensorVector;
		/**
		 * Bee light sensors.
		 */
		LightSensorVector lightSensors;

		/**
		 * Update the position of this bee.  
		 */
		void controlStep (int dt, const World *);

	private:
		void senseLight () const;
	};
}

#endif
