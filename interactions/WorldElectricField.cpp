#include <limits>

#include "WorldElectricField.h"
#include "AbstractElectricFieldActuator.h"

using namespace Enki;
using namespace std;

WorldElectricField::
WorldElectricField (double gridScale):
	gridScale (gridScale)
{
}

const Field &WorldElectricField::getElectricFieldAt (int x, int y) const
{
	return this->electricField [x][y];
}

void WorldElectricField::
initParameters (const ExtendedWorld *world)
{
	switch (world->wallsType) {
	case World::WALLS_SQUARE: {
		Enki::Vector min (0, 0);
		Enki::Vector max (world->w, world->h);

		this->size = (max - min) / gridScale;
		this->size.x = round (this->size.x);
		this->size.y = round (this->size.y);
		this->electricField.resize (this->size.x);
		for (int x = 0; x < this->size.x; x++) {
			this->electricField [x].resize (this->size.y);
		}
		this->origin = min;
		std::cout << "* ** ELECTRIC FIELD DATA ** *\nmin: " << min << "\nmax: " << max << "\nsize: " << size << std::endl;
		break;
	}
	case World::WALLS_CIRCULAR: {
		Enki::Vector min (-world->r, -world->r);
		Enki::Vector max (world->r, world->r);

		this->size = (max - min) / gridScale;
		this->size.x = round (this->size.x);
		this->size.y = round (this->size.y);
		this->electricField.resize (this->size.x);
		for (int x = 0; x < this->size.x; x++) {
			this->electricField [x].resize (this->size.y);
		}
		this->origin = min;
		std::cout << "* ** ELECTRIC FIELD DATA ** *\nmin: " << min << "\nmax: " << max << "\nsize: " << size << std::endl;
		break;
	}
	case World::WALLS_NONE: {
		break;
	}
	default:
		throw 0;
	}
}


void WorldElectricField::
initStateComputing (double deltaTime)
{
	this->sources.resize (0);
}

void WorldElectricField::
computeNextState (double deltaTime)
{
	Vector pos;
	pos.x = origin.x;
	for (int x = 0; x < this->size.x; x++) {
		pos.y = origin.y;
		for (int y = 0; y < this->size.y; y++) {
			Field sumField;
			for (size_t i = 0; i < this->sources.size (); i++) {
				Field aField;
				this->sources [i]->measureAt (pos, &aField.value, &aField.direction);
				sumField += aField;
			}
			this->electricField [x][y] = sumField;
			pos.y += this->gridScale;
		}
		pos.x += this->gridScale;
	}
}

void WorldElectricField::
toIndex (const Enki::Vector& position, int &x, int &y) const
{
	x = round ((position.x - this->origin.x) / this->gridScale);
	y = round ((position.y - this->origin.y) / this->gridScale);
}

void WorldElectricField::
addSource (const AbstractElectricFieldActuator *source)
{
	this->sources.insert (this->sources.end (), source);
}
