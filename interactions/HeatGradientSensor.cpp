#include "interactions/HeatGradientSensor.h"

#include "interactions/WorldHeat.h"

using namespace Enki;

HeatGradientSensor::
HeatGradientSensor (Robot* owner, Vector relativePosition,
                    double minMeasurableHeat, double maxMeasurableHeat,
                    double thermalResponseTime,
                    double ambientTemperature,
                    int radius
                    ):
	Component (owner, relativePosition, Component::OMNIDIRECTIONAL), 
    HeatSensor (owner, relativePosition, minMeasurableHeat, maxMeasurableHeat, thermalResponseTime, ambientTemperature),
    radius (radius),
    measuredGradient (Vector (0, 0))
{
}

HeatGradientSensor::
HeatGradientSensor (Robot* owner, Vector relativePosition,
                    double minMeasurableHeat, double maxMeasurableHeat,
                    int radius
                    ):
	Component (owner, relativePosition, Component::OMNIDIRECTIONAL), 
    HeatSensor (owner, relativePosition, minMeasurableHeat, maxMeasurableHeat),
    radius (radius),
    measuredGradient (Vector (0, 0))
{
}

void HeatGradientSensor::
init (double dt, PhysicSimulation* ps)
{
    HeatSensor::init (dt, ps);
}

void HeatGradientSensor::
step (double dt, PhysicSimulation* ps)
{
    WorldHeat *worldHeat = dynamic_cast<WorldHeat *> (ps);
    if (worldHeat != NULL) {
        this->measuredHeat = worldHeat->getHeatAt (this->absolutePosition);
        int xc, yc, bx, by;
        double highestHeat = this->measuredHeat;
        worldHeat->toIndex (this->absolutePosition, xc, yc);
        bx = xc; by = yc;
        for (int x = xc - this->radius; x <= xc + this->radius; x++) {
            for (int y = yc - this->radius; y <= yc + this->radius; y++) {
                double currentHeat = worldHeat->cell (x, y);
                if (currentHeat > highestHeat) {
                    highestHeat = currentHeat;
                    bx = x; by = y;
                }
            }
        }
        int vx = bx - xc;
        int vy = by - yc;
        double angle = atan2 (vy, vx) - Component::owner->angle;
        double dist = sqrt (vx * vx + vy * vy);
        this->measuredGradient.x = dist * cos (angle);
        this->measuredGradient.y = dist * sin (angle);
    }
}


// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// indent-tabs-mode: nil
// tab-width: 4
// c-basic-offset: 4
// End: 

