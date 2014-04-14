/*

 */

#include <iostream>
#include <algorithm>

#include "AssisiPlayground.h"
#include "interactions/WorldHeat.h"
#include "interactions/AbstractElectricFieldActuator.h"

namespace Enki
{
	AssisiPlayground::AssisiPlayground
	(ExtendedWorld *world,
	 WorldHeat *worldHeat,
	 double maxHeat,
	 WorldElectricField *worldElectricField,
	 double maxElectricField,
	 WorldLight *worldLight, double maxLightIntensity,
	 double deltaTime,
	 unsigned int simulationSpeed,
	 QWidget *parent) :
		ViewerWidget(world, parent),
		worldHeat (worldHeat),
		maxHeat (maxHeat),
		worldElectricField (worldElectricField),
		maxElectricField (maxElectricField),
		worldLight (worldLight),
		maxLightIntensity (maxLightIntensity),
		layerToDraw (ROBOTS),
		deltaTime (deltaTime),
		simulationSpeed (simulationSpeed),
		zLayer (11),
		transparency (0.5)
	{
		//ViewerWidget::pos = QPointF(-world->w*5, -world->h * 2);
		ViewerWidget::pitch = M_PI/2;
		ViewerWidget::altitude = 80;
		this->initializeGL ();
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
		glEnable (GL_BLEND);
		glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glPushMatrix ();
		glTranslated (0, 0, this->zLayer);
		switch (this->layerToDraw) {
		case HEAT:
			if (this->useGradient)
				drawHeatLayer_Gradient ();
			else
				drawHeatLayer_Chequerboard ();
			break;
		case ELECTRIC_FIELD:
			drawElectricFieldLayer ();
			break;
		case LIGHT:
			if (this->useGradient)
				drawLightLayer_Gradient ();
			else
				drawLightLayer_Chequerboard ();
			break;
		case ROBOTS:
			break;
		default:
			break ;
		}
		glPopMatrix ();
		glDisable (GL_BLEND);
		// glColor3d(0,0,0);
		// renderText(10, height()-50, tr("rotate camera by moving mouse while pressing ctrl+left mouse button"));
		// renderText(10, height()-30, tr("move camera on x/y by moving mouse while pressing ctrl+shift+left mouse button"));
		// renderText(10, height()-10, tr("move camera on z by moving mouse while pressing ctrl+shift+right mouse button"));
	}

	void AssisiPlayground::drawHeatLayer_Gradient ()
	{
		Point delta[] = {
			Point (0,                           0),
			Point (-this->worldHeat->gridScale, 0),
			Point (-this->worldHeat->gridScale, -this->worldHeat->gridScale),
			Point (0,                           -this->worldHeat->gridScale)
		};
		Vector pos;
		pos.x = -this->world->r + this->worldHeat->gridScale;
		double radius2 = this->world->r * this->world->r;
		do {
			pos.y = -this->world->r + this->worldHeat->gridScale;
			do {
				if (pos.norm2 () < radius2) {
					glBegin (GL_QUADS); {
						for (int i = 0; i < 4; i++) {
							Point where = pos + delta [i];
							double heat = this->worldHeat->getHeatAt (where);
							double colour = std::max (-1.0, std::min ((heat - this->worldHeat->normalHeat) / maxHeat, 1.0));
							if (colour < 0) {
								glColor4f (0, 0, -colour, transparency);
							}
							else {
								glColor4f (colour, 0, 0, transparency);
							}
							glVertex2f (where.x, where.y);
						}
					} glEnd ();
				}
				pos.y += this->worldHeat->gridScale;
			} while (pos.y < world->r);
			pos.x += this->worldHeat->gridScale;
		} while (pos.x < world->r);
	}
}

using namespace Enki;

void AssisiPlayground::drawElectricFieldLayer ()
{
	Point delta[] = {
		Point ( 0,  0),
		Point (-1,  0),
		Point (-1, -1),
		Point ( 0, -1)
	};
	glPushMatrix ();
	glTranslated (-this->world->r, -this->world->r, 0);
	glScaled (this->worldElectricField->gridScale, this->worldElectricField->gridScale, 1);
	Vector pos;
	pos.x = 1;
	double radius2 = this->worldElectricField->size.x * this->worldElectricField->size.x / 4;
	do {
		pos.y = 1;
		do {
			if ((pos - this->worldElectricField->size / 2).norm2 () < radius2) {
				glBegin (GL_QUADS); {
					for (int i = 0; i < 4; i++) {
						Point where = pos + delta [i];
						double value = this->worldElectricField->getElectricFieldAt (where.x, where.y).value;
						double colour = std::max (-1.0, std::min (value / this->maxElectricField, 1.0));
						if (colour < 0) {
							glColor4f (0, 0, -colour, transparency);
						}
						else {
							glColor4f (colour, 0, 0, transparency);
						}
						glVertex2f (where.x, where.y);
					}
				} glEnd ();
			}
			pos.y++;
		} while (pos.y < this->worldElectricField->size.y);
		pos.x++;
	} while (pos.x < this->worldElectricField->size.x);
	glPopMatrix ();
}

void AssisiPlayground::drawLightLayer_Gradient ()
{
	Point delta[] = {
		Point (-1, -1),
		Point ( 0, -1),
		Point ( 0,  0),
		Point (-1,  0)
	};
	glPushMatrix ();
	glTranslated (this->worldLight->origin.x, this->worldLight->origin.y, 0);
	glScaled (this->worldLight->gridScale, this->worldLight->gridScale, 1);
	Vector pos;
	pos.x = 1;
	double radius2 = this->worldLight->size.x * this->worldLight->size.x / 4;
	do {
		pos.y = 1;
		do {
			if ((pos - this->worldLight->size / 2).norm2 () < radius2) {
				glBegin (GL_QUADS); {
					for (int idx = 0; idx < 4; idx++) {
						Point where = pos + delta [idx];
						double value = this->worldLight->getIntensityAt (where.x, where.y);
						double colour = std::min (value / this->maxLightIntensity, 1.0);
						glColor4f (colour, colour, colour, transparency);
						glVertex2f (where.x, where.y);
					}
				} glEnd ();
			}
			pos.y += 1;
			} while (pos.y < this->worldLight->size.y);
		pos.x += 1;
	} while (pos.x < this->worldLight->size.x);
	glPopMatrix ();
}

void AssisiPlayground::drawLightLayer_Chequerboard ()
{
	Point delta[] = {
		Point (-0.5, -0.5),
		Point ( 0.5, -0.5),
		Point ( 0.5,  0.5),
		Point (-0.5,  0.5)
	};
	glPushMatrix ();
	glTranslated (this->worldLight->origin.x, this->worldLight->origin.y, 0);
	glScaled (this->worldLight->gridScale, this->worldLight->gridScale, 1);
	Vector pos;
	pos.x = 0;
	double radius2 = this->worldLight->size.x * this->worldLight->size.x / 4;
	do {
		pos.y = 0;
		do {
			if ((pos - this->worldLight->size / 2).norm2 () < radius2) {
				double value = this->worldLight->getIntensityAt (pos.x, pos.y);
				double colour = std::min (value / this->maxLightIntensity, 1.0);
				glColor4f (colour, colour, colour, transparency);
				glBegin (GL_QUADS); {
					for (int idx = 0; idx < 4; idx++) {
						Point where = pos + delta [idx];
						glVertex2f (where.x, where.y);
					}
				} glEnd ();
			}
			pos.y += 1;
			} while (pos.y < this->worldLight->size.y);
		pos.x += 1;
	} while (pos.x < this->worldLight->size.x);
	glPopMatrix ();
}


void AssisiPlayground::drawHeatLayer_Chequerboard ()
{
	Point delta[] = {
		Point ( this->worldHeat->gridScale / 2,  this->worldHeat->gridScale / 2),
		Point (-this->worldHeat->gridScale / 2,  this->worldHeat->gridScale / 2),
		Point (-this->worldHeat->gridScale / 2, -this->worldHeat->gridScale / 2),
		Point ( this->worldHeat->gridScale / 2, -this->worldHeat->gridScale / 2)
	};
	Vector pos;
	pos.x = -this->world->r + this->worldHeat->gridScale;
	double radius2 = this->world->r * this->world->r;
	do {
		pos.y = -this->world->r;
		do {
			if (pos.norm2 () < radius2) {
				double heat = this->worldHeat->getHeatAt (pos);
				double colour = std::max (-1.0, std::min ((heat - this->worldHeat->normalHeat) / maxHeat, 1.0));
				if (colour < 0) {
					glColor4f (0, 0, -colour, transparency);
				}
				else {
					glColor4f (colour, 0, 0, transparency);
				}
				glBegin (GL_QUADS); {
					for (int i = 0; i < 4; i++) {
						Point where = pos + delta [i];
						glVertex2f (where.x, where.y);
					}
				} glEnd ();
			}
			pos.y += this->worldHeat->gridScale;
		} while (pos.y < world->r - this->worldHeat->gridScale);
		pos.x += this->worldHeat->gridScale;
	} while (pos.x < world->r - this->worldHeat->gridScale);
}



void AssisiPlayground::drawVibrationLayer ()
{
}

void AssisiPlayground::keyPressEvent (QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Plus:
		transparency = std::min (1.0, transparency + 0.1);
		qDebug () << "Transparency: " << transparency;
		updateGL ();
		break;
	case Qt::Key_Minus:
		transparency = std::max (0.1, transparency - 0.1);
		qDebug () << "Transparency: " << transparency;
		updateGL ();
		break;
	case Qt::Key_G:
		this->useGradient = (this->useGradient ? false : true);
		qDebug () << "Use gradient: " << this->useGradient;
		updateGL ();
		break;
	case Qt::Key_H:
		qDebug () << "Switching heat";
		this->layerToDraw = (this->layerToDraw == HEAT ? ROBOTS : HEAT);
		updateGL ();
		break;
	case Qt::Key_E:
		qDebug () << "Switching electric";
		this->layerToDraw = (this->layerToDraw == ELECTRIC_FIELD ? ROBOTS : ELECTRIC_FIELD);
		updateGL ();
		break;
	case Qt::Key_L:
		qDebug () << "Switching light";
		this->layerToDraw = (this->layerToDraw == LIGHT ? ROBOTS : LIGHT);
		updateGL ();
		break;
	default:
		QGLWidget::keyPressEvent(event);
	}
}

void AssisiPlayground::
timerEvent (QTimerEvent * event)
{
	// std::cout << "ping\n";
	((ExtendedWorld *)world)->step (deltaTime, 3);
	updateGL ();
}

void AssisiPlayground::
initializeGL ()
{
#if 0
	glClearColor(world->wallsColor.r(), world->wallsColor.g(), world->wallsColor.b(), 1.0);
		
	float LightAmbient[] = {0.6, 0.6, 0.6, 1};
	float LightDiffuse[] = {1.2, 1.2, 1.2, 1};
	float defaultColor[] = {0.5, 0.5, 0.5, 1};
	glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, LightDiffuse);
	glEnable(GL_LIGHT0);
		
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, defaultColor);
		
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
		
	glShadeModel(GL_SMOOTH);
	glEnable(GL_LIGHTING);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
		
	glPolygonOffset(-1.01f, 0.f);
		
	worldTexture = bindTexture(QPixmap(QString(":/textures/world.png")), GL_TEXTURE_2D, GL_LUMINANCE8);
	wallTexture = bindTexture(QPixmap(QString(":/textures/wall.png")), GL_TEXTURE_2D, GL_LUMINANCE8);
	if (world->hasGroundTexture()) {
		glGenTextures(1, &worldGroundTexture);
		glBindTexture(GL_TEXTURE_2D, worldGroundTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, world->groundTextureWidth, world->groundTextureHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &world->groundTextureData[0]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	}
	worldList = glGenLists(1);
	renderWorld();
		
		
	// let subclass manage their static types
	renderObjectsTypesHook();
#endif

	ViewerWidget::initializeGL ();

	//	startTimer (this->simulationSpeed);
}
