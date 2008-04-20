//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHBUILD_CPP__
#define MESHBUILD_CPP__

#include <Math/MathConstants.h>
#include <Math/index.h>

#include <Mesh/MMesh/MeshBuild.h>



void meshGenerateMeshQuads_row(Array<int> &quads, int row0Offset, int row1Offset, int widthVertices, bool wrap)
{
	for (int i = 1; i < widthVertices; i++)
	{
		quads.push_back( row0Offset + i - 1 );
		quads.push_back( row0Offset + i );
		quads.push_back( row1Offset + i );
		quads.push_back( row1Offset + i - 1 );
	}

	if ( wrap )
	{
		quads.push_back( row0Offset + widthVertices - 1 );
		quads.push_back( row0Offset );
		quads.push_back( row1Offset );
		quads.push_back( row1Offset + widthVertices - 1 );
	}
}


void meshGenerateQuadMesh(Array<int> &quads, int widthFaces, int heightFaces, bool wWrap, bool hWrap)
{
	int widthVertices = wWrap ? widthFaces : widthFaces + 1;
	int heightVertices = hWrap ? heightFaces : heightFaces + 1;

	quads.reserve( widthFaces * heightFaces << 2 );

	int offset = 0;
	for (int i = 1; i < heightVertices; i++)
	{
		meshGenerateMeshQuads_row( quads, offset, offset + widthVertices, widthVertices, wWrap );

		offset += widthVertices;
	}

	if ( hWrap )
	{
		meshGenerateMeshQuads_row( quads, offset, 0, widthVertices, wWrap );
	}
}


void meshGenerateTriFan(Array<int> &tris, int numTris, int centreVertex, bool wrap)
{
	for (int i = 0; i < numTris; i++)
	{
		int a = i;
		int b = wrap  ?  nextIndex( i, numTris )  :  i + 1;
		tris.push_back( a );
		tris.push_back( b );
		tris.push_back( centreVertex );
	}
}

void meshGenerateCircularTextureCoords(Array<Point2f> &tex, int vertices, bool wrap)
{
	double theta = 0.0, thetaInc = 2.0 * M_PI / (double)vertices;

	tex.reserve( vertices );

	for (int i = 0; i < vertices; i++)
	{
		tex.push_back( Point2f( 0.5  +  sin( theta ) * 0.5, 0.5  +  cos( theta ) * 0.5 ) );
		theta += thetaInc;
	}

	if ( wrap )
	{
		tex.push_back( Point2f( 0.5, 1.0 ) );
	}
}




#endif

