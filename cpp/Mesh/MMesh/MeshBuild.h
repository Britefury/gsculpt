//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHBUILD_H__
#define MESHBUILD_H__

#include <Util/Array.h>
#include <Math/Point2f.h>

//generates the indices of the vertices which make up quadrilateral faces, which
//form a rectangular patch
GS_DllExport void meshGenerateQuadMesh(Array<int> &quads, int widthFaces, int heightFaces, bool wWrap, bool hWrap);

GS_DllExport void meshGenerateTriFan(Array<int> &tris, int numTris, int centreVertex, bool wrap);
GS_DllExport void meshGenerateCircularTextureCoords(Array<Point2f> &tex, int vertices, bool wrap);


#endif
