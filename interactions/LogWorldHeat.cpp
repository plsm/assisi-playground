#include <iostream>

#include "LogWorldHeat.h"

using namespace std;
using namespace Enki;

LogWorldHeat::LogWorldHeat (const ExtendedWorld *world, double normalHeat, double gridScale, double borderSize, double concurrencyLevel, bool constantTemperatureFlag, int logRate):
    AbstractGrid (world, gridScale, borderSize),
    WorldHeat (world, normalHeat, gridScale, borderSize, concurrencyLevel, logRate),
    logFileName ("heatlog.csv"),
    logFilePtr (NULL),
    header (CELL_COORDINATES)
{
}

LogWorldHeat::~LogWorldHeat ()
{
    std::cout << "Closing heat log file\n";
    this->closeLogFile ();
}

void LogWorldHeat::computeNextState (double deltaTime)
{
    WorldHeat::computeNextState (deltaTime);
    if (this->logFilePtr != NULL) {
        fprintf (this->logFilePtr, "%lf", this->relativeTime);
        for (set<Point>::iterator it = this->cellsToLog.begin (); it != this->cellsToLog.end (); it++) {
            int x = it->x;
            int y = it->y;
            fprintf (this->logFilePtr, ";%lf", this->grid [this->adtIndex][x][y]);
        }
        fprintf (this->logFilePtr, "\n");
    }
}

void LogWorldHeat::setLogFileName (const string &logFileName)
{
    this->closeLogFile ();
    this->logFileName = logFileName;
}

void LogWorldHeat::setHeader (Header header)
{
    this->header = header;
}

void LogWorldHeat::openLogFile ()
{
    this->closeLogFile ();
    this->logFilePtr = fopen (this->logFileName.c_str (), "w");
    fprintf (this->logFilePtr, "\"time\"");
    for (set<Point>::iterator it = this->cellsToLog.begin (); it != this->cellsToLog.end (); it++) {
        int cx = it->x;
        int cy = it->y;
        double wx = it->x * this->gridScale + this->origin.x;
        double wy = it->y * this->gridScale + this->origin.y;
        switch (this->header) {
        case CELL_COORDINATES:
            fprintf (this->logFilePtr, ";\"(%d,%d)\"", cx, cy);
            break;
        case WORLD_COORDINATES:
            fprintf (this->logFilePtr, ";\"(%d,%d)\"", wx, wy);
            break;
        case WORLD_X_COORDINATE:
            fprintf (this->logFilePtr, ";\"x=%.1f\"", wx);
            break;
        }
    }
    fprintf (this->logFilePtr, "\n");
}

void LogWorldHeat::closeLogFile ()
{
    if (this->logFilePtr != NULL) {
        fclose (this->logFilePtr);
        this->logFilePtr = NULL;
    }
}

void LogWorldHeat::addRectangle (const Point &min_position, const Point &max_position)
{
    Point it;
    for (it.x = min_position.x; it.x <= max_position.x; it.x += this->gridScale) {
        for (it.y = min_position.y; it.y <= max_position.y; it.y += this->gridScale) {
            int gx, gy;
            this->toIndex (it, gx, gy);
            this->cellsToLog.insert (Point (gx, gy));
        }
    }
}
