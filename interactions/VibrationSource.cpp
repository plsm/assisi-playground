#include "VibrationSource.h"

using namespace Enki;

VibrationSource::VibrationSource (double range, Robot* owner, Vector relativePosition, double orientation):
	LocalInteraction (range, owner),
	Component (owner, relativePosition, orientation)
{
	Component::init ();
	this->pos = Component::absolutePosition;
}


void VibrationSource::
init (double dt, Enki::World* w)
{
	Component::init ();
}

