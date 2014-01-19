/* 
 * File:   QuadraticVibrationSource.h
 * Author: pedro
 *
 * Created on 19 de Janeiro de 2014, 16:50
 */

#ifndef QUADRATICVIBRATIONSOURCE_H
#define	QUADRATICVIBRATIONSOURCE_H

#include <enki/Geometry.h>

#include "VibrationSource.h"

namespace Enki
{
	/**
	 * A vibration source that approximates the wave equation.  Vibration
	 * amplitude decays quadratic with distance.  
	 */
	class QuadraticVibrationSource:
		public VibrationSource
	{
	public:
		/**
		 * Quadratic coefficient of the vibration model.
		 */
		const double a;
		/**
		 * Current vibration amplitude of this source.
		 */
		double amplitude;
	public:

		QuadraticVibrationSource (double range, Robot* owner, Vector relativePosition, double orientation, double a);

		QuadraticVibrationSource (const QuadraticVibrationSource& orig);

		virtual ~QuadraticVibrationSource ();

		virtual double getAmplitude (const Point &position, double time) const;
	private:

	};
}

#endif	/* QUADRATICVIBRATIONSOURCE_H */


// Local Variables: 
// mode: c++
// mode: flyspell-prog
// ispell-local-dictionary: "british"
// End: 
