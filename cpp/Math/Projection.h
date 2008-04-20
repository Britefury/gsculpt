//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PROJECTION_H__
#define PROJECTION_H__

#include <Util/Array.h>

#include <Math/Matrix4.h>
#include <Math/Plane.h>
#include <Math/Point2.h>
#include <Math/Point3.h>
#include <Math/Polygon2.h>
#include <Math/Polygon3.h>
#include <Math/Segment3.h>



class GS_DllExport Projection
{
private:
	Matrix4 worldToScreenMatrix, screenToWorldMatrix;
	Plane nearClipPlane, farClipPlane;

public:
	Projection();
	Projection(const Matrix4 &worldToScreen, const Matrix4 &screenToWorld, const Plane &nearClip, const Plane &farClip);


	const Matrix4 & getWorldToScreenMatrix() const;
	const Matrix4 & getScreenToWorldMatrix() const;
	const Plane & getNearClipPlane() const;
	const Plane & getFarClipPlane() const;


	void py_setWorldToScreenMatrix(const Matrix4 &m)
	{
		worldToScreenMatrix = m;
	}

	void py_setScreenToWorldMatrix(const Matrix4 &m)
	{
		screenToWorldMatrix = m;
	}

	void py_setNearClipPlane(const Plane &p)
	{
		nearClipPlane = p;
	}

	void py_setFarClipPlane(const Plane &p)
	{
		farClipPlane = p;
	}


	Point3 transformWorldToScreen(const Point3 &world) const;
	Point3 transformScreenToWorldScreenZ(const Point2 &screen, double z) const;

	Segment3 computeRay(const Point2 &screen) const;

	bool containsPoint(const Point3 &point) const;
	Point3 projectPoint(const Point3 &point) const;
	Point2 projectPoint2d(const Point3 &point) const;
	bool checkAndProjectPoint(Point3 &out, const Point3 &in) const;
	bool checkAndProjectPoint2d(Point2 &out, const Point3 &in) const;

	bool clipSegment(Segment3 &seg) const;
	Segment3 projectSegment(const Segment3 &seg) const;
	Segment2 projectSegment2d(const Segment3 &seg) const;
	bool clipAndProjectSegment(Segment3 &out, Segment3 in) const;
	bool clipAndProjectSegment2d(Segment2 &out, Segment3 in) const;

	void clipPoly(Polygon3 &poly) const;
	void clipPoly(Polygon3 &poly, Array<int> &indices) const;
	void clipAndProjectPoly(Polygon3 &poly) const;
	void clipAndProjectPoly(Polygon3 &poly, Array<int> &indices) const;
	void clipAndProjectPoly(Polygon2 &out, const Polygon3 &in) const;
	void clipAndProjectPoly(Polygon2 &out, Array<int> &indices, const Polygon3 &in) const;

	Projection preTransformed(const Matrix4 &m, const Matrix4 &mInv) const;


private:
	static void generateInitialIndices(Array<int> &indices, int size);
	void projectPoly(Polygon3 &poly) const;
	void projectPoly(Polygon2 &out, const Polygon3 &in) const;
};


#endif

