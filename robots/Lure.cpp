#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include "Lure.h"

static const double pi = boost::math::constants::pi<double>();

using namespace Enki;

const double Lure::SCALE_FACTOR = 1.0;

const double Lure::DISTANCE_BETWEEN_WHEELS = 0.8;
const double Lure::MAX_SPEED = 50.0;
const double Lure::NOISE_AMOUNT = 0;

const int Lure::NUMBER_RANGE_SENSORS = 6;
const double Lure::RANGE_SENSOR_HEIGHT = 0;
const double Lure::RANGE_SENSOR_RANGE = 2;
const double Lure::RANGE_SENSOR_M = 3731
const double Lure::RANGE_SENSOR_X0 = 0.3;
const double Lure::RANGE_SENSOR_C = 0.7;
const double Lure::RANGE_SENSOR_NOISESD = 0.0;

const double Lure::COLOUR_RED = 0.33;
const double Lure::COLOUR_GREEN = 0.33;
const double Lure::COLOUR_BLUE = 0.33;

const double Lure::COLLISION_ELASTICITY = 0.1;
const double Lure::DRY_FRICTION_COEFFICIENT = 0.25;

const double Lure::MASS = 10;

const double Lure::WIDTH = 0.8;
const double Lure::LENGTH = 7;
const double Lure::HEIGHT = 1.2;

Lure::Lure (double scaleFactor) :
    DifferentialWheeled (Lure::DISTANCE_BETWEEN_WHEELS, Lure::MAX_SPEED, Lure::NOISE_AMOUNT),
    range_sensors (NUMBER_RANGE_SENSORS),
    color_r_ (Lure::COLOUR_RED),
    color_g_ (Lure::COLOUR_GREEN),
    color_b_ (Lure::COLOUR_BLUE)
{
    // set shape and mass
    Point points[9] = {
        Point (Lure::LENGTH / 2                        ,  Lure::WIDTH / 6),
        Point (Lure::LENGTH / 2 - Lure::WIDTH / 3      ,  Lure::WIDTH / 2 - Lure::WIDTH / 7),
        Point (Lure::LENGTH / 3                        ,  Lure::WIDTH / 2),
        Point (-Lure::LENGTH / 4                       ,  Lure::WIDTH / 4),
        Point (-Lure::LENGTH / 2                       ,  0),
        Point (-Lure::LENGTH / 4                       ,  -Lure::WIDTH / 4),
        Point (Lure::LENGTH / 3                        ,  -Lure::WIDTH / 2),
        Point (Lure::LENGTH / 2 - Lure::WIDTH / 3      ,  -Lure::WIDTH / 2 + Lure::WIDTH / 7),
        Point (Lure::LENGTH / 2                        ,  -Lure::WIDTH / 6)
    };
    Polygone footprint;
    for (int i = 0; i < 9; i++) {
        footprint.push_back (points [i] * scaleFactor);
    }
    PhysicalObject::Hull hull (PhysicalObject::Part (footprint, Lure::HEIGHT));
    setCustomHull (hull, Lure::MASS);
    // set colour
    this->setColor (Lure::COLOUR_RED, Lure::COLOUR_GREEN, Lure::COLOUR_BLUE);
    // set other physical properties
    this->PhysicalObject::collisionElasticity = Lure::COLLISION_ELASTICITY;
    this->PhysicalObject::dryFrictionCoefficient = Lure::DRY_FRICTION_COEFFICIENT;
    // set proximity sensors
    for (int i = 0; i < NUMBER_RANGE_SENSORS; i++) {
        double angle = 2 * pi / i;
        Vector relativePosition (Lure::LENGTH / 2 * cos (angle), Lure::WIDTH / 2 * sin (angle));
        range_sensors [i] = new IRSensor
            (this, relativePosition,
             Lure::RANGE_SENSOR_HEIGHT, angle, Lure::RANGE_SENSOR_RANGE,
             Lure::RANGE_SENSOR_M, Lure::RANGE_SENSOR_X0, Lure::RANGE_SENSOR_C,
             Lure::RANGE_SENSOR_NOISESD);
    }
}

/* virtual */
Lure::~Lure ()
{
    BOOST_FOREACH (ObjectSensor* p, range_sensors) {
        delete p;
    }
}

void Lure::step (int deltaTime, const World *world)
{
}

void Lure::setColor (double r, double g, double b)
{
    color_r_ = r;
    color_g_ = g;
    color_b_ = b;
    PhysicalObject::setColor (Color (r, g, b, 1));
}


// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// indent-tabs-mode: nil
// tab-width: 4
// c-basic-offset: 4
// End: 
