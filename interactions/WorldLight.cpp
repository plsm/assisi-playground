#include <limits>

#include "WorldLight.h"
#include "AbstractElectricFieldActuator.h"

using namespace Enki;
using namespace std;

WorldLight::
WorldLight (double gridScale, double waveLength):
	AbstractGridSimulation (gridScale, 0),
	waveLength (waveLength)
{
}

double WorldLight::
getIntensityAt (int x, int y) const
{
	return this->grid [x][y];
}

void WorldLight::
initStateComputing (double deltaTime)
{
	this->sources.resize (0);
}

void WorldLight::
computeNextState (double deltaTime)
{
	Vector pos;
	pos.x = origin.x;
	for (int x = 0; x < this->size.x; x++) {
		pos.y = origin.y;
		for (int y = 0; y < this->size.y; y++) {
			double sumIntensity = 0;
			for (size_t i = 0; i < this->sources.size (); i++) {
				sumIntensity += this->sources [i]->getIntensityAt (pos, this->waveLength);
			}
			this->grid [x][y] = sumIntensity;
			pos.y += this->gridScale;
		}
		pos.x += this->gridScale;
	}
}

void WorldLight::
addSource (const LightSource *source)
{
	this->sources.insert (this->sources.end (), source);
}
