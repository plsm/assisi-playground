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
            double currentTemperature = worldHeat->getHeatAt (this->absolutePosition + (*(this->mesh)) [0]);
            double temperatureDifference = this->targetTemperature - currentTemperature;
            double absoluteTemperatureDifference = fabs (temperatureDifference);
            double absoluteTemperatureChange = std::min (absoluteTemperatureDifference, dt * this->slope);
            double newTemperature = currentTemperature + boost::math::sign (temperatureDifference) * absoluteTemperatureChange;
            // std::cout << "HeatActuatorSlope::step(" << dt
            //           << ")  ct=" << currentTemperature
            //           << "  td=" << temperatureDifference
            //           << "  atc=" << absoluteTemperatureChange << '\n';
            for (int i = this->mesh->size () - 1; i >= 0; i--) {
                worldHeat->setHeatAt (this->absolutePosition + (*(this->mesh)) [i], newTemperature);
            }
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
