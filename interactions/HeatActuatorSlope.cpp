#include <iostream>
#include <algorithm>

#include <boost/math/special_functions/sign.hpp>

#include "interactions/HeatActuatorSlope.h"

using namespace Enki;

HeatActuatorSlope::HeatActuatorSlope (
    Enki::Robot* owner,
    Enki::Vector relativePosition,
    double targetTemperature,
    double slope,
    PointMesh *mesh):
    PhysicInteraction (owner),
    Component (owner, relativePosition, Component::OMNIDIRECTIONAL), 
    targetTemperature (targetTemperature),
    slope (slope),
    switchedOn (false),
    mesh (mesh)
{
    Component::init ();
}

HeatActuatorSlope::
~HeatActuatorSlope ()
{
    delete this->mesh;
}

void HeatActuatorSlope::
init (double dt, PhysicSimulation *ps)
{
    Component::init ();
}

#ifdef DEBUG
int updates = 9;
#endif

void HeatActuatorSlope::
step (double dt, PhysicSimulation *ps)
{
    WorldHeat *worldHeat = dynamic_cast<WorldHeat *> (ps);
    if (worldHeat != NULL) {
        // CASU *casu = dynamic_cast<CASU *> (this->owner);
        // if (casu != NULL) {
        //     std::cout << casu->temp_sensors [0]->getMeasuredHeat() << '\n';
        // }
        if (this->switchedOn) {
            int ox, oy;
            worldHeat->toIndex (this->absolutePosition, ox, oy);
            const Point &p0 ((*(this->mesh)) [0]);
            double currentTemperature = worldHeat->getHeatAtRaw (ox + (int) p0.x, oy + (int) p0.y);
            double temperatureDifference = this->targetTemperature - currentTemperature;
            double absoluteTemperatureDifference = fabs (temperatureDifference);
            double absoluteTemperatureChange = std::min (absoluteTemperatureDifference, dt * this->slope);
            double newTemperature = currentTemperature + boost::math::sign (temperatureDifference) * absoluteTemperatureChange;
            for (int i = this->mesh->size () - 1; i >= 0; i--) {
                const Point &pi ((*(this->mesh)) [i]);
                worldHeat->setHeatAtRaw (ox + (int) pi.x, oy + (int) pi.y, newTemperature);
#ifdef DEBUG
                if (updates > 0) {
                    std::cout << " (" << ox + (int) pi.x << "," << oy + (int) pi.y << ")";
                }
#endif
            }
#ifdef DEBUG
            if (updates > 0) {
                updates--;
                std::cout << "\n";
            }
#endif
        }
    }
}

void HeatActuatorSlope::
print (std::ostream &os) const
{
    os << "HeatActuatorSlope("
       << " owner=" << this->Component::owner
       << " relativePosition=" << this->relativePosition
       << " targetTemperature=" << this->targetTemperature
       << " slope=" << this->slope
       << " switchedOn=" << this->switchedOn
       << " )";
}

std::ostream &operator<< (std::ostream &os, const HeatActuatorSlope &heatActuatorSlope)
{
    heatActuatorSlope.print (os);
    return os;
}
