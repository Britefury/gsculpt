//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LOGMULTISCALE_CPP__
#define LOGMULTISCALE_CPP__

#include <LogGrid/LogMultiScale.h>



LogMultiScale::LogMultiScale() : LogScale()
{
	nLevs = 3;
}

LogMultiScale::LogMultiScale(double bs, const Array<double> pwr, int nLevels)
									  : LogScale(bs, pwr)
{
	nLevs = nLevels;
}


Array<LogScale::Level> LogMultiScale::generateLevels(double sz) const
{
	Array<LogScale::Level> levels;

	LogScale::Level level = getSmaller( sz );

	for (int i = 0; i < nLevs; i++)
	{
		levels.push_back( level );
		--level;
	}

	return levels;
}


int LogMultiScale::nLevels() const
{
	return nLevs;
}





#endif
