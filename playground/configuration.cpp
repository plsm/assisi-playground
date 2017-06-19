#include <QApplication>
#include <QImage>
#include <QGLWidget>

#include <fstream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

// MAC workaround for Thomas
#if defined __APPLE__
#include <mach-o/dyld.h>
#include <sys/param.h>
#endif

#include "extensions/ExtendedWorld.h"
#include "interactions/WorldHeat.h"

#include "robots/Bee.h"
#include "robots/Casu.h"
#include "handlers/PhysicalObjectHandler.h"
#include "handlers/CasuHandler.h"
#include "handlers/BeeHandler.h"

#include "configuration.h"

using namespace std;
using namespace Enki;

namespace po = boost::program_options;
namespace fs = boost::filesystem;

// Variables to store the command line arguments
namespace Enki {

    static string config_file_name ("Playground.cfg");

    static string pub_address ("tcp://*:5555"); 

    static string sub_address ("tcp://*:5556");

    static int arena_radius = 20;

    static string heat_state_filename;

    double env_temp = 28;

    static double heat_scale = 2;

    static int heat_border_size;

    static string heat_log_file_name;

    static double maxVibration;

    static char layer;

    Mode mode;

    double timerPeriod = 0.01;

    static double parallelismLevel = 1.0;

    static double bee_body_length;

    static double bee_body_width;

    static double bee_body_height;

    static double bee_body_mass;

    static double bee_max_speed;

    static double cameraPosX = 0;

    static double cameraPosY = 0;

    static double cameraAltitude = 1;

    static double cameraYaw = 0;

    static double cameraPitch = M_PI / 2;

    static unsigned int skewMonitorRate = 60;

    static double skewReportThreshold = 0.05;
}

static fs::path default_config_file_name ()
{
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
    default_config.remove_filename() /= config_file_name;
    return default_config;
}

po::options_description Enki::standardOptions ()
{
    po::options_description desc("Recognized options");
    desc.add_options ()
        (
         "help,h",
         "produce help message"
         )
        (
         "nogui",
         "run without viewer"
         )
        (
         "config_file,c", 
         po::value<string>(&config_file_name)->default_value(default_config_file_name ().native()),
         "configuration file name"
         )
        (
         "pub_addr",
         po::value<string>(&pub_address)->default_value("tcp://*:5555"),
         "Address for publishing data, in the form tcp://hostname:port"
         )
        (
         "sub_addr",
         po::value<string>(&sub_address)->default_value("tcp://*:5556"),
         "Address for subscribing to commands, in the form tcp://hostname:port"
         )
        (
         "Arena.radius,r",
         po::value<int>(&arena_radius),
         "playground radius, in cm"
         )
        (
         "Heat.state",
         po::value<string>(&heat_state_filename)->default_value (""),
         "use heat state stored in given filename"
         )
        (
         "Heat.env_temp,t",
         po::value<double>(&env_temp),
         "environment temperature, in C"
         )
        (
         "Heat.scale",
         po::value<double>(&heat_scale),
         "length of the size of a heat cell"
         )
        (
         "Heat.border_size",
         po::value<int>(&heat_border_size),
         "playground radius, in cm"
         )
        (
         "Heat.log_file",
         po::value<string> (&heat_log_file_name)->default_value (""),
         "heat log file name"
         )
        (
         "Heat.cell_dissipation",
         po::value<double> (&WorldHeat::CELL_DISSIPATION),
         "heat lost by cells directly to outside world"
         )
        (
         "Vibration.range",
         po::value<double> (&Casu::VIBRATION_SOURCE_RANGE),
         "vibration range, in cm"
         )
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
         "Peltier.thermal_response", 
         po::value<double> (&Casu::PELTIER_THERMAL_RESPONSE),
         "peltier thermal response"
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
         "Viewer.layer",
         po::value<char> (&layer),
         "which data layer should be displayed by default: N none, H heat, V vibration, A airflow"
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
         "Camera.pos_x",
         po::value<double> (&cameraPosX),
         "camera x position"
         )
        (
         "Camera.pos_y",
         po::value<double> (&cameraPosY),
         "camera y position"
         )
        (
         "Camera.altitude",
         po::value<double> (&cameraAltitude),
         "camera altitude"
         )
        (
         "Camera.yaw",
         po::value<double> (&cameraYaw),
         "camera yaw"
         )
        (
         "Camera.pitch",
         po::value<double> (&cameraPitch),
         "camera pitch"
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
        ;
    return desc;
}

po::variables_map Enki::processConfiguration (int argc, char *argv[], const po::options_description &desc)
{
    po::variables_map vm;
    po::store (po::parse_command_line (argc, argv, desc), vm);
    ifstream config_file (config_file_name.c_str (), std::ifstream::in);
    po::store(po::parse_config_file(config_file, desc), vm);
    config_file.close ();
    po::notify (vm);

    if (vm.count ("help")) {
        cout << desc << endl;
        exit (1);
    }
    if (vm.count ("nogui") == 0) {
        mode = MODE_NO_VIEWER;
    }
    else {
        mode = MODE_GUI;
    }
    return vm;
}

WorldExt *Enki::createWorld (const po::variables_map &vm)
{
    WorldExt *world;
    WorldHeat *heatModel;

    //QImage texture("playground/world.png");
    QImage texture(QString(":/textures/ground_grayscale.png"));
    texture = QGLWidget::convertToGLFormat(texture);    
    //texture.invertPixels(QImage::InvertRgba);
    
    world = new WorldExt (arena_radius, pub_address, sub_address,
        Color::gray, 
        World::GroundTexture (
            texture.width(),
            texture.height(),
            (const uint32_t*) texture.constBits ()),
        skewMonitorRate,
        skewReportThreshold);
    // create heat model
    if (heat_state_filename != "" && vm.count ("Heat.state")) {
       if (vm.count ("Heat.env_temp"))
          cout << "Discarding parameter Heat.env_temp\n";
       if (vm.count ("Heat.scale"))
          cout << "Discarding parameter Heat.scale\n";
       if (vm.count ("Heat.border_size"))
          cout << "Discarding parameter Heat.border_size\n";
       heatModel = WorldHeat::worldHeatFromFile (heat_state_filename, parallelismLevel);
    }
    else
       heatModel = new WorldHeat (world, env_temp, heat_scale, heat_border_size, parallelismLevel);
    if (heat_log_file_name != "") {
        heatModel->logToStream (heat_log_file_name);
    }
    world->addPhysicSimulation (heatModel);
    // Add handlers
    CasuHandler *ch = new CasuHandler ();
    world->addHandler ("Casu", ch);
    BeeHandler *bh = new BeeHandler(bee_body_length,bee_body_width, bee_body_height,
                                    bee_body_mass, bee_max_speed);
    world->addHandler ("Bee", bh);
    PhysicalObjectHandler *ph = new PhysicalObjectHandler ();
    world->addHandler ("Physical", ph);    

    return world;
}
