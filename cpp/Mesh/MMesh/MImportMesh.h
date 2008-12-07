//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MIMPORTMESH_H__
#define MIMPORTMESH_H__

#include <Util/Array.h>

#include <Math/Point3.h>
#include <Math/Vector3f.h>
#include <Math/Point2f.h>


class GS_DllExport MImportMesh
{
public:
	class GS_DllExport FaceVertex
	{
	private:
		int vertexIndex, textureCoordIndex;

	public:
		inline FaceVertex() : vertexIndex( -1 ), textureCoordIndex( -1 )
		{
		}

		inline FaceVertex(int vertexIndex, int textureCoordIndex = -1)
					: vertexIndex( vertexIndex ), textureCoordIndex( textureCoordIndex )
		{
		}


		inline int getVertexIndex() const
		{
			return vertexIndex;
		}

		inline int getTextureCoordIndex() const
		{
			return textureCoordIndex;
		}
	};


	typedef Array<FaceVertex> Face;


private:
	Array<Point3> importVertices;
	Array<Point2f> importTextureCoords;
	Array<Face> importFaces;
	bool bFinalised;


public:
	MImportMesh();


	void reserve(int numVertices, int numTextureCoords, int numFaces);
	int addVertex(const Point3 &position);
	int addTextureCoord(const Point2f &tex);
	int addFace(const Face &face);

	Array<Point3> & getVertexArray();
	Array<Point2f> & getTextureCoordArray();
	Array<Face> & getFaceArray();


	void finalise();


private:
	bool isFinalised() const;


	friend class MMesh;
};


#endif

