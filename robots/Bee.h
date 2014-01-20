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
	 * threshold they either climb the gradient or descend it.

	 * <p> There are two bee controllers.  See the documentation of methods
	 * {@code controlStep_reaction(double, const World *)} and {@code
	 * controlStep_behaviourBased(double, const World *)}.
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
		/**
		 * Maximum bee speed.
		 */
		static const double MAX_SPEED;
		/**
		 * Number of light sensors in a bee.
		 */
		static const int NUMBER_LIGHT_SENSORS;
		/**
		 * Light sensor positions relative to bee centre. 
		 */
		static const Point LIGHT_SENSOR_POSITIONS [];
		/**
		 * Bee peak light sensitivity.
		 */
		static const double LIGHT_SENSOR_WAVELENGTH;
		/**
		 * Bee's light sensors Gaussian noise.
		 */
		static const double LIGHT_SENSOR_NOISE;
		/**
		 * Number of vibration sensors in a bee.
		 */
		static const int NUMBER_VIBRATION_SENSORS;
		/**
		 * Vibration sensor positions relative to bee centre. 
		 */
		static const Point VIBRATION_SENSOR_POSITIONS [];
		/**
		 * Maximum perceived vibration amplitude.
		 */
		static const double VIBRATION_SENSOR_MAX_AMPLITUDE;
		/**
		 * Maximum perceived vibration frequency.
		 */
		static const double VIBRATION_SENSOR_MAX_FREQUENCY;
		/**
		 * Bee's vibration sensors Gaussian noise.
		 */
		static const double VIBRATION_SENSOR_NOISE_AMPLITUDE;
		/**
		 * Bee's vibration sensors Gaussian noise.
		 */
		static const double VIBRATION_SENSOR_NOISE_FREQUENCY;
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
		 * A simple reactive bee behaviour rule.  A bee may react to some
		 * value and change its behaviour.  Behaviour rules are characterised
		 * by a sensor range.  The range can have a lower and upper limit.  A
		 * rule range must have at least one limit otherwise it will not
		 * trigger an action.
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

		/**
		 * A bee only makes a control decision at specific time intervals.
		 * This allows us to use different simulation ticks.
		 */
		static const double HYSTERESIS_DECISION;
		/**
		 * Time left to perform the next control decision.  This is
		 * decremented by {@code dt} at each {@code controlStep(double,const
		 * World*)} method call.
		 */
		double clockDecision;

		typedef std::vector<LightSensor *> LightSensorVector;
		/**
		 * Bee light sensors.
		 */
		LightSensorVector lightSensors;

		typedef std::vector<VibrationSensor *> VibrationSensorVector;
		/**
		 * Bee vibration sensors.
		 */
		VibrationSensorVector vibrationSensors;
	public:
		//! Create a Bee
		Bee (Point *position);
        
        //! destructor
        virtual ~Bee();

        typedef std::vector<IRSensor*> IRSensorVector;
        IRSensorVector range_sensors;

		/**
		 * Update the position of this bee.

		 * <p> Main method of all robot objects in Enki.
		 */
		void controlStep (double dt, const World *);

	private:
		/**
		 * Update the position of this bee using a randomised reactive
		 * behaviour.  At each time step a bee senses light and vibration in
		 * order to obtain a value and a gradient.  It will then randomly
		 * follow the highest gradient.  The gradient is represented by a
		 * vector going form the point where it sensed the lowest value to
		 * the point where it sensed the highest value. The probability to
		 * follow a stimulus is proportional to the vector norm square.
		 */
		void controlStep_reaction (double dt, const World *);
		/**
		 * Update the position of this bee using a Behaviour Based bee model.
		 *
		 * <p> At each time step a bee senses light and vibration in order to
		 * obtain a value and a gradient.  Then it iterates vector {@code
		 * behaviour} to find the first behaviour rule whose condition is
		 * triggered.  Its is then executed.  If no rule is triggered, the
		 * bee wanders.
		 */
		void controlStep_behaviourBased (double dt, const World *);
		/**
		 * Compute the light perceived by the bee and its gradient.
		 *
		 * <p> The bee light sensors are located at the front.  They are used
		 * to compute a set of values.  The perceived intensity is the
		 * average.  The gradient is represented by a vector going form the
		 * point where it sensed the lowest value to the point where it
		 * sensed the highest value.
		 */
		void senseLight (double *intensity, Vector *gradient) const;
		/**
		 * Compute the vibration frequency perceived by the bee and its
		 * gradient.
		 *
		 * <p> The bee vibration sensors are located around its body.  They
		 * are used to compute a set of values.  The perceived frequency is
		 * the average.  The gradient is represented by a vector going form
		 * the point where it sensed the lowest value to the point where it
		 * sensed the highest value.
		 */
		void senseFrequency (double *frequency, Vector *gradient) const;
		/**
		 * Update motor speeds in order to climb the given gradient.
		 *
		 *  @param gradient Vector representing the gradient increasing
		 * direction.
		 */
		void moveUp (const Vector *gradient);
		/**
		 * Update motor speeds in order to descend the given gradient.
		 *
		 *  @param gradient Vector representing the gradient increasing
		 * direction.
		 */
		void moveDown (const Vector *gradient);

		void moveRandomly ();
	};
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
