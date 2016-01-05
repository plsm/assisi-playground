/*! \file  Fish.h
    \brief A simplistic fish model implementation.
*/
#ifndef ENKI_FISH_H
#define ENKI_FISH_H

#include <enki/robots/DifferentialWheeled.h>
#include <enki/interactions/CircularCam.h>
#include "extensions/ExtendedRobot.h"
#include "interactions/ObjectSensor.h"
#include "interactions/HeatSensor.h"
#include "interactions/LightSensor.h"

namespace Enki
{
    /**
     * A simplistic fish model.
     *
     * A fish is based on a differential wheeled robot.  
     *
     * Zebra fish are drawn in a light grey colour.  They are represented by
     * an ellipse with length 70mm, width 8mm and height 12mm.
     *
     * \ingroup robot */
    class Fish :
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
         * Number of object sensors in zebrafish.
         */
        static const int NUMBER_OBJECT_SENSORS;
        /**
         * Height of object sensors.
         */
        static const double OBJECT_SENSOR_HEIGHT;
        /**
         * Maximum object of object sensors.
         */
        static const double OBJECT_SENSOR_RANGE;
        /**
         * Value of parameter m of object sensor model.
         */
        static const double OBJECT_SENSOR_M;
        /**
         * Value of parameter x0 of object sensor model.
         */
        static const double OBJECT_SENSOR_X0;
        /**
         * Value of parameter c of object sensor model.
         */
        static const double OBJECT_SENSOR_C;
        /**
         * Standard deviation of noise in object sensor model.
         */
        static const double OBJECT_SENSOR_NOISESD;
        /**
         */
        static const double CAMERA_HEIGHT;
        /**
         * Half aperture of a circular camera.  The real field of view is twice
         * this value [0; PI/2].  Circular cameras are used to model fish's
         * eyes.
         */
        static const double CAMERA_HALF_FIELD_OF_VIEW;
        /**
         * Overlap of the two circular cameras.  Circular cameras are used to
         * model fish's eyes.
         */
        static const double CAMERA_FIELD_OF_VIEW_OVERLAP;
        /**
         * Number of pixel to cover the full field of view of a circular camera.
         * Circular cameras are used to model fish's eyes.
         */
        static const unsigned CAMERA_PIXEL_COUNT;
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
        //! Create a Fish
        Fish (double scaleSize = Fish::SCALE_FACTOR);
        
        //! destructor
        virtual ~Fish ();

        /* Sensors */

        typedef std::vector<ObjectSensor*> ObjectSensorVector;

        //! object proximity sensors.
        ObjectSensorVector object_sensors;

        typedef std::vector<CircularCam *> CircularCamVector;
        /**
         * Fish eyes are modelled by two circular cameras.
         *
         * Instance at index 0 points to direction:
         *
         * Fish::CAMERA_HALF_FIELD_OF_VIEW - Fish::CAMERA_FIELD_OF_VIEW_OVERLAP
         *
         * while instance at index 1 points to direction
         *
         * -Fish::CAMERA_HALF_FIELD_OF_VIEW + Fish::CAMERA_FIELD_OF_VIEW_OVERLAP
         *
         * Instance at index 0 corresponds to left eye while instance at index 1
         * corresponds to right eye.
         */
        CircularCamVector eyes;

        /* Properties */
        

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
