//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GRID_CPP__
#define GRID_CPP__

#include <algorithm>

#include <LogGrid/Grid.h>




Grid::Grid()
{
	useSmallestFlag = true;
	mul = 0.5;
	setCorners( Point3( 0.0, 0.0, 0.0 ), Point3( 1.0, 1.0, 0.0 ) );
}

Grid::Grid(const Ruler &rl, bool useSmallest, double multiplier)
{
	r = rl;
	useSmallestFlag = useSmallest;
	mul = multiplier;
	setCorners( Point3( 0.0, 0.0, 0.0 ), Point3( 1.0, 1.0, 0.0 ) );
}

Grid::~Grid()
{
}



void Grid::setCorners(const Point3 &p, const Point3 &q)
{
	double w, h, size;

	p1.x = std::min( p.x, q.x );
	p1.y = std::min( p.y, q.y );
	p2.x = std::max( p.x, q.x );
	p2.y = std::max( p.y, q.y );


	w = p2.x - p1.x;
	h = p2.y - p1.y;

	if ( useSmallestFlag )
	{
		size = std::min( w, h );
	}
	else
	{
		size = std::max( w, h );
	}

	size *= mul;

	r.setLength( size );
}


void Grid::generateHLines(int level, LineList &lines) const
{
	Array<double> ticks;

	ticks = r.generateTicks( p1.x, p2.x, level );
	for (int i = 0; i < (int)ticks.size(); i++)
	{
		GridLine line;

		line.line = Segment3( Point3( ticks[i], p1.y, 0.0 ), Point3( ticks[i], p2.y, 0.0 ) );
		line.labelValue = ticks[i];

		lines.push_back( line );
	}
}

void Grid::generateVLines(int level, LineList &lines) const
{
	Array<double> ticks;

	ticks = r.generateTicks( p1.y, p2.y, level );
	for (int i = 0; i < (int)ticks.size(); i++)
	{
		GridLine line;

		line.line = Segment3( Point3( p1.x, ticks[i], 0.0 ), Point3( p2.x, ticks[i], 0.0 ) );
		line.labelValue = ticks[i];

		lines.push_back( line );
	}
}


int Grid::nLevels() const
{
	return r.nLevels();
}

const Ruler &Grid::getRuler() const
{
	return r;
}


#endif
