/*! \file  Lure.h
    \brief A simplistic fish lure model implementation.
*/
#ifndef ENKI_FISH_H
#define ENKI_FISH_H

#include <enki/robots/DifferentialWheeled.h>
#include "extensions/ExtendedRobot.h"
#include "interactions/IRSensor.h"

namespace Enki
{
    /**
     * A simplistic fish lure model.
     *
     * A fish lure is based on a differential wheeled robot.  
     *
     * Fish Lure are drawn in a dark grey colour.  They are represented by
     * an ellipse with length 70mm, width 8mm and height 12mm.
     *
     * \ingroup robot */
    class Lure :
        public DifferentialWheeled,
        public ExtendedRobot
    {
    public:
        /**
         * Default scale factor used when constructing a new fish.
         */
        static const double SCALE_FACTOR;
        /**
         * Distance between the fish 'wheels'.  Unit is centimetre.
         */
        static const double DISTANCE_BETWEEN_WHEELS;
        /**
         * Maximum speed of the zebra fish.
         */
        static const double MAX_SPEED;
        /**
         * Noise used in the motor model.
         */
        static const double NOISE_AMOUNT;
        /**
         * Number of range sensors in fish lure.
         */
        static const int NUMBER_RANGE_SENSORS;
        /**
         * Height of range sensors.
         */
        static const double RANGE_SENSOR_HEIGHT;
        /**
         * Maximum range of range sensors.
         */
        static const double RANGE_SENSOR_RANGE;
        /**
         * Value of parameter m of range sensor model.
         */
        static const double RANGE_SENSOR_M;
        /**
         * Value of parameter x0 of range sensor model.
         */
        static const double RANGE_SENSOR_X0;
        /**
         * Value of parameter c of range sensor model.
         */
        static const double RANGE_SENSOR_C;
        /**
         * Standard deviation of noise in range sensor model.
         */
        static const double RANGE_SENSOR_NOISESD;
        /**
         * Zebra fish are drawn in a light grey colour.
         */
        static const double COLOUR_RED;
        /**
         * Zebra fish are drawn in a light grey colour.
         */
        static const double COLOUR_GREEN;
        /**
         * Zebra fish are drawn in a light grey colour.
         */
        static const double COLOUR_BLUE;
        /**
         * Zebra fish are soft objects that dissipate 90% of energy.
         */
        static const double COLLISION_ELASTICITY;
        /**
         * The dry friction coefficient mu.
         */
        static const double DRY_FRICTION_COEFFICIENT;
        /**
         * Zebra fish mass in grams.
         */
        static const double MASS;
        /**
         * Zebra fish width.
         */
        static const double WIDTH;
        /**
         * Zebra fish length.
         */
        static const double LENGTH;
        /**
         * Zebra fish height.
         */
        static const double HEIGHT;
    public:
        //! Create a Fish lure
        Lure (double scaleSize = Fish::SCALE_FACTOR);
        
        //! destructor
        virtual ~Lure ();

        /* Sensors */

        typedef std::vector<IRSensor*> IRSensorVector;

        //! Proximity sensors.
        IRSensorVector range_sensors;    

        //! Fish color (useful for diagnostic purposes)
        double color_r_;
        double color_g_;
        double color_b_;

        /**
         * Update the position of this fish.  
         */
        void step (int dt, const World *);

        /**
         * Update the fish color (useful for diagnostic purposes).
         */
        void setColor (double r, double g, double b);
    };
}

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// indent-tabs-mode: nil
// tab-width: 4
// c-basic-offset: 4
// End: 
