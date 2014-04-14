#include <fstream>
#include <QApplication>
#include <QImage>

#include "WorldExt.h"
#include "AssisiPlayground.h"
#include "robots/Casu.h"

#include "handlers/PhysicalObjectHandler.h"
#include "handlers/EPuckHandler.h"
#include "handlers/CasuHandler.h"
#include "handlers/BeeHandler.h"

#include "interactions/WorldLight.h"


#include <iostream>

using namespace std;
using namespace Enki;

static double radius;
static double normalHeat, maxHeat, borderSize;
static double maxElectricField;
static double waveLength;
static double gridScale;
static double deltaTime = 1000;
static unsigned int simulationSpeed = 100;

static double currentTime = 0;

static WorldExt *world;
static WorldHeat *worldHeat;
static WorldElectricField* worldElectricField;
static WorldLight* worldLight;

static void initWorld ()
{
	QImage texture (QString (":/textures/ground_grayscale.png"));
	texture = texture.convertToFormat (QImage::Format_ARGB32);
	texture.invertPixels (QImage::InvertRgba);
	string pub_address ("tcp://*:5555"); 
	string sub_address ("tcp://*:5556");
	world = new WorldExt
		(radius,
		 pub_address,
		 sub_address,
		 Color::gray,
		 texture.width (),
		 texture.height (),
		 (uint32_t*) texture.bits ()
		 );
	worldHeat = new WorldHeat (normalHeat, gridScale, borderSize);
	world->addPhysicSimulation (worldHeat);
	worldElectricField = new WorldElectricField (gridScale);
	// world->addPhysicSimulation (worldElectricField);
	worldLight = new WorldLight (gridScale, waveLength);
	// world->addPhysicSimulation (worldLight);
}

static void readConfigFile ()
{
	ifstream ifs ("config.txt");
	ifs >> radius;
	ifs >> normalHeat >> maxHeat >> borderSize;
	ifs >> maxElectricField;
	ifs >> waveLength;
	ifs >> gridScale;


	int dummy;
	// std::cout << "Read physics parameters\n";
	// std::cin >> dummy;

	initWorld ();

	// std::cout << "Initialised world\n";
	// std::cin >> dummy;

	int numberCASUs;
	ifs >> numberCASUs;
	for (int i = 0; i < numberCASUs; i++) {
		Point point;
		ifs >> point.x >> point.y;

		Casu *casu = new Casu (world, &point);
		ifs
			>> casu->lightSource->intensity
			>> casu->vibrationSource->amplitude
			>> casu->vibrationSource->frequency
			>> casu->heatActuator->heat
			>> casu->electricFieldActuator->electricCharge;
		world->addObject (casu);
		std::cerr << "Read casu #" << i << std::endl;
	}


	ifs >> deltaTime >> simulationSpeed;
	ifs.close ();
}

static void initHandlers ()
{
	EPuckHandler *eh = new EPuckHandler();
	world->addHandler("EPuck", eh);

	CasuHandler *ch = new CasuHandler();
	world->addHandler("Casu", ch);

	PhysicalObjectHandler *ph = new PhysicalObjectHandler();
	world->addHandler("Physical", ph);

	BeeHandler *bh = new BeeHandler();
	world->addHandler("Bee", bh);
}

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	readConfigFile ();
	world->step (deltaTime);
	initHandlers ();
	AssisiPlayground viewer
		(world, worldHeat, maxHeat,
		 worldElectricField, maxElectricField,
		 worldLight, waveLength,
		 deltaTime, simulationSpeed);
	viewer.show ();
	
	return app.exec();
	
	// // Create the world and the viewer
   //  double r = 40; // World radius (in cm?)
   //  //string pub_address("tcp://127.0.0.1:5555"); 
   //  //string sub_address("tcp://127.0.0.1:5556");
   //  string pub_address("tcp://*:5555"); 
   //  string sub_address("tcp://*:5556");

   //  //QImage texture("playground/world.png");
   //  QImage texture(QString(":/textures/ground_grayscale.png"));
   //  texture = texture.convertToFormat(QImage::Format_ARGB32);    texture.invertPixels(QImage::InvertRgba);
   //  WorldExt world(r, pub_address, sub_address,
   //                 Color::gray, texture.width(),
   //                 texture.height(), (uint32_t*) texture.bits() );

   //  // Add handlers
   //  EPuckHandler *eh = new EPuckHandler();
   //  world.addHandler("EPuck", eh);

   //  CasuHandler *ch = new CasuHandler();
   //  world.addHandler("Casu", ch);

   //  PhysicalObjectHandler *ph = new PhysicalObjectHandler();
   //  world.addHandler("Physical", ph);

   //  BeeHandler *bh = new BeeHandler();
   //  world.addHandler("Bee", bh);

   //  AssisiPlayground viewer(&world);	
	// viewer.show();
	
	// return app.exec();
    
}

