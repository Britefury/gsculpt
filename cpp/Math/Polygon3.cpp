//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYGON3_CPP__
#define POLYGON3_CPP__

#include <Util/gsassert.h>

#include <Math/Polygon3.h>





Polygon3::Polygon3()
{
}

Polygon3::Polygon3(const Array<Point3> &vts)
				: vertices( vts )
{
}

Polygon3::Polygon3(boost::python::list verts)
{
	boost::python::object lenObj = verts.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numVerts = lenExtract;
		vertices.reserve( numVerts );
		for (int i = 0; i < numVerts; i++)
		{
			boost::python::object vertObj = verts[i];
			boost::python::extract<Point3&> pointExtract( vertObj );
			if ( pointExtract.check() )
			{
				vertices.push_back( pointExtract );
			}
		}
	}
}



int Polygon3::size() const
{
	return vertices.size();
}

Point3 & Polygon3::operator[](int i)
{
	return vertices[i];
}

const Point3 & Polygon3::operator[](int i) const
{
	return vertices[i];
}


Point3 & Polygon3::py__getitem__(int i)
{
	gs_assert( i >= -size()  &&  i < size(), "Polygon3::py__getitem__(): index out of range\n" );
	if ( i < 0 )
	{
		i += size();
	}
	return vertices[i];
}

void Polygon3::py__setitem__(int i, const Point3 &v)
{
	gs_assert( i >= -size()  &&  i < size(), "Polygon3::py__setitem__(): index out of range\n" );
	if ( i < 0 )
	{
		i += size();
	}
	vertices[i] = v;
}


Point3 & Polygon3::firstVertex()
{
	return vertices.front();
}

const Point3 & Polygon3::firstVertex() const
{
	return vertices.front();
}

Point3 & Polygon3::lastVertex()
{
	return vertices.back();
}

const Point3 & Polygon3::lastVertex() const
{
	return vertices.back();
}

const Array<Point3> & Polygon3::getVertices() const
{
	return vertices;
}


void Polygon3::addVertex(const Point3 &v)
{
	vertices.push_back( v );
}

void Polygon3::reserve(int n)
{
	vertices.reserve( n );
}

void Polygon3::resize(int n)
{
	vertices.resize( n );
}

void Polygon3::removeVertex(int i)
{
	vertices.remove( i );
}

void Polygon3::clear()
{
	vertices.clear();
}



void Polygon3::generatePolygon2(Polygon2 &p) const
{
	p.resize( size() );

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		p[vertexI] = vertices[vertexI].toPoint2();
	}
}

Polygon2 Polygon3::py_toPolygon2() const
{
	Polygon2 p;
	generatePolygon2( p );
	return p;
}


Side Polygon3::side(const Plane &plane, bool strict) const
{
	Side s = plane.side( vertices[0], strict );

	for (int vertexI = 1; vertexI < vertices.size(); vertexI++)
	{
		Side s2 = plane.side( vertices[vertexI], strict );

		if ( s2 != s )
		{
			return SIDE_BOTH;
		}
	}

	return s;
}


void Polygon3::sutherlandHodgmanClip(Polygon3 &out, const Polygon3 &in, const Plane &clipPlane)
{
	out.clear();

	if ( in.size() > 0 )
	{
		out.reserve( in.size() + 1 );

		const Point3 *prevVertex, *curVertex;
		bool prevFlag, curFlag;


		prevVertex = &in.lastVertex();
		prevFlag = *prevVertex > clipPlane;
		for (int i = 0; i < in.size(); i++)
		{
			curVertex = &in[i];
			curFlag = *curVertex > clipPlane;

			if ( prevFlag != curFlag )
			{
				double t;
				clipPlane.intersect( *prevVertex, *curVertex - *prevVertex, t );
				out.addVertex( lerp( *prevVertex, *curVertex, t ) );
			}

			if ( curFlag )
			{
				out.addVertex( *curVertex );
			}

			prevVertex = curVertex;
			prevFlag = curFlag;
		}
	}
}

void Polygon3::sutherlandHodgmanClip(Polygon3 &out, Array<int> &outIndices, const Polygon3 &in, const Array<int> &inIndices, const Plane &clipPlane)
{
	out.clear();
	outIndices.clear();

	gs_assert( in.size() == inIndices.size(), "Polygon3::sutherlandHodgmanClip(): @in and @inIndices differ in size\n" );

	if ( in.size() > 0 )
	{
		out.reserve( in.size() + 1 );

		const Point3 *prevVertex, *curVertex;
		int prevIndex, curIndex;
		bool prevFlag, curFlag;


		prevVertex = &in.lastVertex();
		prevIndex = inIndices.back();
		prevFlag = *prevVertex > clipPlane;
		for (int i = 0; i < in.size(); i++)
		{
			curVertex = &in[i];
			curIndex = inIndices[i];
			curFlag = *curVertex > clipPlane;

			if ( prevFlag != curFlag )
			{
				double t;
				clipPlane.intersect( *prevVertex, *curVertex - *prevVertex, t );
				out.addVertex( lerp( *prevVertex, *curVertex, t ) );
				outIndices.push_back( -1 );
			}

			if ( curFlag )
			{
				out.addVertex( *curVertex );
				outIndices.push_back( curIndex );
			}

			prevVertex = curVertex;
			prevIndex = curIndex;
			prevFlag = curFlag;
		}
	}
}


Polygon3 Polygon3::py_sutherlandHodgmanClip(const Polygon3 &in, const Plane &clipPlane)
{
	Polygon3 out;
	sutherlandHodgmanClip( out, in, clipPlane );
	return out;
}


Point3 Polygon3::computeCentroid() const
{
	Point3 sum;

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		sum.cumulativeAdd( vertices[vertexI] );
	}

	return Point3::mul( sum, 1.0 / (double)vertices.size() );
}

double Polygon3::computeCentroidZ() const
{
	double sum = 0.0;

	for (int vertexI = 0; vertexI < vertices.size(); vertexI++)
	{
		sum += vertices[vertexI].z;
	}

	return sum  /  (double)vertices.size();
}



#endif
