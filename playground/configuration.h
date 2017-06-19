#ifndef __CONFIGURATION__
#define __CONFIGURATION__

#include <boost/program_options.hpp>

#include "playground/WorldExt.h"

namespace Enki {
    typedef enum {
        MODE_GUI,
        MODE_NO_VIEWER
    } Mode;

    extern double env_temp;

    extern Mode mode;

    extern double timerPeriod;

    /**
     * Return a description of the standard options that can be passed in the
     * command line or be presented in a configuration file.
     */
    boost::program_options::options_description standardOptions ();

    /**
     * Process the configuration options that can be passed in the command line
     * or be in the 'Playground.cfg' file.
     */
    boost::program_options::variables_map processConfiguration (int argc, char *argv[], const boost::program_options::options_description &desc);

    /**
     * Create a world for the corresponding mode.
     */
    WorldExt *createWorld (const boost::program_options::variables_map &vm);
}

#endif // __CONFIGURATION__
