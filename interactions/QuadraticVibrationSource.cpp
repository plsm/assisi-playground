/* 
 * File:   QuadraticVibrationSource.cpp
 * Author: pedro
 * 
 * Created on 19 de Janeiro de 2014, 16:50
 */

#include "QuadraticVibrationSource.h"

using namespace Enki;

QuadraticVibrationSource::QuadraticVibrationSource
	(double range, Robot* owner,
	 Vector relativePosition, double orientation,
	 double a)
	:
	VibrationSource (range, owner, relativePosition, orientation),
	a (a),
	amplitude (0)
{
}

QuadraticVibrationSource::QuadraticVibrationSource (const QuadraticVibrationSource& orig):
	VibrationSource (orig),
	a (orig.a),
	amplitude (0)
{
}

QuadraticVibrationSource::~QuadraticVibrationSource()
{
}

double QuadraticVibrationSource::getAmplitude (const Point &position, double time) const
{
	double squareDistance = (this->absolutePosition - position).norm2 ();
	return this->amplitude / (this->a * squareDistance + 1);
}
