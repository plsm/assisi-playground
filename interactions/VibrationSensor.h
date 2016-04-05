/* 
 * File:   VibrationSensor.h
 * Author: Pedro Mariano
 *
 * Created on 7 de Janeiro de 2014, 12:14
 */

#ifndef __VIBRATION_SENSOR__
#define __VIBRATION_SENSOR__

#include <iostream>
#include <vector>

#include <enki/Interaction.h>

#include "extensions/Component.h"

namespace Enki
{
	/**
	 * Represents a vibration sensor.
	 *
	 * @author Pedro Mariano
	 */
	class VibrationSensor
		: public LocalInteraction,
		  public Component
	{
	public:
		struct Measure
		{
			double amplitude;
			double frequency;
			double maxAbsAmplitude;
			int id;
			std::string source;
		};
	private:
		/**
		 * How much time has passed.  The enki simulator does not store how
		 * many time has passed.  The local interaction only receives delta
		 * time.
		 */
		double totalElapsedTime;
		// /**
		//  * Maximum measurable amplitude of this sensor.
		//  */
		// const double maxMeasurableAmplitude;
		/**
		 * Maximum measurable frequency of this sensor.
		 */
		const double maxMeasurableFrequency;
		/**
		 * Standard deviation of the Gaussian noise that is applied to each
		 * perceived vibration amplitude.
		 */
		const double amplitudeStandardDeviationGaussianNoise;
		/**
		 * Standard deviation of the Gaussian noise that is applied to each
		 * perceived vibration frequency.
		 */
		const double frequencyStandardDeviationGaussianNoise;
		// /**
		//  * Measured amplitude in the current simulation iteration.
		//  *
		//  * <p> This field is set to zero at the beginning of each time step.
		//  * For each object that is a vibration actuator we update this
		//  * attribute with the sensed vibration according to the vibration
		//  * model.
		//  */
		// std::vector<double> amplitudeValues;
		// /**
		//  * Measured frequency in the current simulation iteration.
		//  *
		//  * <p> This field is set to zero at the beginning of each time step.
		//  * For each object that is a vibration actuator we update this
		//  * attribute with the sensed vibration according to the vibration
		//  * model.
		//  */
		// std::vector<double> frequencyValues;
		/**
		 * Vibration measures for each vibration actuator in the world.
		 */
		std::vector<Measure> measureData;
	public:
		VibrationSensor (
			double range, Enki::Robot* owner,
			Enki::Vector relativePosition, double orientation,
			 double maxMeasurableFrequency, double amplitudeStandardDeviationGaussianNoise, double frequencyStandardDeviationGaussianNoise);
		/**
		 * Copy constructor.
		 */
		VibrationSensor (const VibrationSensor& orig);

		virtual ~VibrationSensor ();
		// /**
		//  * Return the amplitude of the vibration sensed by this sensor.
		//  */
		// const std::vector<double> &getAmplitude () const
		// {
		// 	return this->amplitudeValues;
		// }
		// /**
		//  * Return the frequency of the vibration sensed by this sensor.
		//  */
		// const std::vector<double> &getFrequency () const
		// {
		// 	return this->frequencyValues;
		// }
		const std::vector<Measure> &getMeasure () const
		{
			return this->measureData;
		}
		const Measure &getMeasure (int index) const
		{
			return this->measureData [index];
		}
		/**
		 * Initialise the measured amplitude and frequency in the current
		 * iteration step.
		 *
		 * @param dt time step.
		 *
		 * @param w world where the interaction takes place.
		 */
		virtual void init (double dt, Enki::World* w);
		/**
		 * Update the measured amplitude and frequency if the object is a
		 * vibration actuator.
		 *
		 * @param dt time step.
		 *
		 * @param w world where the interaction takes place.
		 */
		virtual void objectStep (double dt, Enki::World* w, Enki::PhysicalObject *po);


		virtual void finalize (double dt, Enki::World* w);
	};

}

#endif /* __VIBRATION_SENSOR__ */


// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
