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
		enum Layer {ROBOTS, HEAT};
		/**
		 * Heat physic simulation used in the underlying world.
		 */
		WorldHeat *worldHeat;
		/**
		 * Maximum presentable heat.  Ambient heat is drawn black, higher
		 * values are drawn in shades of red while lower values are drawn in
		 * shades of blue.
		 */
		const double maxHeat;
		/**
		 * The layer to drawn upon robots. 
		 */
		Layer layerToDraw;
	public:
		AssisiPlayground (ExtendedWorld *world, WorldHeat *worldHeat, double maxHeat, QWidget *parent = 0);	
		void addEPuck(World *world, Point pos);

		~AssisiPlayground()
		{
			
		}
	
		// Inherited from viewer 
		virtual void sceneCompletedHook();
		virtual void renderObjectHook(PhysicalObject *object);
		void keyPressEvent (QKeyEvent *event);
	private:
		void drawHeatLayer ();
	};

} // namespace Enki

#endif

// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
