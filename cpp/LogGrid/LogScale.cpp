//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef LOGSCALE_CPP__
#define LOGSCALE_CPP__

#include <algorithm>

#include <LogGrid/LogScale.h>



LogScale::Level::Level()
{
	ls = NULL;
}

LogScale::Level::Level(const LogScale *l, int maj, int min)
{
	ls = l;
	major = maj;
	minor = min;
}


LogScale::Level &LogScale::Level::operator++()
{
	minor++;
	if ( minor >= (int)ls->powers.size() )
	{
		minor = 0;
		major++;
	}

	return *this;
}

LogScale::Level LogScale::Level::operator++(int)
{
	Level temp;
	++(*this);
	return temp;
}

LogScale::Level &LogScale::Level::operator+=(int i)
{
	minor += i;
	while ( minor >= (int)ls->powers.size() )
	{
		minor -= ls->powers.size();
		major++;
	}

	return *this;
}

LogScale::Level LogScale::Level::operator+(int i) const
{
	Level l = *this;
	l += i;
	return l;
}



LogScale::Level &LogScale::Level::operator--()
{
	minor--;
	if (minor < 0)
	{
		minor = ls->powers.size() - 1;
		major--;
	}

	return *this;
}

LogScale::Level LogScale::Level::operator--(int)
{
	Level temp;
	--(*this);
	return temp;
}

LogScale::Level &LogScale::Level::operator-=(int i)
{
	minor -= i;
	while (minor < 0)
	{
		minor += ls->powers.size();
		major--;
	}

	return *this;
}

LogScale::Level LogScale::Level::operator-(int i) const
{
	Level l = *this;
	l -= i;
	return l;
}


double LogScale::Level::getValue() const
{
	return pow( ls->base, (double)major + ls->powers[minor] );
}

double LogScale::Level::getPower() const
{
	return (double)major + ls->powers[minor];
}




LogScale::LogScale()
{
	base = 10.0;
	oneOverLogBase = 1.0  /  log( base );
	//powers.push_back( log(2.0)  *  oneOverLogBase );
	powers.push_back( log(5.0)  *  oneOverLogBase );
	powers.push_back( 1.0 );
}

LogScale::LogScale(double bs, const Array<double> &pwr)
{
	base = bs;
	powers = pwr;
}

LogScale::~LogScale()
{
}



LogScale::Level LogScale::getSmaller(double v) const
{
	return find( log( v )  *  oneOverLogBase );
}

LogScale::Level LogScale::getNearest(double v) const
{
	double p = log( v )  *  oneOverLogBase;
	Level level = find( p );
	Level nextLevel = level;
	++nextLevel;

	if ( ( nextLevel.getPower() - p )  <  ( p - level.getPower() ) )
	{
		return nextLevel;
	}
	else
	{
		return level;
	}
}

LogScale::Level LogScale::getLarger(double v) const
{
	Level level = find( log( v )  *  oneOverLogBase );
	++level;
	return level;
}





LogScale::Level LogScale::find(double pwr) const
{
	double pwrRoundedDown = 0.0;
	double fracPower = modf( pwr, &pwrRoundedDown );
	int gridLevel = (int)pwrRoundedDown;


	// Get an iterator @iter pointing to the item in @powers that is the first item that has a value greater and or equal to @fracPower
	const double *iter = std::lower_bound( powers.begin(), powers.end(), fracPower );

	// Convert to an int, and subtract 1, to get the next grid sub-level down
	int gridSubLevel = ( iter - powers.begin() )  -  1;

	// Handle the case where gridSubLevel is -1
	if ( gridSubLevel < 0 )
	{
		// Use the last grid sub-level
		gridSubLevel = powers.size() - 1;
		// Go down a whole grid level
		gridLevel--;
	}

	return Level( this, gridLevel, gridSubLevel );
}


#endif
