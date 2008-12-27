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
	Array<Vertex> vertices;
	Array<IndexTri> tris;

	enum
	{
		BUFFERINDEX_VERTICES = 0,
		BUFFERINDEX_INDICES = 1,
		NUMBUFFERS = 2
	};

	GLuint buffers[NUMBUFFERS];


public:
	BackgroundMesh();
	BackgroundMesh(Array<Point3f> &inVerts, Array<IndexFace> &inFaces);


	void initGL();
	void drawGL();
	void shutdownGL();
};


#endif

