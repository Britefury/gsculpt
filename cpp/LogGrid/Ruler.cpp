//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef RULER_CPP__
#define RULER_CPP__

#include <LogGrid/Ruler.h>




Ruler::Ruler()
{
}

Ruler::Ruler(const LogMultiScale &s)
{
	sc = s;
}

Ruler::~Ruler()
{
}


void Ruler::setLength(double l)
{
	levels = sc.generateLevels(l);
}


Array<double> Ruler::generateTicks(double lower, double upper, int level) const
{
	int l, u;
	Array<double> result;

	l = (int)( lower  /  levels[level].getValue() );
	u = (int)( upper  /  levels[level].getValue() );

	for (int i = l; i <= u; i++)
	{
		result.push_back( (double)i * levels[level].getValue() );
	}

	return result;
}


int Ruler::nLevels() const
{
	return sc.nLevels();
}

const LogMultiScale & Ruler::getScale() const
{
	return sc;
}

const Array<LogMultiScale::Level> & Ruler::getLevels() const
{
	return levels;
}



#endif
