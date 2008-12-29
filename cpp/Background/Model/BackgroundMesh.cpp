//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BACKGROUNDMESH_CPP__
#define BACKGROUNDMESH_CPP__

#define BUFFER_OFFSET(i) (((char*)NULL) + (i))

#include <PlatformSpecific/IncludeGL.h>
#include <PlatformSpecific/GLExtensions.h>

#include <Model/ModelDraw.h>


#include <Background/Model/BackgroundMesh.h>




/*static void setupUntexturedMaterial(bool reflection)
{
	Colour3f diffCol = reflection  ?  getSurfaceDiffuseColour() * getReflectedSurfaceFilterColour() : getSurfaceDiffuseColour();
	const Colour3f &specCol = getSurfaceSpecularColour();
	GLfloat diff[] = { diffCol.r, diffCol.g, diffCol.b, 0.0f };
	GLfloat spec[] = { specCol.r, specCol.g, specCol.b, 0.0f };
	GLfloat shin[] = { 35.0f };

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
}*/

static void setupTransparentMaterial()
{
	const Colour3f &diffCol = getTransparentBackgroundSurfaceDiffuseColour();
	const Colour3f &specCol = getTransparentBackgroundSurfaceSpecularColour();
	float alpha = getTransparentBackgroundSurfaceAlpha();

	GLfloat diff[] = { diffCol.r, diffCol.g, diffCol.b, alpha };
	GLfloat spec[] = { specCol.r, specCol.g, specCol.b, alpha };
	GLfloat shin[] = { 35.0f };

	glMaterialfv( GL_FRONT, GL_DIFFUSE, diff );
	glMaterialfv( GL_FRONT, GL_SPECULAR, spec );
	glMaterialfv( GL_FRONT, GL_SHININESS, shin );
}

static void beginSolidRendering()
{
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_POLYGON_OFFSET_FILL );

	glPolygonOffset( 1.0, 1.0 );

	glEnable( GL_LIGHTING );
}

static void endSolidRendering()
{
	glDisable( GL_LIGHTING );

	glDisable( GL_POLYGON_OFFSET_FILL );
}






BackgroundMesh::BackgroundMesh()
{
	bInitialisedGL = false;
}

BackgroundMesh::BackgroundMesh(Array<Point3f> &inVerts, Array<IndexFace> &inFaces)
{
	bInitialisedGL = false;

	// Face index buffer
	IndexFace faceIndices;
	// Edge vectors
	Vector3f edges[2];
	// Normals for faces
	Array<Vector3f> faceNormals;
	// Normal vector
	Vector3f n;

	vNormal.resize( inVerts.size() );
	tris.reserve( inFaces.size() );

	// Copy vertex positions
	vPosition = inVerts;

	// For each face
	for (int faceI = 0; faceI < inFaces.size(); faceI++)
	{
		// Copy the face indices into @faceIndices, removing any duplicate vertices
		IndexFace &inFace = inFaces[faceI];

		faceIndices.clear();
		faceIndices.push_back( inFace[0] );

		int x = inFace[0];
		for (int faceVertexI = 1; faceVertexI < inFace.size(); faceVertexI++)
		{
			int y = inFace[faceVertexI];
			if ( y != x )
			{
				faceIndices.push_back( y );
				x = y;
			}
		}


		if ( faceIndices.size() >= 3 )
		{
			// The face is not degenerate

			// Compute the normal of the face
			int aIndex = faceIndices[0], bIndex = faceIndices[1];
			const Point3f *a = &inVerts[aIndex];
			const Point3f *b = &inVerts[bIndex];
			edges[0] = *b - *a;
			n = Vector3f( 0.0f, 0.0f, 0.0f );
			Vector3f *u = &edges[0], *v = &edges[1];
			for (int cFaceVertexIndex = 2; cFaceVertexIndex < faceIndices.size(); cFaceVertexIndex++)
			{
				int cIndex = faceIndices[cFaceVertexIndex];
				const Point3f *c = &inVerts[cIndex];
				*v = *c - *a;

				n += u->cross( *v );

				tris.push_back( IndexTri() );
				IndexTri &tri = tris.back();
				tri.i[0] = aIndex;
				tri.i[1] = bIndex;
				tri.i[2] = cIndex;

				bIndex = cIndex;
				b = c;
				std::swap( u, v );
			}

			faceNormals.push_back( n * ( 1.0f / n.length() ) );
			const Vector3f &unitN = faceNormals.back();

			// Accumulate normals per vertex
			for (int faceVertexI = 0; faceVertexI < faceIndices.size(); faceVertexI++)
			{
				int vIndex = faceIndices[faceVertexI];
				vNormal[vIndex] += unitN;
			}
		}
	}


	// Normalise vertex normals
	for (int vertexI = 0; vertexI < inVerts.size(); vertexI++)
	{
		vNormal[vertexI].normalise();
	}
}




void BackgroundMesh::initGL()
{
	if ( !bInitialisedGL )
	{
		bInitialisedGL = true;

		GLExtensions *glext = getGLExtensions();
		if ( glext != NULL )
		{
			glext->glGenBuffers( NUMBUFFERS, buffers );
		
			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_POSITION] );
			glext->glBufferData( GL_ARRAY_BUFFER, sizeof( Point3f ) * vPosition.size(), vPosition.begin(), GL_STATIC_DRAW );
			glVertexPointer( 3, GL_FLOAT, 0, NULL );

			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_NORMAL] );
			glext->glBufferData( GL_ARRAY_BUFFER, sizeof( Vector3f ) * vNormal.size(), vNormal.begin(), GL_STATIC_DRAW );
			glNormalPointer( GL_FLOAT, 0, NULL );
		
			glext->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[BUFFERINDEX_INDICES] );
			glext->glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( IndexTri ) * tris.size(), tris.begin(), GL_STATIC_DRAW );
		}
	}
}


void BackgroundMesh::drawGL()
{
	if ( bInitialisedGL )
	{
		GLExtensions *glext = getGLExtensions();
		if ( glext != NULL )
		{
			glEnableClientState( GL_VERTEX_ARRAY );
			glEnableClientState( GL_NORMAL_ARRAY );
		
			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_POSITION] );

			glext->glBindBuffer( GL_ARRAY_BUFFER, buffers[BUFFERINDEX_NORMAL] );

			glext->glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, buffers[BUFFERINDEX_INDICES] );
		




			beginSolidRendering();

			setupTransparentMaterial();

			glDrawElements( GL_TRIANGLES, tris.size() * 3, GL_UNSIGNED_INT, 0 );

			endSolidRendering();





			glDisableClientState( GL_VERTEX_ARRAY );
			glDisableClientState( GL_NORMAL_ARRAY );
		}
	}
}


void BackgroundMesh::shutdownGL()
{
	if ( bInitialisedGL )
	{
		bInitialisedGL = false;
		GLExtensions *glext = getGLExtensions();
		if ( glext != NULL )
		{
			glext->glDeleteBuffers( NUMBUFFERS, buffers );
		}
	}
}



#endif
