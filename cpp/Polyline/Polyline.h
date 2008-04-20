//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYLINE_H__
#define POLYLINE_H__

#include <stdio.h>

#include <Util/Array.h>

#include <Math/Axis.h>
#include <Math/BBox3.h>
#include <Math/Matrix4.h>
#include <Math/IndexTriangle.h>

#include <Transformation/Transformation.h>

#include <Model/MarkPredicate.h>

#include <Graphics/Region.h>

#include <PolyBlend/BlendingPolyline.h>

#include <Polyline/PolylineTypes.h>
#include <Polyline/PVertex.h>


/*
*******************************************************************************
										PolylineEdgeTexture
*******************************************************************************
*/

class GS_DllExport PolylineEdgeTexture
{
private:
	int textureID;
	double u0, u1;

public:
	PolylineEdgeTexture();
	PolylineEdgeTexture(int tid, double u0, double u1);

	int getTextureID() const;
	double getU0() const;
	double getU1() const;
};



/*
*******************************************************************************
									PolylineEdgeTextureTable
*******************************************************************************
*/

class GS_DllExport PolylineEdgeTextureTable
{
private:
	Array<PolylineEdgeTexture> table;
	int numTextures;

public:
	PolylineEdgeTextureTable();


	const PolylineEdgeTexture &operator[](int i) const;
	int getNumTextures() const;

private:
	void reserve(int s);
	void add(const PolylineEdgeTexture &t);
	void rotateBackward(int n);

	friend class Polyline;
};



/*
*******************************************************************************
											Polyline
*******************************************************************************
*/

class GS_DllExport Polyline
{
private:
	Array<PVertex> vertices;
	bool closedFlag;
	int markedVertexCount;

public:
	Polyline();


	void read(FILE *f);
	void write(FILE *f) const;

	void copyFrom(const Polyline &p);


	int computeMemoryUsage() const;


	int getNumVertices() const;
	int getNumEdges() const;

	int size() const;
	const PVertex& operator[](int i) const;
	const PVertex& at(int i) const;


	int addVertex(const PVertex &v);
	int addVertex(const Point3 &location, bool marked = true);
	int addVertexAt(int position, const PVertex &v);
	int addVertexAt(int position, const Point3 &location, bool marked = true);
	void removeVertex(PVertex* vtx);
	void swapVertices(PVertex *a, PVertex *b);
	void copyVertexPropertiesFrom(int destIndex, const PVertex &srcVertex);

	void removeMarkedVertices();

	void flip();

	void reserveVertices(int s);
	void clear();

	void rotateForward(int positions);
	void rotateBackward(int positions);


	void close();
	void open();
	void setClosed(bool c);
	bool isClosed() const;


	void transform(const Matrix4 &m, bool markedOnly);
	void transform(const Transformation *x, bool markedOnly);
	void savePositions(bool markedOnly);
	void restorePositions(bool markedOnly);


	void mark(MarkPredicate predicate, const Region3d *region);
	void markAll(MarkPredicate predicate);
	void saveMark();
	void restoreMark();
	MarkStatus getMarkStatus() const;
	Side areVerticesInside(const Region3d *region) const;


	BBox3 getBBox(bool markedOnly) const;
	BBox3 getBBox(const Matrix4 &m, bool markedOnly) const;
	BBox3 getBBox(const Transformation *x, bool markedOnly) const;
	BBox3 getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;


	void transformVertices(const Array<int> &vertexIndices, const Matrix4 &xform);
	void saveVertexPositions(const Array<int> &vertexIndices);
	void restoreVertexPositions(const Array<int> &vertexIndices);


	void mirror(const Plane &mirrorPlane, bool reverseOrder, bool markedOnly);
	void interleave(int markedStride, int unmarkedStride);

	void subdivide(int numToInsert, bool markedFlag, PSubdivCurveType curveType, bool betweenMarkedVerticesOnlyFlag);
private:
	void subdivideEdge(int n, int nPlus1, const Array<PVertex> &oldVertices, int numToInsert, bool markedFlag, PSubdivCurveType curveType, bool betweenMarkedVerticesOnlyFlag);

public:
	void normalSharpenMarkedVertices();
	void textureBreakMarkedVertices();




	void computeCumulativeLengthArray(Array<double> &lengthArray) const;
	void computeUnitCumulativeLengthArray(Array<double> &lengthArray) const;
	double computeLength() const;

	void flatten(Axis flattenAxis = AXIS_Z);
	void flatten(Vector3 flattenAxis);
	void generatePointList(Array<Point3> &pointList) const;
	void generatePointList(Array<Point2> &pointList, Axis flattenAxis) const;
	void computePlanarTextureCoords(Array<Point2f> &texCoords, Axis planeNormalAxis) const;
	Plane computePlane() const;
	void getBlendingPolyline(BlendingPolyline &p) const;

	void computeEdgeTextureTable(PolylineEdgeTextureTable &table) const;

	static void computeIdentityTextureTable(int numSegments, PolylineEdgeTextureTable &table);
};



#endif
