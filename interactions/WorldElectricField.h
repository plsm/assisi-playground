#ifndef __WORLD_ELECTRIC_FIELD_H
#define __WORLD_ELECTRI_CFIELD_H

#include "ExtendedWorld.h"
#include "PhysicSimulation.h"
#include "AbstractElectricFieldActuator.h"
#include "Field.h"

namespace Enki
{
	/**
	 * Contains the electric field produced by ALL robots in a simulation.
	 * This class allows easy access to this field.  Enki does not allow
	 * access to robot global interactions.
	 */
	class WorldElectricField :
		public PhysicSimulation
	{
	public:
		/**
		 * Length in world coordinates of a grid point.
		 */
		const double gridScale;
		/**
		 * Grid size.
		 */
		Enki::Vector size;
		/**
		 * Smallest world coordinates of cell (0,0) in the electric field matrix.
		 */
		Vector origin;
	private:
		/**
		 * Electric field matrix.
		 */
		std::vector<std::vector<Field> > electricField;
		/**
		 * Active electric field sources.
		 */
		std::vector<const AbstractElectricFieldActuator *>sources;
	public:
		WorldElectricField (double gridScale);
		virtual ~WorldElectricField () {}

		const Field &getElectricFieldAt (int x, int y) const;

		/**
		 * Initialise this physic interaction with the given world.
		 */
		virtual void initParameters (const ExtendedWorld *);
		/**
		 * Initialise the computation of the next state of this physic
		 * interaction.
		 */
		virtual void initStateComputing (double deltaTime);
		/**
		 * Computes the next state of this physic interaction.
		 */
		virtual void computeNextState (double deltaTime);

		void addSource (const AbstractElectricFieldActuator *source);

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
