/*

 */

#include <iostream>

#include "AssisiPlayground.h"
#include "interactions/WorldHeat.h"

namespace Enki
{
	AssisiPlayground::AssisiPlayground (ExtendedWorld *world, WorldHeat *worldHeat, double maxHeat, QWidget *parent) :
		ViewerWidget(world, parent),
		worldHeat (worldHeat),
		maxHeat (maxHeat),
		layerToDraw (ROBOTS)
	{
		//ViewerWidget::pos = QPointF(-world->w*5, -world->h * 2);
		ViewerWidget::pitch = M_PI/2;
		ViewerWidget::altitude = 80;
	}
    
	/* virtual */ 
	void AssisiPlayground::renderObjectHook(PhysicalObject *object)
	{
		// Override the default direction arrow drawn over robots.
		if (dynamic_cast<Robot*>(object))
			{
            glColor3d(0, 0, 0);
            glBegin(GL_TRIANGLES);
            glVertex3d(0.4, 0, object->getHeight() + 0.01);
            glVertex3d(-0.4, 0.2, object->getHeight() + 0.01);
            glVertex3d(-0.4, -0.2, object->getHeight() + 0.01);
            glEnd();
			}
	}


	/* virtual */
	void AssisiPlayground::sceneCompletedHook()
	{
		switch (this->layerToDraw) {
		case HEAT:
			drawHeatLayer ();
			break;
		case ROBOTS:
			break;
		default:
			return ;
		}
		glColor3d(0,0,0);
		renderText(10, height()-50, tr("rotate camera by moving mouse while pressing ctrl+left mouse button"));
		renderText(10, height()-30, tr("move camera on x/y by moving mouse while pressing ctrl+shift+left mouse button"));
		renderText(10, height()-10, tr("move camera on z by moving mouse while pressing ctrl+shift+right mouse button"));
	}

	void  AssisiPlayground::drawHeatLayer ()
	{
		Point delta[] = {
			Point (0,                           0),
			Point (-this->worldHeat->gridScale, 0),
			Point (-this->worldHeat->gridScale, -this->worldHeat->gridScale),
			Point (0,                           -this->worldHeat->gridScale)
		};
		glPushMatrix ();
		glTranslatef (-this->world->w / 2, -this->world->h / 2, 0);
		glScalef (100, 100, 1);
		Vector pos;
		pos.x = this->worldHeat->gridScale;
		do {
			pos.y = this->worldHeat->gridScale;
			do {
				glBegin (GL_QUADS); {
					for (int i = 0; i < 4; i++) {
						Point where = pos + delta [i];
						double heat = this->worldHeat->getHeatAt (where);
						double colour = std::max (-1.0, std::min ((heat - this->worldHeat->normalHeat) / maxHeat, 1.0));
						if (colour < 0) {
							glColor3f (0, 0, -colour);
						}
						else {
							glColor3f (colour, 0, 0);
						}
						glVertex3f (where.x, where.y, 3);
					}
				} glEnd ();
				pos.y += this->worldHeat->gridScale;
			} while (pos.y < world->h);
			pos.x += this->worldHeat->gridScale;
		} while (pos.x < world->w);
		glPopMatrix ();
	}
}

using namespace Enki;

void AssisiPlayground::keyPressEvent (QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Left:
		qDebug () << "Left key pressed";
		break;
	case Qt::Key_H:
		qDebug () << "Switching heat";
		this->layerToDraw = (this->layerToDraw == HEAT ? ROBOTS : HEAT);
		updateGL ();
		break;
	default:
		QGLWidget::keyPressEvent(event);
	}
}
