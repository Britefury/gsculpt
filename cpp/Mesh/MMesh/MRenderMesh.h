//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MRENDERMESH_H__
#define MRENDERMESH_H__

#include <Math/Point3.h>
#include <Math/Point2f.h>
#include <Math/Point3f.h>

#include <Mesh/MMesh/MeshTypes.h>


class GS_DllExport MRenderMesh
{
private:
	class GS_DllExport BuildVertex
	{
	private:
		int srcMaterialIndex, srcNormalIndex;
		int nextIndex;

	public:
		inline BuildVertex()
						: srcMaterialIndex( -1 ), srcNormalIndex( -1 ), nextIndex( -1 )
		{
		}

		inline BuildVertex(int srcMaterialIndex, int srcNormalIndex)
					: srcMaterialIndex( srcMaterialIndex ), srcNormalIndex( srcNormalIndex ), nextIndex( -1 )
		{
		}


		inline void link(int nextIndex)
		{
			this->nextIndex = nextIndex;
		}


		inline bool matches(int srcMaterialIndex, int srcNormalIndex)
		{
			return this->srcMaterialIndex == srcMaterialIndex  &&  this->srcNormalIndex == srcNormalIndex;
		}


		inline int getSourceMaterialIndex() const
		{
			return srcMaterialIndex;
		}

		inline int getSourceNormalIndex() const
		{
			return srcMaterialIndex;
		}

		inline int getNextIndex() const
		{
			return nextIndex;
		}
	};

	typedef Array<int> Face;


	Array<Point3> pointData;
	Array<Point3> positionData;
	Array<Point2f> materialData;
	Array<Vector3f> normalData;
	Array<Face> faceData;

	Array<BuildVertex> buildVerts;
	Array<int> buildVertIndices;



public:
	MRenderMesh();


	void init(int numVertices);
	int getRenderVertexIndex(MVertex *vertex, MVertexAttrib *material, MVertexNormal *normal);
	void addFace(const Array<int> &indices);
};


#endif //MRENDERMESH_H__
