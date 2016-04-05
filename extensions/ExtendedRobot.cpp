/* 
 * File:   ExtendedRobot.cpp
 * Author: pedro
 * 
 * Created on 17 de Fevereiro de 2014, 18:32
 */

#include <boost/interprocess/sync/interprocess_mutex.hpp>

#include "ExtendedRobot.h"

using namespace Enki;

/**
 * Robot id generator.
 */
static int idGenerator = 0;

/**
 * Mutex used to obtain a new robot identifier.
 */
static boost::interprocess::interprocess_mutex idMutex;

static int nextId ()
{
	int result;
	idMutex.lock ();
	result = idGenerator++;
	idMutex.unlock ();
	return result;
}

ExtendedRobot::
ExtendedRobot (const std::string &name):
	id (nextId ()),
	name (name)
{
}

ExtendedRobot::
ExtendedRobot (const ExtendedRobot& orig):
	id (nextId ()),
	name (orig.name)
{
}

ExtendedRobot::
~ExtendedRobot ()
{
}

void ExtendedRobot::
initPhysicInteractions (double dt, PhysicSimulation* ps)
{
	for (size_t i = 0; i < physicInteractions.size (); i++ ) {
		physicInteractions [i]->init (dt, ps);
	}
}

void ExtendedRobot::
doPhysicInteractions (double dt, PhysicSimulation *ps)
{
	for (size_t i = 0; i < this->physicInteractions.size (); i++) {
		physicInteractions [i]->step (dt, ps);
	}
}

void ExtendedRobot::
finalizePhysicInteractions (double dt, PhysicSimulation* ps)
{
	for (size_t i = 0; i < physicInteractions.size (); i++ ) {
		physicInteractions [i]->finalize (dt, ps);
	}
}
