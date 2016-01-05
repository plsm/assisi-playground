#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include "Fish.h"

const double pi = boost::math::constants::pi<double>();

using namespace Enki;

const double Fish::SCALE_FACTOR = 1.0;

const double Fish::DISTANCE_BETWEEN_WHEELS = 0.8;
const double Fish::MAX_SPEED = 50.0;
const double Fish::NOISE_AMOUNT = 0;

const int Fish::NUMBER_OBJECT_SENSORS = 5;
const double Fish::OBJECT_SENSOR_HEIGHT = 0;
const double Fish::OBJECT_SENSOR_RANGE = 20;
const double Fish::OBJECT_SENSOR_M = 3731;
const double Fish::OBJECT_SENSOR_X0 = 0.3;
const double Fish::OBJECT_SENSOR_C = 0.7;
const double Fish::OBJECT_SENSOR_NOISESD = 0.0;

const double Fish::CAMERA_HEIGHT = 1;
const double Fish::CAMERA_HALF_FIELD_OF_VIEW = (3 * pi / 4) / 2;
const double Fish::CAMERA_FIELD_OF_VIEW_OVERLAP = 0;
const unsigned Fish::CAMERA_PIXEL_COUNT = 10;

const double Fish::COLOUR_RED = 0.55;
const double Fish::COLOUR_GREEN = 0.55;
const double Fish::COLOUR_BLUE = 0.65;

const double Fish::COLLISION_ELASTICITY = 0.1;
const double Fish::DRY_FRICTION_COEFFICIENT = 0.25;

const double Fish::MASS = 10;

const double Fish::WIDTH = 0.8;
const double Fish::LENGTH = 7;
const double Fish::HEIGHT = 1.2;

Fish::Fish (double scaleFactor) :
    DifferentialWheeled (Fish::DISTANCE_BETWEEN_WHEELS, Fish::MAX_SPEED, Fish::NOISE_AMOUNT),
    object_sensors (Fish::NUMBER_OBJECT_SENSORS),
    eyes (2),
    color_r_ (Fish::COLOUR_RED),
    color_g_ (Fish::COLOUR_GREEN),
    color_b_ (Fish::COLOUR_BLUE)
{
    // set shape and mass
    Point points[9] = {
        Point ( Fish::LENGTH / 2                    ,  Fish::WIDTH / 6),
        Point ( Fish::LENGTH / 2 - Fish::WIDTH / 3  ,  Fish::WIDTH / 2 - Fish::WIDTH / 7),
        Point ( Fish::LENGTH / 3                    ,  Fish::WIDTH / 2),
        Point (-Fish::LENGTH / 4                    ,  Fish::WIDTH / 4),
        Point (-Fish::LENGTH / 2                    ,  0),
        Point (-Fish::LENGTH / 4                    , -Fish::WIDTH / 4),
        Point ( Fish::LENGTH / 3                    , -Fish::WIDTH / 2),
        Point ( Fish::LENGTH / 2 - Fish::WIDTH / 3  , -Fish::WIDTH / 2 + Fish::WIDTH / 7),
        Point ( Fish::LENGTH / 2                    , -Fish::WIDTH / 6)
    };
    Polygone footprint;
    for (int i = 0; i < 9; i++) {
        footprint.push_back (points [i] * scaleFactor);
    }
    PhysicalObject::Hull hull (PhysicalObject::Part (footprint, Fish::HEIGHT));
    setCustomHull (hull, Fish::MASS);
    // set colour
    this->setColor (Fish::COLOUR_RED, Fish::COLOUR_GREEN, Fish::COLOUR_BLUE);
    // set other physical properties
    this->PhysicalObject::collisionElasticity = Fish::COLLISION_ELASTICITY;
    this->PhysicalObject::dryFrictionCoefficient = Fish::DRY_FRICTION_COEFFICIENT;
    // set object sensors
    for (int i = 0; i < NUMBER_OBJECT_SENSORS; i++) {
        double angle = pi / 2 - pi * i / NUMBER_OBJECT_SENSORS / 4;
        Vector relativePosition (Fish::LENGTH / 2 * cos (angle), Fish::WIDTH / 2 * sin (angle));
        object_sensors [i] = new ObjectSensor
            (this, relativePosition,
             Fish::OBJECT_SENSOR_HEIGHT, angle, Fish::OBJECT_SENSOR_RANGE,
             Fish::OBJECT_SENSOR_M, Fish::OBJECT_SENSOR_X0, Fish::OBJECT_SENSOR_C,
             Fish::OBJECT_SENSOR_NOISESD);
        addLocalInteraction (object_sensors [i]);
    }
    // set circular camera
    for (int i = -1; i <= 1; i += 2) {
        angle = i * Fish::CAMERA_HALF_FIELD_OF_VIEW - i * Fish::CAMERA_FIELD_OF_VIEW_OVERLAP;
        Vector relativePosition (Fish::LENGTH / 2 * cos (angle), Fish::WIDTH / 2 * sin (angle));
        eyes [(-i + 1) / 2] = new CircularCam
            (this, relativePosition,
             Fish::CAMERA_HEIGHT, Fish::angle,
             Fish::CAMERA_HALF_FIELD_OF_VIEW, Fish::CAMERA_PIXEL_COUNT);
        addLocalInteraction (eyes [(-i + 1) / 2]);
    }
}

/* virtual */
Fish::~Fish ()
{
    BOOST_FOREACH (ObjectSensor* p, object_sensors) {
        delete p;
    }
}

void Fish::step (int deltaTime, const World *world)
{
}

void Fish::setColor (double r, double g, double b)
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
