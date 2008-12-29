//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BACKGROUNDMESH_H__
#define BACKGROUNDMESH_H__

#include <PlatformSpecific/IncludeGL.h>

#include <PlatformSpecific/GLExtensions.h>

#include <Util/Array.h>

#include <Math/Point3f.h>
#include <Math/Vector3f.h>


class GS_DllExport BackgroundMesh
{
public:
	typedef Array<int> IndexFace;


	struct IndexTri
	{
		int i[3];
	};


	struct Vertex
	{
		Point3f v;
		Vector3f n;
	};



private:
	Array<Point3f> vPosition;
	Array<Vector3f> vNormal;
	Array<IndexTri> tris;

	enum
	{
		BUFFERINDEX_POSITION = 0,
		BUFFERINDEX_NORMAL = 1,
		BUFFERINDEX_INDICES = 2,
		NUMBUFFERS = 3
	};

	GLuint buffers[NUMBUFFERS];
	bool bInitialisedGL;


public:
	BackgroundMesh();
	BackgroundMesh(Array<Point3f> &inVerts, Array<IndexFace> &inFaces);


	void initGL();
	void drawGL();
	void shutdownGL();
};


#endif

