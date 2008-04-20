//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef INDEXTRIANGLE_CPP__
#define INDEXTRIANGLE_CPP__

#include <Math/IndexTriangle.h>



Tesselation::Tesselation()
{
}

Tesselation::~Tesselation()
{
}



void Tesselation::read(FILE *f)
{
	int numTriangles;
	fread( &numTriangles, sizeof(int), 1, f );
	tris.reserve( numTriangles );

	for (int i = 0; i < numTriangles; i++)
	{
		IndexTriangle &tri = tris.push_back();
		tri.read( f );
	}
}

void Tesselation::write(FILE *f) const
{
	int numTriangles = tris.size();
	fwrite( &numTriangles, sizeof(int), 1, f );

	for (int i = 0; i < tris.size(); i++)
	{
		tris[i].write( f );
	}
}


void Tesselation::read(SENode &node)
{
	std::string header;
	node[0]  >>  header;
	if ( header == "Tesselation" )
	{
		int numTriangles = 0;
		node[1]  >>  numTriangles;
		tris.reserve( numTriangles );

		for (int i = 0; i < numTriangles; i++)
		{
			IndexTriangle &tri = tris.push_back();
			tri.read( node[i+2] );
		}
	}
}

void Tesselation::write(SENode &node) const
{
	node.addItem()  <<  "Tesselation";

	node.addItem()  <<  tris.size();

	for (int i = 0; i < tris.size(); i++)
	{
		tris[i].write( node.addList() );
	}
}


void Tesselation::reserve(int s)
{
	tris.reserve( s );
}

void Tesselation::add(const IndexTriangle &t)
{
	tris.push_back( t );
}

const IndexTriangle & Tesselation::operator[](int i) const
{
	return tris[i];
}

const IndexTriangle & Tesselation::at(int i) const
{
	return tris[i];
}

int Tesselation::size() const
{
	return tris.size();
}

void Tesselation::clear()
{
	tris.clear();
}


void Tesselation::optimiseMemoryUsageLean()
{
	tris.optimiseMemoryUsageLean();
}

void Tesselation::optimiseMemoryUsageFor(int s)
{
	tris.optimiseMemoryUsageFor( s );
}



#endif
