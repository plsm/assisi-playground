#ifndef __FIELD_H
#define __FIELD_H

#include <PhysicalEngine.h>

struct Field
{
	double value;
	Enki::Point direction;
	Field ():
		value (0),
		direction (0, 0)
			{
			}
	void reset ()
	{
		value = 0;
		direction.x = 0;
		direction.y = 0;
	}
	Field &operator+= (const Field &other)
		{
			this->value += other.value;
			this->direction += other.direction;
			return *this;
		}
};

#endif
