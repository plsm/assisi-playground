/* 
 * File:   AbstractElectricFieldActuator.cpp
 * Author: pedro
 * 
 * Created on 3 de Março de 2014, 19:09
 */

#include "AbstractElectricFieldActuator.h"
#include "WorldElectricField.h"

using namespace Enki;

double AbstractElectricFieldActuator::PERMITTIVITY = 8.8541878176E-12;

AbstractElectricFieldActuator::
AbstractElectricFieldActuator (
	Robot *owner,
	Vector relativePosition,
	double relativeOrientation
):
	Component (owner, relativePosition, relativeOrientation),
	PhysicInteraction (owner),
	switchedOn (false)
{
}

AbstractElectricFieldActuator::
AbstractElectricFieldActuator (
	const AbstractElectricFieldActuator& orig
):
	Component (orig),
	PhysicInteraction (orig),
	switchedOn (orig.switchedOn)
{
}

AbstractElectricFieldActuator::
~AbstractElectricFieldActuator ()
{
}

void AbstractElectricFieldActuator::
toogle ()
{
	this->switchedOn = !this->switchedOn;
}

void AbstractElectricFieldActuator::
step (double dt, PhysicSimulation *ps)
{
	if (this->switchedOn) {
		WorldElectricField *worldElectricField = dynamic_cast<WorldElectricField *> (ps);
		if (worldElectricField != NULL) {
			if (this->switchedOn) {
				worldElectricField->addSource (this);
			}
		}
	}
}
