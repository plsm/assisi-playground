#include <signal.h>
#include <sys/time.h>
#include <semaphore.h>

#include <QApplication>
#include <QImage>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "WorldExt.h"
#include "AssisiPlayground.h"

#include "extensions/ExtendedWorld.h"
#include "interactions/WorldHeat.h"
#include "interactions/LogWorldHeat.h"

#include "handlers/PhysicalObjectHandler.h"
#include "handlers/EPuckHandler.h"
#include "handlers/CasuHandler.h"
#include "handlers/BeeHandler.h"

#include "robots/Bee.h"
#include "robots/Casu.h"

#include <iostream>
#include <fstream>

// MAC workaround for Thomas
#if defined __APPLE__
#include <mach-o/dyld.h>
#include <sys/param.h>
#endif

using namespace std;
using namespace Enki;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

namespace Enki {
    double env_temp;
}

static WorldExt *world;

/**
 * Heat model used by ASSISIbf playground
 */
static WorldHeat *heatModel;

/**
 * Timer period used in the headless simulation mode.  If the timer period is
 * greater than zero we use an alarm to update the world at every {@code
 * timerPeriod} seconds.  Otherwise a loop is used to update the world.
 */
static double timerPeriod = 0.01;


/**
 * Delta time used to update world state.  It is equal to the value used by
 * enki.  Unit is seconds.
 */
static const double DELTA_TIME = .03;

static unsigned int skewMonitorRate = 60;

static double skewReportThreshold = 0.05;

/**
 * Semaphore used in the headless simulation mode with an alarm to block the
 * main thread.
 */
static sem_t block;

#define PHYSICS_OVERSAMPLING 3

static bool go = true;



static LogWorldHeat *logableHeatModel;
extern double test_diffusivity;

void closeLogFiles (int signum);

int main(int argc, char *argv[])
{
    //  QApplication app(argc, argv);

    /** Parse command line options **/
    
    po::options_description desc("Recognized options");
    
    // Variables to store the options
    int r;
    string config_file_name("Playground.cfg");
    // Create the world and the viewer
    string pub_address("tcp://*:5555"); 
    string sub_address("tcp://*:5556");
    // heat model parameters;
    string heat_state_filename;
    string heat_log_file_name;
    double heat_scale;
    int heat_border_size;
    char heatMode = 'D';

    double maxVibration;
    double parallelismLevel = 1.0;

    // Bee physical parameters
    double bee_body_length, bee_body_width, bee_body_height,
        bee_body_mass, bee_max_speed;


    string list_waiting_times;
    string list_target_temperatures;

    fs::path default_config = fs::path("");
    // MAC workaround for Thomas
#if defined __APPLE__
    char * pathbuf = new char[MAXPATHLEN];
    uint32_t* bufsize = new uint32_t(MAXPATHLEN);
    _NSGetExecutablePath(pathbuf, bufsize);
    default_config = fs::path(pathbuf);
    delete [] pathbuf;
    delete bufsize;
#elif defined __linux__
    default_config = fs::read_symlink(fs::path("/proc/self/exe"));
#endif
    char crap;
    default_config.remove_filename() /= "Playground.cfg";

    desc.add_options
        ()
        ("help,h", "produce help message")
        ("nogui", "run without viewer")
        ("config_file,c", 
         po::value<string>(&config_file_name)->default_value(default_config.native()),
         "configuration file name")
        ("pub_addr",
         po::value<string>(&pub_address)->default_value("tcp://*:5555"),
         "Address for publishing data, in the form tcp://hostname:port")
        ("sub_addr",
         po::value<string>(&sub_address)->default_value("tcp://*:5556"),
         "Address for subscribing to commands, in the form tcp://hostname:port")
        ("Arena.radius,r", po::value<int>(&r), 
         "playground radius, in cm")
        ("Heat.state", po::value<string>(&heat_state_filename)->default_value (""), 
         "use heat state stored in given filename")

        ("Heat.env_temp,t", po::value<double>(&env_temp), 
         "environment temperature, in C")
        ("Heat.scale", po::value<double>(&heat_scale), 
         "heat model scale")
        ("Heat.border_size", po::value<int>(&heat_border_size), "playground radius, in cm")
        (
            "Vibration.range",
            po::value<double> (&Casu::VIBRATION_SOURCE_RANGE),
            "vibration range, in cm"
            )
        (
            "Heat.log_file",
            po::value<string> (&heat_log_file_name)->default_value ("heat-log.csv"),
            "heat log file name"
            )
        (
            "Heat.cell_dissipation",
            po::value<double> (&WorldHeat::CELL_DISSIPATION),
            "heat lost by cells directly to outside world"
            )
        (
            "Heat.mode",
            po::value<char> (&heatMode),
            "mode the heat model should run. Valid options are: 'N' no heat model; 'S' static heat model; 'D' dynamic heat model (default)"
            )
       // (
       //  "Heat.casu_outer_circumference_diffusivity",
       //  po::value<double> (&test_diffusivity),
       //  "Heat diffusivity of circumference around CASU ring"
       //  )
        // (
        //  "Heat.thermal_diffusivity_air",
        //  po::value<double> (&WorldHeat::THERMAL_DIFFUSIVITY_AIR),
        //  "Heat diffusivity of normal grid cells"
        // )
        // (
        //  "Heat.thermal_diffusivity_copper",
        //  po::value<double> (&WorldHeat::THERMAL_DIFFUSIVITY_COPPER),
        //  "Heat diffusivity of grid cells where a CASU is located"
        // )
        (
            "AirFlow.pump_range",
            po::value<double> (&Casu::AIR_PUMP_RANGE),
            "maximum range of CASU air pump"
            )
        (
            "AirFlow.sensor_range",
            po::value<double> (&Bee::AIR_FLOW_SENSOR_RANGE),
            "maximum range of Bee air flow sensor"
            )
        (
            "Vibration.maximum_amplitude", 
            po::value<double> (&Casu::VIBRATION_SOURCE_MAXIMUM_AMPLITUDE),
            "maximum amplitude of vibration"
            )
        (
            "Vibration.amplitude_quadratic_decay", 
            po::value<double> (&Casu::VIBRATION_SOURCE_AMPLITUDE_QUADRATIC_DECAY),
            "quadratic decay of vibration amplitude"
            )
        (
            "Vibration.noise", 
            po::value<double> (&Casu::VIBRATION_SOURCE_NOISE),
            "vibration frequency noise"
            )
        (
            "Viewer.layer",
            po::value<char> (&crap),
            "which data layer should be displayed by default: N none, H heat, V vibration, A airflow"
            )
        (
            "Viewer.max_vibration",
            po::value<double> (&maxVibration),
            "maximum displayed vibration intensity"
            )
        (
            "Viewer.no_help",
            "do not show help text"
            )
        (
            "Simulation.timer_period",
            po::value<double> (&timerPeriod),
            "simulation timer period (in seconds)"
            )
        (
            "Simulation.parallelism_level",
            po::value<double> (&parallelismLevel),
            "Percentage of CPU threads to use"
            )
        (
            "Bee.body_length",
            po::value<double> (&bee_body_length),
            "Bee body length"
            )
        (
            "Bee.body_width",
            po::value<double> (&bee_body_width),
            "Bee body width"
            )
        (
            "Bee.body_height",
            po::value<double> (&bee_body_height),
            "Bee body height"
            )
        (
            "Bee.body_mass",
            po::value<double> (&bee_body_mass),
            "Bee body mass"
            )
        (
            "Bee.max_speed",
            po::value<double> (&bee_max_speed),
            "Maximum bee motion velocity"
            )
        (
             "Skew.rate",
             po::value<unsigned int> (&skewMonitorRate),
             "Rate at which we check skewness between real time and simulated time"
             )
        (
             "Skew.threshold",
             po::value<double> (&skewReportThreshold),
             "Threshold to print a message because of skewness between real time and simulated time"
            )
        (
         "HeatCalibration.target_temperatures",
         po::value<string> (&list_target_temperatures),
         "a list of temperatures. The CASU is going to heat to each one the values and then wait the corresponding time as specified in parameter waiting_times before going to the next temperature value."
         )
        (
         "HeatCalibration.waiting_times",
         po::value<string> (&list_waiting_times),
         "a list of absolute times where the CASU is going to change its temperature set point."
         )
       (
        "Casu.peltier_slope",
        po::value<double> (&Casu::PELTIER_SLOPE),
        "CASU peltier slop"
        )
        (
         "Casu.thermal_diffusivity_fake_ring",
         po::value<double> (&Casu::THERMAL_DIFFUSIVITY_FAKE_RING),
         "thermal diffusivity of fake ring around the CASUs to simulate low diffusion between cells in the heat model occupied by a CASU and other cells"
         )
        ;

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    // here we read the config file, so it seems crucial that notify is executed first
    ifstream config_file(config_file_name.c_str(), std::ifstream::in);
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close();
    // not clear what the consequences of >1 notify call are, but no issues noticed
    po::notify(vm);

    if (vm.count("help"))
    {
        cout << desc << endl;
        return 1;
    }
    //QImage texture("playground/world.png");
    QImage texture(QString(":/textures/ground_grayscale.png"));
    texture = QGLWidget::convertToGLFormat(texture);    
    //texture.invertPixels(QImage::InvertRgba);
    
    world = new WorldExt (r, pub_address, sub_address,
        Color::gray, 
        World::GroundTexture (
            texture.width(),
            texture.height(),
            (const uint32_t*) texture.constBits ()),
        skewMonitorRate,
        skewReportThreshold);

    cout << "Creating heat model with logging capabilities\n";
    logableHeatModel = new LogWorldHeat (world, env_temp, heat_scale, heat_border_size, parallelismLevel, heatMode == 'S');
    logableHeatModel->addRectangle (Point (0, 0), Point (4 * Casu::PELTIER_RADIUS, 0));
    logableHeatModel->setLogFileName (heat_log_file_name);
    logableHeatModel->setHeader (LogWorldHeat::WORLD_X_COORDINATE);
    logableHeatModel->openLogFile ();

    struct sigaction saFinish;
    saFinish.sa_handler = closeLogFiles;
    saFinish.sa_flags = 0;
    sigaction (SIGHUP, &saFinish, 0);
    sigaction (SIGQUIT, &saFinish, 0);
    sigaction (SIGINT, &saFinish, 0);
    sigaction (SIGTERM, &saFinish, 0);

    heatModel = logableHeatModel;
    world->addPhysicSimulation (heatModel);

    cout << "Adding handlers\n";
    CasuHandler *ch = new CasuHandler();
    world->addHandler("Casu", ch);

    PhysicalObjectHandler *ph = new PhysicalObjectHandler();
    world->addHandler("Physical", ph);

    BeeHandler *bh = new BeeHandler(bee_body_length,bee_body_width, bee_body_height,
                                    bee_body_mass, bee_max_speed);
    world->addHandler("Bee", bh);

    vector<double> target_temperatures;
    istringstream isltt (list_target_temperatures);
    while (true) {
        double n;
        isltt >> n;
        if (!isltt)
            break;
        target_temperatures.push_back (n);
    }
    cout << "Built target_temperatures vector from \"" << list_target_temperatures << "\" => [";
    for (int i = 0; i < target_temperatures.size (); i++) {
        if (i > 0) cout << ',';
        cout << " " << target_temperatures [i];
    }
    cout << " ]\n";
    vector<int> waiting_times;
    istringstream islwt (list_waiting_times);
    while (true) {
        int n;
        islwt >> n;
        if (!islwt)
            break;
        waiting_times.push_back (n);
    }
    cout << "Built waiting_times vector from \"" << list_waiting_times << "\" => [";
    for (int i = 0; i < waiting_times.size (); i++) {
        if (i > 0) cout << ',';
        cout << " " << waiting_times [i];
    }
    cout << " ]\n";
    Casu *casu = new Casu (Point (0, 0), 0.0, world, env_temp);
    world->addObject (casu);

    {
       ofstream ofs ("heat-properties.csv", ofstream::out | ofstream::trunc);
       for (int y = 0; y < logableHeatModel->size.y; y++) {
          for (int x = 0; x < logableHeatModel->size.x; x++) {
             ofs << ' ' << logableHeatModel->getProp (x, y);
          }
          ofs << '\n';
       }
       ofs.close ();
    }
    cout << "Save heat model properties\n";
    for (unsigned int i = 0; i < waiting_times.size (); i++) {
        double elapsed_time = 0;
        if (target_temperatures [i] > 0) {
            casu->peltier->setHeat (target_temperatures [i]);
            casu->peltier->setSwitchedOn (true);
            cout << "Set peltier temperature to " << target_temperatures [i] << '\n';
        }
        else {
            casu->peltier->setSwitchedOn (false);
            cout << "Turned off the peltier\n";
        }
        do {
            world->step (DELTA_TIME, PHYSICS_OVERSAMPLING);
            elapsed_time += DELTA_TIME;
        } while (elapsed_time < waiting_times [i]);
        ostringstream filename;
        filename << "heat-state_#" << (i + 1) << "_" << target_temperatures [i] << ".txt";
        logableHeatModel->saveState (filename.str ());
    }
    delete logableHeatModel;
    // delete world; causes a crash
    return 0;
}

/**
 * Signal handler to close logged files.
 */
void closeLogFiles (int signum)
{
    cout << "Received signal " << signum << "\n";
    logableHeatModel->closeLogFile ();
    exit (0);
}


// Local Variables: 
// indent-tabs-mode: nil
// End: 
