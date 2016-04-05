/*

 */

#include <cmath>

#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>

#include <PhysicalEngine.h>

#include "Bee.h"
#include "interactions/LightConstants.h"

const double pi = boost::math::constants::pi<double>();

// Signum function
template <typename T>
int sign(T val)
{
    return (T(0) < val) - (val < T(0));
}

namespace Enki
{

    const Vector Bee::AIR_FLOW_SENSOR_POSITION (0, 0);
    /*const*/ double Bee::AIR_FLOW_SENSOR_RANGE = 5;
    const double Bee::AIR_FLOW_SENSOR_ORIENTATION = 0;

    const int Bee::NUMBER_VIBRATION_SENSORS = 4;
    double Bee::VIBRATION_SENSOR_RANGE = 100;
    const Vector Bee::VIBRATION_SENSOR_POSITION[] = {
        Vector (-2.5,  0),
        Vector ( 2.5,  0),
        Vector (   0, -2.5),
        Vector (   0,  2.5)
    };
    /*const*/ double Bee::VIBRATION_SENSOR_MAX_MEASURABLE_FREQUENCY = 500;
    /*const*/ double Bee::VIBRATION_SENSOR_AMPLITUDE_STANDARD_DEVIATION_GAUSSIAN_NOISE = 0;
    /*const*/ double Bee::VIBRATION_SENSOR_FREQUENCY_STANDARD_DEVIATION_GAUSSIAN_NOISE = 0;

    Bee::Bee(const std::string &name, double body_length, double body_width, double body_height,
             double body_mass, double max_speed) :
        ExtendedRobot (name),
        len_(body_length), w_(body_width), h_(body_height),
        m_(body_mass), v_max_(max_speed),
        DifferentialWheeled(body_width, max_speed, 0.0),
        object_sensors(5),
        vibration_sensors (Bee::NUMBER_VIBRATION_SENSORS),
        color_r_(0.93), color_g_(0.79), color_b_(0)
    {
        collisionElasticity = 0.1;
        dryFrictionCoefficient=0.25;
        // Set shape & color

        //setRectangular(len_, w_, h_, 1);
        Polygone footprint;
        footprint.push_back(Point(len_/2,w_/4));
        footprint.push_back(Point(len_/2-w_/(4*sqrt(2)),w_/2));
        footprint.push_back(Point(-len_/2+w_/(4*sqrt(2)),w_/2));
        footprint.push_back(Point(-len_/2,w_/4));
        footprint.push_back(Point(-len_/2,-w_/4));
        footprint.push_back(Point(-len_/2+w_/(2*sqrt(2)),-w_/2));
        footprint.push_back(Point(len_/2-w_/(2*sqrt(2)),-w_/2));
        footprint.push_back(Point(len_/2,-w_/4));
        PhysicalObject::Hull hull(PhysicalObject::Part(footprint, h_));
        setCustomHull(hull, m_);
        setColor(color_r_, color_g_ , color_b_);

        // Set other physical properties
        PhysicalObject::dryFrictionCoefficient = 2.5;

        int i = 0;
        for (double a = -pi/2; i < 5; i++, a += pi/4) 
            {
            object_sensors[i] = new ObjectSensor
               (this, 
                Vector(len_/2-sin(a)*w_/2, sin(a)*w_/2),
                0, a, 10, 3731, 0, 0.7, 0);
            addLocalInteraction(object_sensors[i]);
            }

        double light_sensor_range = 10.0;
        light_sensor_blue = new LightSensor
           (light_sensor_range, this,
            Vector(0,0), 0.0, Light::Blue);
        addLocalInteraction(light_sensor_blue);

        // Check in the model why is this necessary
        double minMeasurableHeat = 0.0;
        double maxMeasurableHeat = 100.0;
        heat_sensor = new HeatSensor
           (this, Vector(0,0),
            minMeasurableHeat,
            maxMeasurableHeat);
        addPhysicInteraction(heat_sensor);

        // Add vibration sensors
        for (int i = 0; i < Bee::NUMBER_VIBRATION_SENSORS; i++) {
            VibrationSensor *vs = new VibrationSensor 
                (Bee::VIBRATION_SENSOR_RANGE, this,
                 Bee::VIBRATION_SENSOR_POSITION [i], 0,
                 Bee::VIBRATION_SENSOR_MAX_MEASURABLE_FREQUENCY,
                 Bee::VIBRATION_SENSOR_AMPLITUDE_STANDARD_DEVIATION_GAUSSIAN_NOISE,
                 Bee::VIBRATION_SENSOR_FREQUENCY_STANDARD_DEVIATION_GAUSSIAN_NOISE);
            this->vibration_sensors [i] = vs;
            addLocalInteraction (vs);
        }

        air_flow_sensor = new AirFlowSensor
            (Bee::AIR_FLOW_SENSOR_RANGE,
             this,
             Bee::AIR_FLOW_SENSOR_POSITION,
             Bee::AIR_FLOW_SENSOR_ORIENTATION);
        addLocalInteraction (this->air_flow_sensor);
    }

    /* virtual */
    Bee::~Bee()
    {
        BOOST_FOREACH(ObjectSensor* p, object_sensors)
        {
            delete p;
        }
        delete light_sensor_blue;
    }

    void Bee::setColor(double r, double g, double b)
    {
        color_r_ = r;
        color_g_ = g;
        color_b_ = b;
        PhysicalObject::setColor(Color(r,g,b,1));
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
