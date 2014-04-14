#ifndef __WORLD_LIGHT_H
#define __WORLD_LIGHT_H

#include "ExtendedWorld.h"
#include "AbstractGridSimulation.h"
#include "LightSource.h"

namespace Enki
{
	/**
	 * Contains the electric field produced by ALL robots in a simulation.
	 * This class allows easy access to this field.  Enki does not allow
	 * access to robot global interactions.
	 */
	class WorldLight :
		public AbstractGridSimulation<double>
	{
		/**
		 */
		double waveLength;
	private:
		/**
		 * Active electric field sources.
		 */
		std::vector<const LightSource *>sources;
	public:
		WorldLight (double gridScale, double waveLength);
		virtual ~WorldLight () {}

		double getIntensityAt (int x, int y) const;

		/**
		 * Initialise the computation of the next state of this physic
		 * interaction.
		 */
		virtual void initStateComputing (double deltaTime);
		/**
		 * Computes the next state of this physic interaction.
		 */
		virtual void computeNextState (double deltaTime);

		void addSource (const LightSource *source);
	};
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
