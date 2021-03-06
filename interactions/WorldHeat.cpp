#include <limits>
#include <stdio.h>

#include "WorldHeat.h"

using namespace Enki;
using namespace std;

const double WorldHeat::THERMAL_DIFFUSIVITY_AIR = 1.9e-5;
const double WorldHeat::THERMAL_DIFFUSIVITY_COPPER = 1.11e-4;
/*const*/ double WorldHeat::CELL_DISSIPATION = 1e-6;

WorldHeat::
WorldHeat (const ExtendedWorld *world, double normalHeat, double gridScale, double borderSize, double concurrencyLevel, int logRate):
	AbstractGrid (world, gridScale, borderSize),
#ifdef WORLDHEAT_SERIAL
	AbstractGridSimulation (),
#else
	AbstractGridParallelSimulation (concurrencyLevel, this, false),
#endif
	AbstractGridProperties (),
	initFlag (true),
	normalHeat (normalHeat),
	logStream (NULL),
	logRate (logRate - 1),
	iterationsToNextLog (logRate),
	relativeTime (0),
	partialAlpha (
		100 * 100 // gridScale is in centimetres
		/ (gridScale * gridScale))
{
}

WorldHeat::
WorldHeat (const Vector &size, const Vector &origin, double normalHeat, double gridScale, double borderSize, double concurrencyLevel, int logRate):
	AbstractGrid (gridScale, borderSize, size, origin),
#ifdef WORLDHEAT_SERIAL
	AbstractGridSimulation (),
#else
	AbstractGridParallelSimulation (concurrencyLevel, this, false),
#endif
	AbstractGridProperties (),
	initFlag (false),
	normalHeat (normalHeat),
	logStream (NULL),
	logRate (logRate - 1),
	iterationsToNextLog (logRate),
	relativeTime (0),
	partialAlpha (
		100 * 100 // gridScale is in centimetres
		/ (gridScale * gridScale))
{
}

WorldHeat *WorldHeat::
worldHeatFromFile (string filename, double concurrencyLevel, int logRate)
{
	ifstream ifs (filename.c_str ());
	Vector size;
	Vector origin;
	double gridScale;
	double borderSize;
	double normalHeat;
	string dummy;
	ifs >> dummy >> borderSize;
	ifs >> dummy >> gridScale;
	ifs >> dummy >> size.x >> size.y;
	ifs >> dummy >> origin.x >> origin.y;
	ifs >> dummy >> normalHeat;
	WorldHeat *result = new WorldHeat
		(size, origin,
		 normalHeat, gridScale, borderSize, concurrencyLevel, logRate);
	int qty = 0;
	for (int y = 0; y < result->size.y; y++) {
		for (int x = 0; x < result->size.x; x++) {
			double v;
			ifs >> v;
			qty++;
			result->grid [0][x][y] = v;
		}
	}
	for (int x = 0; x < size.x; x++) {
		for (int y = 0; y < size.y; y++) {
			result->prop [x][y] = WorldHeat::THERMAL_DIFFUSIVITY_AIR;
		}
	}
	printf ("Read %d heat cells\n", qty);
	ifs.close ();
	return result;
}

WorldHeat::
~WorldHeat ()
{
	if (this->logStream != NULL) {
		cout << "Closing heat log\n";
		this->logStream->flush ();
		delete this->logStream;
	}
}

bool WorldHeat::validParameters (double deltaTime) const
{
	double alpha = 
		this->partialAlpha
		* WorldHeat::THERMAL_DIFFUSIVITY_COPPER
		* deltaTime
		;
	return alpha <= 0.25;
}

double WorldHeat::getHeatAt (const Vector &pos) const
{
	int x, y;
	toIndex (pos, x, y);
	return this->grid [this->adtIndex][x][y];
}

void WorldHeat::
setHeatAt (const Vector &pos, double value)
{
	int x, y;
	toIndex (pos, x, y);
	this->grid [this->adtIndex][x][y] = value;
}

double WorldHeat::
getHeatDiffusivityAt (const Point &pos) const
{
	int x, y;
	toIndex (pos, x, y);
	return this->prop [x][y];
}

void WorldHeat::
setHeatDiffusivityAt (const Point &pos, double value)
{
	int x, y;
	toIndex (pos, x, y);
	this->prop [x][y] = value;
}


void WorldHeat::
initParameters (const ExtendedWorld *world)
{
	if (this->initFlag) {
		for (int x = 0; x < this->size.x; x++) {
			for (int y = 0; y < this->size.y; y++) {
				for (int i = 0; i < 2; i++) {
					this->grid [i][x][y] = this->normalHeat;
				}
				this->prop [x][y] = WorldHeat::THERMAL_DIFFUSIVITY_AIR;
			}
		}
	}
}

void WorldHeat::
initStateComputing (double deltaTime)
{
}

void WorldHeat::
computeNextState (double deltaTime)
{
	this->relativeTime += deltaTime;
	if (this->logStream != NULL) {
		if (this->iterationsToNextLog == 0) {
			dumpState (*this->logStream);
		}
		else {
			this->iterationsToNextLog--;
		}
	}
#ifdef WORLDHEAT_SERIAL
	const int nextAdtIndex = 1 - this->adtIndex;
	const double alpha = this->partialAlpha * deltaTime;
	for (int x = 1; x < this->size.x - 1; x++) {
		for (int y = 1; y < this->size.y - 1; y++) {
			const double currentHeat = this->grid [this->adtIndex][x][y];
			const double deltaHeat =
				(
				 + (this->grid [this->adtIndex][x][y + 1] - currentHeat) * this->prop [x][y + 1]
				 + (this->grid [this->adtIndex][x][y - 1] - currentHeat) * this->prop [x][y - 1]
				 + (this->grid [this->adtIndex][x + 1][y] - currentHeat) * this->prop [x + 1][y]
				 + (this->grid [this->adtIndex][x - 1][y] - currentHeat) * this->prop [x - 1][y]
				 + (this->normalHeat - currentHeat ) * CELL_DISSIPATION
				 ) * alpha
				;
			this->grid [nextAdtIndex][x][y] =
				this->grid [this->adtIndex][x][y] + deltaHeat;
		}
	}
	this->adtIndex = nextAdtIndex;
#else
	AbstractGridParallelSimulation::updateState (deltaTime);
#endif
}

void WorldHeat::
updateGrid (double deltaTime, int xmin, int ymin, int xmax, int ymax)
{
	const int nextAdtIndex = 1 - this->adtIndex;
	const double alpha = this->partialAlpha * deltaTime;
	for (int x = xmin; x < xmax; x++) {
		for (int y = ymin; y < ymax; y++) {
			const double currentHeat = this->grid [this->adtIndex][x][y];
			const double deltaHeat =
				(
				 + (this->grid [this->adtIndex][x][y + 1] - currentHeat) * this->prop [x][y + 1]
				 + (this->grid [this->adtIndex][x][y - 1] - currentHeat) * this->prop [x][y - 1]
				 + (this->grid [this->adtIndex][x + 1][y] - currentHeat) * this->prop [x + 1][y]
				 + (this->grid [this->adtIndex][x - 1][y] - currentHeat) * this->prop [x - 1][y]
				 + (this->normalHeat - currentHeat ) * CELL_DISSIPATION
				 ) * alpha
				;
			this->grid [nextAdtIndex][x][y] =
				this->grid [this->adtIndex][x][y] + deltaHeat;
		}
	}
}

void WorldHeat::
saveState (std::string filename) const
{
	ofstream ofs (filename.c_str (), std::ofstream::out | std::ofstream::trunc);
	ofs << "borderSize " << this->borderSize << '\n';
	ofs << "gridScale " << this->gridScale << '\n';
	ofs << "size " << this->size.x << ' ' << this->size.y << '\n';
	ofs << "origin "<< this->origin.x << ' ' << this->origin.y << '\n';
	ofs << "normalHeat " << this->normalHeat<< '\n';
	int ai = this->adtIndex;
	for (int y = 0; y < this->size.y; y++) {
		for (int x = 0; x < this->size.x; x++) {
			ofs << ' ' << this->grid [ai][x][y];
		}
		ofs << '\n';
	}
	ofs.close ();
}

void WorldHeat::
dumpState (ostream &os)
{
#ifdef DUMP_MATRIX
	for (int y = 1; y < this->size.y - 1; y++) {
		os << this->grid [this->adtIndex][1][y];
		for (int x = 2; x < this->size.x - 1; x++) {
			os << '\t' << this->grid [this->adtIndex][x][y];
		}
		os << '\n';
	}
	os << '\n';
#endif
// #ifdef DUMP_LINE
	os << this->relativeTime;
	for (int y = 1; y < this->size.y - 1; y++) {
		for (int x = 1; x < this->size.x - 1; x++) {
			os << '\t' << this->grid [this->adtIndex][x][y];
		}
	}
	os << '\n';
// #endif
	this->iterationsToNextLog = this->logRate;
}

void WorldHeat::
resetTemperature (double value)
{
	for (int x = this->size.x - 1; x >= 0; x--) {
		for (int y = this->size.y - 1; y >= 0; y--) {
			this->grid [this->adtIndex][x][y] = value;
		}
	}
}
