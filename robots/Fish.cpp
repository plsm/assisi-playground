#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include "Fish.h"

const double pi = boost::math::constants::pi<double>();

using namespace Enki;

const double Fish::SCALE_FACTOR = 1.0;

const double Fish::DISTANCE_BETWEEN_WHEELS = 0.8;
const double Fish::MAX_SPEED = 50.0;
const double Fish::NOISE_AMOUNT = 0;

const double Fish::COLOUR_RED = 0.6;
const double Fish::COLOUR_GREEN = 0.6;
const double Fish::COLOUR_BLUE = 0.6;

const double Fish::COLLISION_ELASTICITY = 0.1;
const double Fish::DRY_FRICTION_COEFFICIENT = 0.25;

const double Fish::MASS = 10;

const double Fish::WIDTH = 0.8;
const double Fish::LENGTH = 7;
const double Fish::HEIGHT = 1.2;

Fish::Fish (double scaleFactor) :
	DifferentialWheeled (Fish::DISTANCE_BETWEEN_WHEELS, Fish::MAX_SPEED, Fish::NOISE_AMOUNT),
	color_r_ (Fish::COLOUR_RED),
	color_g_ (Fish::COLOUR_GREEN),
	color_b_ (Fish::COLOUR_BLUE)
{
	// set shape and mass
	Polygone footprint;
	for (double angle = 0; angle < 2 * pi; angle += pi / 3) {
		double x = Fish::LENGTH / 2 * cos (angle);
		double y = Fish::WIDTH / 2 * sin (angle);
		footprint.push_back (Point (x, y) * scaleFactor);
	}
	PhysicalObject::Hull hull (PhysicalObject::Part (footprint, Fish::HEIGHT));
	setCustomHull (hull, Fish::MASS);
	// set colour
	this->setColor (Fish::COLOUR_RED, Fish::COLOUR_GREEN, Fish::COLOUR_BLUE);
	// set other physical properties
	this->PhysicalObject::collisionElasticity = Fish::COLLISION_ELASTICITY;
	this->PhysicalObject::dryFrictionCoefficient = Fish::DRY_FRICTION_COEFFICIENT;
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

