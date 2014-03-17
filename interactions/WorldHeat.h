#ifndef __WORLD_HEAT_H
#define __WORLD_HEAT_H

#include <vector>
#include <iostream>

#include "../ExtendedWorld.h"
#include "../PhysicSimulation.h"

namespace Enki
{
	/**
	 * Provides a simulation of heat to be used in Enki.

	 * <p> World heat is represented by a lattice grid.  The size of this
	 * grid depends on world dimensions, scale and border size.  The heat in
	 * a grid point depends on its current value and the value of the four
	 * Moore neighbours.
	 */
	class WorldHeat :
		public PhysicSimulation
	{
	public:
		/**
		 * Normal environmental heat used to compute heat at world borders.
		 */
		const double normalHeat;
		/**
		 * Length in world coordinates of a grid point.
		 */
		const double gridScale;
	private:
		/**
		 * Distance between world limits and environment.
		 */
		const double borderSize;
		/**
		 * Cellular automaton size.
		 */
		Enki::Vector size;
		/**
		 * Smallest world coordinates of cell (0,0) in the cellular automaton.
		 */
		Enki::Vector origin;
		/**
		 * Heat cellular automata.  One contains the heat in the current
		 * iteration while the other is used to computed the next state.
		 */
		std::vector<std::vector<double> > heat [2];
		/**
		 * Index to the current cellular automaton in field {@code heat}.
		 */
		int adtIndex;
	public:
		static const double THERMAL_DIFFUSIVITY_AIR;

		WorldHeat (double normalHeat, double gridScale, double borderSize);
		virtual ~WorldHeat () {}

		double getHeatAt (Vector &pos) const;
		void setHeatAt (Vector &pos, double value);

		/**
		 * Initialise this physic interaction with the given world.
		 */
		virtual void initParameters (const ExtendedWorld *);
		/**
		 * Initialise the computation of the next state of this physic
		 * interaction.
		 */
		virtual void initStateComputing (double deltaTime);
		// /**
		//  * Handles the action of the given physical object in this physic
		//  * interaction.
		//  */
		// virtual void handleObjectAction (const PhysicalObject *po);
		/**
		 * Computes the next state of this physic interaction.
		 */
		virtual void computeNextState (double deltaTime);
		// /**
		//  * Updates the physical sensors of the given object.
		//  */
		// virtual void handleObjectSense (PhysicalObject *po);

		void dumpState (std::ostream &os);

	private:
		void toIndex (const Enki::Vector& position, int &x, int &y) const;
	};
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
