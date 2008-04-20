//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHPRIMITIVES_H__
#define MESHPRIMITIVES_H__

#include <Util/Array.h>

#include <Mesh/GSProductMesh.h>



class GS_DllExport MeshVertexList
{
public:
	Array<int> vertexIndices;


	MeshVertexList();
	virtual ~MeshVertexList();

	virtual void clear();
	virtual void setCapacityToZero();
	virtual void transform(const Matrix4 &m, GSProduct *o);
	virtual void savePositions(GSProduct *o);
	virtual void restorePositions(GSProduct *o);



	friend class ProcCreateMesh;
	friend class ToolCreateMesh;
};






class GS_DllExport BoxPrimitive
{
public:
	int xSegments, ySegments, zSegments;
	double width, height, depth;
	bool frontFlag, backFlag, leftFlag, rightFlag, bottomFlag, topFlag;


public:
	BoxPrimitive(int xSegments, int ySegments, int zSegments,
				double width, double height, double depth,
				bool frontFlag, bool backFlag, bool leftFlag, bool rightFlag, bool bottomFlag, bool topFlag);


	void createBox(GSProductMesh *mesh, MeshVertexList *meshVertices);


private:
	void createEdge(GSProductMesh *mesh,  int startVertex, int endVertex, int sections, Array<int> &vertices, MeshVertexList *meshVertices);

	void createSide(GSProductMesh *mesh, const Array<int> &topEdge, const Array<int> &bottomEdge, const Array<int> &leftEdge, const Array<int> &rightEdge,
				MeshVertexList *meshVertices);

	Array<int> reverseVertexList(const Array<int> &createdVerts) const;
};




class GS_DllExport PlanePrimitive
{
public:
	Axis axis;
	int uSegments, vSegments;
	double width, height;


public:
	PlanePrimitive(Axis axis, int uSegments, int vSegments, double width, double height);


	void createPlane(GSProductMesh *mesh, MeshVertexList *meshVertices);
};




class GS_DllExport SpherePrimitive
{
public:
	Axis axis;
	int stacks, slices;
	double radius;


public:
	SpherePrimitive(Axis axis, int stacks, int slices, double radius);


	void createSphere(GSProductMesh *mesh, MeshVertexList *meshVertices);
};




class GS_DllExport CylinderPrimitive
{
public:
	Axis axis;
	int angularSegments, lengthSegments, radialSegments;
	double radius1, radius2, length;
	bool bCapStart, bCapEnd;


public:
	CylinderPrimitive(Axis axis, int angularSegments, int lengthSegments, int radialSegments, double radius1, double radius2, double length, bool bCapStart, bool bCapEnd);


	void createCylinder(GSProductMesh *mesh, MeshVertexList *meshVertices);


private:
	void generateConePoint(GSProductMesh *mesh, const Array<int> &vertexIndices, int vertexRingOffset, int centreVertex,
							bool invert, double ringTexV, double pointTexV);
	void generateCap(GSProductMesh *mesh, const Array<int> &vertexIndices, int vertexRingOffset, bool invert,
					const Point3 &position, const Axes3 &axes, double radius, MeshVertexList *meshVertices);
	void generateCapPolygon(GSProductMesh *mesh, const Array<int> &vertexIndices, int offset, bool invert, double textureDiameter);
};




class GS_DllExport TorusPrimitive
{
public:
	Axis axis;
	int majorSegments, minorSegments;
	double majorRadius, minorRadius;


public:
	TorusPrimitive(Axis axis, int majorSegments, int minorSegments, double majorRadius, double minorRadius);


	void createTorus(GSProductMesh *mesh, MeshVertexList *meshVertices);
};




class GS_DllExport TubePrimitive
{
public:
	enum CapMaterialMapping
	{
		CAPMATERIALMAPPING_PLANAR,
		CAPMATERIALMAPPING_POLAR
	};

	Axis axis;
	int radialSegments, lengthSegments, slices;
	double innerRadius, outerRadius, length;
	bool bInnerSection, bOuterSection, bStartCap, bEndCap;
	CapMaterialMapping capMapping;


public:
	TubePrimitive(Axis axis, int radialSegments, int lengthSegments, int slices, double innerRadius, double outerRadius, double length,
				bool bInnerSection, bool bOuterSection, bool bStartCap, bool bEndCap, CapMaterialMapping capMapping);


	void createTube(GSProductMesh *mesh, MeshVertexList *meshVertices);


private:
	void createVertexRing(GSProductMesh *mesh, Array<int> &vertexIndices, const Point3 &centre, double radius,
						const Vector3 &vi, const Vector3 &vj, MeshVertexList *meshVertices);
	void createSectionVertices(GSProductMesh *mesh, Array<int> &vertexIndices, const Point3 &centreA, const Point3 &centreB,
							double radiusA, double radiusB, const Vector3 &vi, const Vector3 &vj, int numSegments,
							MeshVertexList *meshVertices);

	void createSectionFaces(GSProductMesh *mesh, const Array<int> &vertexIndices, int beginAt, bool innerSection);
	void createCapFaces(GSProductMesh *mesh, const Array<int> &vertexIndices, int beginAt, double texRadiusA, double texRadiusB);
};



#endif
