//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef CGRID_CPP__
#define CGRID_CPP__

#include <algorithm>

#include <LogGrid/CGrid.h>


/*
*******************************************************************************
													CGrid
*******************************************************************************
*/

CGrid::CGrid() : Grid()
{
	Colour3f colour = Colour3f( 0.0, 0.8, 0.0 );

	colours.resize( nLevels() );
	for (int i = nLevels() - 1; i >= 0; i--)
	{
		colours[i] = colour;
		colour *= 0.5;
	}
}

CGrid::CGrid(const Ruler &rul, bool useSmallest, double multiplier)
			: Grid( rul, useSmallest, multiplier )
{
	Colour3f colour = Colour3f( 0.0, 0.8, 0.0 );

	colours.resize( nLevels() );
	for (int i = nLevels() - 1; i >= 0; i--)
	{
		colours[i] = colour;
		colour *= 0.5;
	}
}

CGrid::CGrid(const Ruler &rul, bool useSmallest, double multiplier, const Array<Colour3f> &cols)
			: Grid(rul, useSmallest, multiplier)
{
	int j = 0;

	for (int i = 0; i < nLevels(); i++)
	{
		colours.push_back( cols[j] );
		j++;
		if ( j >= (int)cols.size() )
		{
			j--;
		}
	}
}

CGrid::~CGrid()
{
}


Colour3f & CGrid::getGridColour(int gridNo)
{
	return colours[gridNo];
}

const Colour3f & CGrid::getGridColour(int gridNo) const
{
	return colours[gridNo];
}



const Array<Colour3f> & CGrid::getColours() const
{
	return colours;
}

void CGrid::setColours(const Array<Colour3f> &c)
{
	int n = std::min( colours.size(), c.size() );
	for (int i = 0; i < n; i++)
	{
		colours[i] = c[i];
	}
}


#endif
