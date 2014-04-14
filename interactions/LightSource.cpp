/* 
 * File:   LightSource.cpp
 * Author: Pedro Mariano
 *
 * Created on 7 de Janeiro de 2014, 12:14
 */

#include "LightSource.h"
#include "WorldLight.h"

using namespace Enki;

LightSource::
LightSource (double range, Robot* owner, Vector relativePosition, double orientation):
	LocalInteraction (range, owner),
	Component (owner, relativePosition, orientation)
{
	Component::init ();
	this->pos = Component::absolutePosition;
}

LightSource::
LightSource (const LightSource &orig):
	LocalInteraction (orig.LocalInteraction::r, orig.LocalInteraction::owner),
	Component (orig)
{
}

void LightSource::
init (double dt, PhysicSimulation* ps)
{
	Component::init ();
}

void LightSource::
step (double dt, PhysicSimulation *ps)
{
	WorldLight *worldLight = dynamic_cast<WorldLight *> (ps);
	if (worldLight != NULL) {
		worldLight->addSource (this);
	}
}
