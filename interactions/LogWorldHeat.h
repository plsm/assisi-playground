#ifndef __LOG_WORLD_HEAT_
#define __LOG_WORLD_HEAT_

#include <set>
#include <string>

#include <enki/Geometry.h>

#include "interactions/WorldHeat.h"

namespace Enki
{

    class LogWorldHeat:
        public WorldHeat
    {
    public:
        enum Header {WORLD_COORDINATES, WORLD_X_COORDINATE, CELL_COORDINATES};
    private:
        std::set<Point> cellsToLog;
        std::string logFileName;
        Header header;
        FILE *logFilePtr;
    public:
        LogWorldHeat (const ExtendedWorld *world, double normalHeat, double gridScale, double borderSize, double concurrencyLevel, bool constantTemperatureFlag = false, int logRate = 1);
        virtual ~LogWorldHeat ();
        virtual void computeNextState (double deltaTime);
        void setLogFileName (const std::string &logFileName);
        void setHeader (Header header);
        void openLogFile ();
        void closeLogFile ();
        void addRectangle (const Point &min_position, const Point &max_position);
    };
}
#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
