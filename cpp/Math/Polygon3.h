//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYGON3_H__
#define POLYGON3_H__

#include <boost/python.hpp>



#include <stdio.h>

#include <Util/Array.h>
#include <Util/gsassert.h>

#include <Math/Point3.h>
#include <Math/Polygon2.h>
#include <Math/lerp.h>
#include <Math/Plane.h>

class GS_DllExport Polygon3
{
private:
	Array<Point3> vertices;


public:
	Polygon3();
	Polygon3(const Array<Point3> &vts);
	Polygon3(boost::python::list verts);


	int size() const;
	Point3 & operator[](int i);
	const Point3 & operator[](int i) const;

	Point3 & py__getitem__(int i);
	void py__setitem__(int i, const Point3 &v);

	Point3 & firstVertex();
	const Point3 & firstVertex() const;
	Point3 & lastVertex();
	const Point3 & lastVertex() const;
	const Array<Point3> & getVertices() const;

	void addVertex(const Point3 &v);
	void reserve(int n);
	void resize(int n);
	void removeVertex(int i);
	void clear();


	void generatePolygon2(Polygon2 &p) const;
	Polygon2 py_toPolygon2() const;

	Side side(const Plane &plane, bool strict) const;

	static void sutherlandHodgmanClip(Polygon3 &out, const Polygon3 &in, const Plane &clipPlane);
	static void sutherlandHodgmanClip(Polygon3 &out, Array<int> &outIndices, const Polygon3 &in, const Array<int> &inIndices, const Plane &clipPlane);

	static Polygon3 py_sutherlandHodgmanClip(const Polygon3 &in, const Plane &clipPlane);

	Point3 computeCentroid() const;
	double computeCentroidZ() const;
};



#endif
