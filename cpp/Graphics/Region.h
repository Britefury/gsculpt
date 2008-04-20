//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef REGION_H__
#define REGION_H__

#include <boost/python.hpp>





#include <Util/Array.h>

#include <Math/Point2.h>
#include <Math/Segment2.h>
#include <Math/BBox2.h>
#include <Math/Circle2.h>
#include <Math/Triangle3.h>
#include <Math/Matrix4.h>
#include <Math/BBox3.h>
#include <Math/Polygon2.h>
#include <Math/Polygon3.h>
#include <Math/Plane.h>
#include <Math/Projection.h>

#include <RTType/RTType.h>

/***********************************************************************************
						Region2d
 ***********************************************************************************/

class GS_DllExport Region2d : public RTObject
{
RT_TYPE;
public:
	Region2d();
	Region2d(const Region2d &b);
	virtual ~Region2d();


	virtual bool isEmpty() const;

	virtual bool contains(const Point2 &p) const;
	virtual bool containsPartOf(const Segment2 &seg) const;
	virtual bool containsAllOf(const Segment2 &seg) const;
	virtual bool containsPartOf(const Polygon2 &poly) const;
	virtual bool containsAllOf(const Polygon2 &poly) const;

	virtual void generatePoints(Array<Point2> &points) const;
};




/***********************************************************************************
						RectangleRegion2d
 ***********************************************************************************/

class GS_DllExport RectangleRegion2d : public Region2d
{
RT_TYPE;
public:
	BBox2 box;

public:
	RectangleRegion2d();
	RectangleRegion2d(const RectangleRegion2d &b);
	RectangleRegion2d(const BBox2 &b);
	RectangleRegion2d(const Point2 &corner0, const Point2 &corner1);
	virtual ~RectangleRegion2d();


	virtual bool isEmpty() const;

	virtual bool contains(const Point2 &p) const;
	virtual bool containsPartOf(const Segment2 &seg) const;
	virtual bool containsAllOf(const Segment2 &seg) const;
	virtual bool containsPartOf(const Polygon2 &poly) const;
	virtual bool containsAllOf(const Polygon2 &poly) const;

	virtual void generatePoints(Array<Point2> &points) const;
};




/***********************************************************************************
						CircleRegion2d
 ***********************************************************************************/

class GS_DllExport CircleRegion2d : public Region2d
{
RT_TYPE;
public:
	Circle2 circle;

public:
	CircleRegion2d();
	CircleRegion2d(const CircleRegion2d &b);
	CircleRegion2d(const Circle2 &circle);
	virtual ~CircleRegion2d();


	virtual bool isEmpty() const;

	virtual bool contains(const Point2 &p) const;
	virtual bool containsPartOf(const Segment2 &seg) const;
	virtual bool containsAllOf(const Segment2 &seg) const;
	virtual bool containsPartOf(const Polygon2 &poly) const;
	virtual bool containsAllOf(const Polygon2 &poly) const;

	virtual void generatePoints(Array<Point2> &points) const;
};




/***********************************************************************************
						PolylineRegion2d
 ***********************************************************************************/

class GS_DllExport PolylineRegion2d : public Region2d
{
RT_TYPE;
public:
	Array<Point2> vertices;

public:
	PolylineRegion2d();
	PolylineRegion2d(const PolylineRegion2d &b);
	PolylineRegion2d(const Array<Point2> &vts);
	PolylineRegion2d(boost::python::list verts);

	virtual ~PolylineRegion2d();


	virtual bool isEmpty() const;

	virtual bool contains(const Point2 &p) const;
	virtual bool containsPartOf(const Segment2 &seg) const;
	virtual bool containsAllOf(const Segment2 &seg) const;
	virtual bool containsPartOf(const Polygon2 &poly) const;
	virtual bool containsAllOf(const Polygon2 &poly) const;

	virtual void generatePoints(Array<Point2> &points) const;


	boost::python::list py_getVertices() const;
	void py_setVertices(boost::python::list verts);
};




/***********************************************************************************
						LassoRegion2d
 ***********************************************************************************/

class GS_DllExport LassoRegion2d : public Region2d
{
RT_TYPE;
public:
	Polygon2 lasso;

public:
	LassoRegion2d();
	LassoRegion2d(const LassoRegion2d &b);
	LassoRegion2d(const Polygon2 &p);
	LassoRegion2d(const Array<Point2> &vts);
	virtual ~LassoRegion2d();


	virtual bool isEmpty() const;

	virtual bool contains(const Point2 &p) const;
	virtual bool containsPartOf(const Segment2 &seg) const;
	virtual bool containsAllOf(const Segment2 &seg) const;
	virtual bool containsPartOf(const Polygon2 &poly) const;
	virtual bool containsAllOf(const Polygon2 &poly) const;

	virtual void generatePoints(Array<Point2> &points) const;
};




/***********************************************************************************
						Region3d
 ***********************************************************************************/

class GS_DllExport Region3d : public RTObject
{
RT_TYPE;
public:
	Region3d();
	Region3d(const Region3d &b);
	virtual ~Region3d();


	virtual void transform(const Matrix4 &m, const Matrix4 &mInv);

	virtual bool isEmpty() const;

	virtual bool contains(const Point3 &p) const;
	virtual bool containsPartOf(const Segment3 &seg) const;
	virtual bool containsAllOf(const Segment3 &seg) const;
	virtual bool containsPartOf(const Polygon3 &poly) const;
	virtual bool containsAllOf(const Polygon3 &poly) const;

	virtual void paint() const;
};




/***********************************************************************************
						ProjectedRegion3d
 ***********************************************************************************/

class GS_DllExport ProjectedRegion3d : public Region3d
{
RT_TYPE;
private:
	Region2d *region2d;

public:
	Projection projection;

public:
	ProjectedRegion3d();
	ProjectedRegion3d(const ProjectedRegion3d &b);
	ProjectedRegion3d(const Region2d &r2d, const Projection &proj);
	virtual ~ProjectedRegion3d();


	virtual void transform(const Matrix4 &m, const Matrix4 &mInv);

	virtual bool isEmpty() const;

	virtual bool contains(const Point3 &p) const;
	virtual bool containsPartOf(const Segment3 &seg) const;
	virtual bool containsAllOf(const Segment3 &seg) const;
	virtual bool containsPartOf(const Polygon3 &poly) const;
	virtual bool containsAllOf(const Polygon3 &poly) const;

	virtual void paint() const;

	Region2d & getRegion2d();
	void setRegion2d(const Region2d &r2d);


private:
	void projectPolygon(Polygon2 &projectedPoly, const Polygon3 &poly3) const;
};



#endif
