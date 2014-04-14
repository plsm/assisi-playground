/*! \file   playground_ext.h
    \brief  A simple enki playground with an external interface.

 */

#ifndef ASSISI_PLAYGROUND_H
#define ASSISI_PLAYGROUND_H

// Enki includes
#include <viewer/Viewer.h>
#include <PhysicalEngine.h>
#include <robots/e-puck/EPuck.h>

// Qt includes
//#include <QApplication>
#include <QtGui>

#include "ExtendedWorld.h"
#include "interactions/WorldHeat.h"
#include "interactions/WorldLight.h"
#include "interactions/WorldElectricField.h"


/*!	\file AssisiPlayground.h
	\brief The ASSISI Simulator.
*/

namespace Enki
{
	class AssisiPlayground : public ViewerWidget
	{

		Q_OBJECT;
		/**
		 * Data layers that can be drawn in the window.
		 */
		enum Layer {ROBOTS, HEAT, ELECTRIC_FIELD, VIBRATION, LIGHT};
		/**
		 * Heat physic simulation used in the underlying world.
		 */
		const WorldHeat *worldHeat;
		/**
		 * Maximum presentable heat.  Ambient heat is drawn black, higher
		 * values are drawn in shades of red while lower values are drawn in
		 * shades of blue.
		 */
		const double maxHeat;
		const WorldElectricField *worldElectricField;
		const double maxElectricField;

		const WorldLight *worldLight;
		double maxLightIntensity;
		/**
		 * The layer to drawn upon robots. 
		 */
		Layer layerToDraw;

		const double deltaTime;

		const unsigned int simulationSpeed;
		/**
		 * Transparency of the layer drawn above robots.
		 */
		double transparency;
		bool useGradient;
		/**
		 * z coordinate of the layer.
		 */
		const double zLayer;
	public:
		AssisiPlayground
		(ExtendedWorld *world, WorldHeat *worldHeat, double maxHeat,
		 WorldElectricField *worldElectricField, double maxElectricField,
		 WorldLight *worldLight, double maxLightIntensity,
		 double deltaTime,
		 unsigned int simulationSpeed, QWidget *parent = 0);	
		void addEPuck(World *world, Point pos);

		~AssisiPlayground()
		{
		}
	
		// Inherited from viewer 
		virtual void sceneCompletedHook();
		virtual void renderObjectHook(PhysicalObject *object);

		void keyPressEvent (QKeyEvent *event);
		void timerEvent (QTimerEvent * event);

		void initializeGL ();

	private:
		void drawHeatLayer_Gradient ();
		void drawHeatLayer_Chequerboard ();
		void drawElectricFieldLayer ();
		void drawLightLayer_Gradient ();
		void drawLightLayer_Chequerboard ();
		void drawVibrationLayer ();
	};

} // namespace Enki

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
