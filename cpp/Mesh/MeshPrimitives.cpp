//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHPRIMITIVES_CPP__
#define MESHPRIMITIVES_CPP__

#include <Math/index.h>

#include <Mesh/MMesh/MeshBuild.h>

#include <Mesh/MeshPrimitives.h>






MeshVertexList::MeshVertexList()
{
}

MeshVertexList::~MeshVertexList()
{
}


void MeshVertexList::clear()
{
	vertexIndices.clear();
}

void MeshVertexList::setCapacityToZero()
{
	vertexIndices.setCapacity( 0 );
}

void MeshVertexList::transform(const Matrix4 &m, GSProduct *o)
{
	GSProductMesh *mesh = (GSProductMesh*)o;

	mesh->transformVertices( vertexIndices, m );
	mesh->finalise();
}

void MeshVertexList::savePositions(GSProduct *o)
{
	GSProductMesh *mesh = (GSProductMesh*)o;

	mesh->saveVertexPositions( vertexIndices );
}

void MeshVertexList::restorePositions(GSProduct *o)
{
	GSProductMesh *mesh = (GSProductMesh*)o;

	mesh->restoreVertexPositions( vertexIndices );
	mesh->finalise();
}




BoxPrimitive::BoxPrimitive(int xSegments, int ySegments, int zSegments,
						double width, double height, double depth,
						bool frontFlag, bool backFlag, bool leftFlag, bool rightFlag, bool bottomFlag, bool topFlag)
						: xSegments( xSegments ), ySegments( ySegments ), zSegments( zSegments ),
						width( width ), height( height ), depth( depth ),
						frontFlag( frontFlag ), backFlag( backFlag ), leftFlag( leftFlag ), rightFlag( rightFlag ), bottomFlag( bottomFlag ), topFlag( topFlag )
{
}


void BoxPrimitive::createBox(GSProductMesh *mesh, MeshVertexList *meshVertices)
{
	double halfWidth = width * 0.5;
	double halfHeight = height * 0.5;
	double halfDepth = depth * 0.5;

	//create (and get indices of) corner vertices
	//lf=left, rt=right, bt=bottom, tp=top, fr=front, bk=back
	int lfbtfr = 0, rtbtfr = 0, lftpfr = 0, rttpfr = 0, lfbtbk = 0, rtbtbk = 0, lftpbk = 0, rttpbk = 0;

	if ( leftFlag || bottomFlag || frontFlag )
	{
		lfbtfr = mesh->addVertex( Point3( -halfWidth, -halfHeight, halfDepth ) );
		meshVertices->vertexIndices.push_back( lfbtfr );
	}

	if ( rightFlag || bottomFlag || frontFlag )
	{
		rtbtfr = mesh->addVertex( Point3( halfWidth, -halfHeight, halfDepth ) );
		meshVertices->vertexIndices.push_back( rtbtfr );
	}

	if ( leftFlag || topFlag || frontFlag )
	{
		lftpfr = mesh->addVertex( Point3( -halfWidth, halfHeight, halfDepth ) );
		meshVertices->vertexIndices.push_back( lftpfr );
	}

	if ( rightFlag || topFlag || frontFlag )
	{
		rttpfr = mesh->addVertex( Point3( halfWidth, halfHeight, halfDepth ) );
		meshVertices->vertexIndices.push_back( rttpfr );
	}

	if ( leftFlag || bottomFlag || backFlag )
	{
		lfbtbk = mesh->addVertex( Point3( -halfWidth, -halfHeight, -halfDepth ) );
		meshVertices->vertexIndices.push_back( lfbtbk );
	}

	if ( rightFlag || bottomFlag || backFlag )
	{
		rtbtbk = mesh->addVertex( Point3( halfWidth, -halfHeight, -halfDepth ) );
		meshVertices->vertexIndices.push_back( rtbtbk );
	}

	if ( leftFlag || topFlag || backFlag )
	{
		lftpbk = mesh->addVertex( Point3( -halfWidth, halfHeight, -halfDepth ) );
		meshVertices->vertexIndices.push_back( lftpbk );
	}

	if ( rightFlag || topFlag || backFlag )
	{
		rttpbk = mesh->addVertex( Point3( halfWidth, halfHeight, -halfDepth ) );
		meshVertices->vertexIndices.push_back( rttpbk );
	}




	//CREATE EDGES
	Array<int> frontLeftEdge, frontRightEdge, frontTopEdge, frontBottomEdge,
			backLeftEdge, backRightEdge, backTopEdge, backBottomEdge,
			leftTopEdge, leftBottomEdge, rightTopEdge, rightBottomEdge;



	if ( frontFlag || topFlag )
	{
		createEdge( mesh, lftpfr, rttpfr, xSegments, frontTopEdge, meshVertices );
	}

	if ( frontFlag || bottomFlag )
	{
		createEdge( mesh, lfbtfr, rtbtfr, xSegments, frontBottomEdge, meshVertices );
	}

	if ( frontFlag || leftFlag )
	{
		createEdge( mesh, lftpfr, lfbtfr, ySegments, frontLeftEdge, meshVertices );
	}

	if ( frontFlag || rightFlag )
	{
		createEdge( mesh, rttpfr, rtbtfr, ySegments, frontRightEdge, meshVertices );
	}


	if ( backFlag || topFlag )
	{
		createEdge( mesh, rttpbk, lftpbk, xSegments, backTopEdge, meshVertices );
	}

	if ( backFlag || bottomFlag )
	{
		createEdge( mesh, rtbtbk, lfbtbk, xSegments, backBottomEdge, meshVertices );
	}

	if ( backFlag || leftFlag )
	{
		createEdge( mesh, lftpbk, lfbtbk, ySegments, backLeftEdge, meshVertices );
	}

	if ( backFlag || rightFlag )
	{
		createEdge( mesh, rttpbk, rtbtbk, ySegments, backRightEdge, meshVertices );
	}


	if ( leftFlag || topFlag )
	{
		createEdge( mesh, lftpbk, lftpfr, zSegments, leftTopEdge, meshVertices );
	}

	if ( leftFlag || bottomFlag )
	{
		createEdge( mesh, lfbtbk, lfbtfr, zSegments, leftBottomEdge, meshVertices );
	}

	if ( rightFlag || topFlag )
	{
		createEdge( mesh, rttpfr, rttpbk, zSegments, rightTopEdge, meshVertices );
	}

	if ( rightFlag || bottomFlag )
	{
		createEdge( mesh, rtbtfr, rtbtbk, zSegments, rightBottomEdge, meshVertices );
	}


	//front
	if ( frontFlag )
	{
		createSide( mesh, frontTopEdge, frontBottomEdge, frontLeftEdge, frontRightEdge, meshVertices );
	}

	//right
	if ( rightFlag )
	{
		createSide( mesh, rightTopEdge, rightBottomEdge, frontRightEdge, backRightEdge, meshVertices );
	}

	//back
	if ( backFlag )
	{
		createSide( mesh, backTopEdge, backBottomEdge, backRightEdge, backLeftEdge, meshVertices );
	}

	//left
	if ( leftFlag )
	{
		createSide( mesh, leftTopEdge, leftBottomEdge, backLeftEdge, frontLeftEdge, meshVertices );
	}

	//top
	if ( topFlag )
	{
		createSide( mesh, reverseVertexList( backTopEdge ), frontTopEdge, leftTopEdge, reverseVertexList( rightTopEdge ), meshVertices );
	}

	//bottom
	if ( bottomFlag )
	{
		createSide( mesh, frontBottomEdge, reverseVertexList( backBottomEdge ), reverseVertexList( leftBottomEdge ), rightBottomEdge, meshVertices );
	}



	if ( frontFlag || topFlag )
	{
		mesh->setEdgesAsNormalSharp( frontTopEdge, 0, false );
	}

	if ( frontFlag || bottomFlag )
	{
		mesh->setEdgesAsNormalSharp( frontBottomEdge, 0, false );
	}

	if ( frontFlag || leftFlag )
	{
		mesh->setEdgesAsNormalSharp( frontLeftEdge, 0, false );
	}

	if ( frontFlag || rightFlag )
	{
		mesh->setEdgesAsNormalSharp( frontRightEdge, 0, false );
	}


	if ( backFlag || topFlag )
	{
		mesh->setEdgesAsNormalSharp( backTopEdge, 0, false );
	}

	if ( backFlag || bottomFlag )
	{
		mesh->setEdgesAsNormalSharp( backBottomEdge, 0, false );
	}

	if ( backFlag || leftFlag )
	{
		mesh->setEdgesAsNormalSharp( backLeftEdge, 0, false );
	}

	if ( backFlag || rightFlag )
	{
		mesh->setEdgesAsNormalSharp( backRightEdge, 0, false );
	}


	if ( leftFlag || topFlag )
	{
		mesh->setEdgesAsNormalSharp( leftTopEdge, 0, false );
	}

	if ( leftFlag || bottomFlag )
	{
		mesh->setEdgesAsNormalSharp( leftBottomEdge, 0, false );
	}

	if ( rightFlag || topFlag )
	{
		mesh->setEdgesAsNormalSharp( rightTopEdge, 0, false );
	}

	if ( rightFlag || bottomFlag )
	{
		mesh->setEdgesAsNormalSharp( rightBottomEdge, 0, false );
	}


	mesh->finalise();
}

void BoxPrimitive::createEdge(GSProductMesh *mesh, int startVertex, int endVertex, int sections, Array<int> &vertices, MeshVertexList *meshVertices)
{
	const Point3 &start = mesh->getVertexPosition( startVertex );
	const Point3 &end = mesh->getVertexPosition( endVertex );

	Vector3 edgeVector = end - start;
	Vector3 delta = edgeVector * ( 1.0 / (double)sections );


	Point3 position = start + delta;


	vertices.reserve( sections + 1 );

	vertices.push_back( startVertex );

	for (int i = 1; i < sections; i++)
	{
		int vertexIndex = mesh->addVertex( position );
		vertices.push_back( vertexIndex );
		meshVertices->vertexIndices.push_back( vertexIndex );

		position += delta;
	}

	vertices.push_back( endVertex );
}

void BoxPrimitive::createSide(GSProductMesh *mesh, const Array<int> &topEdge, const Array<int> &bottomEdge, const Array<int> &leftEdge, const Array<int> &rightEdge,
							MeshVertexList *meshVertices)
{
	Array<int> vertices;

	vertices.reserve( topEdge.size() * leftEdge.size() );

	for (int i = 0; i < topEdge.size(); i++)
	{
		vertices.push_back( topEdge[i] );
	}


	for (int i = 1; i < ( leftEdge.size() - 1 ); i++)
	{
		Array<int> edgeVertices;

		createEdge( mesh, leftEdge[i], rightEdge[i], topEdge.size() - 1, edgeVertices, meshVertices );

		for (int j = 0; j < edgeVertices.size(); j++)
		{
			vertices.push_back( edgeVertices[j] );
		}
	}


	for (int i = 0; i < bottomEdge.size(); i++)
	{
		vertices.push_back( bottomEdge[i] );
	}



	int horizontalVertices = topEdge.size();
	int horizontalFaces = topEdge.size() - 1;
	int verticalFaces = leftEdge.size() - 1;

	double u = 0.0, uInc = 1.0 / (double)horizontalFaces;
	double v = 1.0, vInc = -1.0 / (double)verticalFaces;

	for (int y = 0; y < verticalFaces; y++)
	{
		u = 0.0;

		for (int x = 0; x < horizontalFaces; x++)
		{
			int a = y * horizontalVertices + x;
			int b = ( y + 1 ) * horizontalVertices + x;
			int c = ( y + 1 ) * horizontalVertices + x + 1;
			int d = y * horizontalVertices + x + 1;

			mesh->addFace( vertices[a], Point2f( u, v ),
							vertices[b], Point2f( u, v + vInc ),
							vertices[c], Point2f( u + uInc, v + vInc ),
							vertices[d], Point2f( u + uInc, v ) );

			u += uInc;
		}

		v += vInc;
	}
}

Array<int> BoxPrimitive::reverseVertexList(const Array<int> &vertices) const
{
	Array<int> result;

	result.reserve( vertices.size() );

	for (int i = vertices.size() - 1; i >= 0; i--)
	{
		result.push_back( vertices[i] );
	}

	return result;
}





PlanePrimitive::PlanePrimitive(Axis axis, int uSegments, int vSegments, double width, double height)
						: axis( axis ), uSegments( uSegments ), vSegments( vSegments ), width( width ), height( height )
{
}


void PlanePrimitive::createPlane(GSProductMesh *mesh, MeshVertexList *meshVertices)
{
	Axes3 axes = Axes3( axis );


	mesh->reserveVertices( ( uSegments + 1 )  * ( vSegments + 1 ) );
	mesh->reserveFaces( uSegments * vSegments );



	double x, xInc = width / (double)uSegments;
	double y = -height * 0.5, yInc = height / (double)vSegments;
	double u = 0.0, uInc = 1.0 / (double)uSegments;
	double v = 0.0, vInc = 1.0 / (double)vSegments;

	Array<Point2f> tex;
	tex.reserve( ( uSegments + 1 )  *  ( vSegments + 1 ) );


	for (int j = 0; j <= vSegments; j++)
	{
		x = -width * 0.5;
		u = 0.0;
		for (int i = 0; i <= uSegments; i++)
		{
			int index = mesh->addVertex( Point3() + axes.i * x + axes.j * y );
			tex.push_back( Point2f( u, v ) );
			meshVertices->vertexIndices.push_back( index );
			x += xInc;
			u += uInc;
		}
		y += yInc;
		v += vInc;
	}


	Array<int> faceQuads;
	meshGenerateQuadMesh( faceQuads, uSegments, vSegments, false, false );

	for (int i = 0; i < faceQuads.size(); i += 4)
	{
		mesh->addFace( faceQuads[i], tex[ faceQuads[i] ],
						faceQuads[i+1], tex[ faceQuads[i+1] ],
						faceQuads[i+2], tex[ faceQuads[i+2] ],
						faceQuads[i+3], tex[ faceQuads[i+3] ] );
	}


	mesh->finalise();
}





SpherePrimitive::SpherePrimitive(Axis axis, int stacks, int slices, double radius)
						: axis( axis ), stacks( stacks ), slices( slices ), radius( radius )
{
}


void SpherePrimitive::createSphere(GSProductMesh *mesh, MeshVertexList *meshVertices)
{
	Axes3 axes = Axes3( axis );



	mesh->reserveVertices( slices  *  ( stacks - 1 )   +   2 );
	mesh->reserveFaces( stacks * slices );


	Array<int> vertexIndices;
	Array<Point2f> tex;
	vertexIndices.reserve( slices  *  ( stacks + 1 ) );
	tex.reserve( ( slices + 1 )  *  ( stacks + 1 ) );




	int vertexIndex = mesh->addVertex( -axes.k * radius );
	meshVertices->vertexIndices.push_back( vertexIndex );
	double u = 0.0, uInc = 1.0 / (double)slices;

	for (int i = 0; i < slices; i++)
	{
		vertexIndices.push_back( vertexIndex );
		tex.push_back( Point2f( u, 0.0 ) );
		u += uInc;
	}
	tex.push_back( Point2f( u, 0.0 ) );

	double stackThetaInc = M_PI / (double)stacks;
	double stackTheta = stackThetaInc;
	double vInc = 1.0 / (double)stacks;
	double v = vInc;

	for (int i = 1; i < stacks; i++)
	{
		double z = -cos( stackTheta ) * radius;

		double sliceThetaInc = 2.0 * M_PI  /  (double)slices;
		double sliceTheta = 0.0;
		u = 0.0;

		double sinStack = sin( stackTheta );

		for (int j = 0; j < slices; j++)
		{
			double x = cos( sliceTheta )  *  sinStack  *  radius;
			double y = sin( sliceTheta )  *  sinStack  *  radius;

			vertexIndex = mesh->addVertex( axes.i * x  +  axes.j * y  +  axes.k * z );
			vertexIndices.push_back( vertexIndex );
			meshVertices->vertexIndices.push_back( vertexIndex );
			tex.push_back( Point2f( u, v ) );

			sliceTheta += sliceThetaInc;
			u += uInc;
		}
		tex.push_back( Point2f( u, v ) );

		stackTheta += stackThetaInc;
		v += vInc;
	}

	u = 0.0;
	vertexIndex = mesh->addVertex( axes.k * radius );
	meshVertices->vertexIndices.push_back( vertexIndex );
	for (int i = 0; i < slices; i++)
	{
		vertexIndices.push_back( vertexIndex );
		tex.push_back( Point2f( u, 1.0 ) );
		u += uInc;
	}
	tex.push_back( Point2f( u, 1.0 ) );



	Array<int> faceQuads, texQuads;
	meshGenerateQuadMesh( faceQuads, slices, stacks, true, false );
	meshGenerateQuadMesh( texQuads, slices, stacks, false, false );

	for (int i = 0; i < faceQuads.size(); i += 4)
	{
		int a = vertexIndices[ faceQuads[i] ];
		int b = vertexIndices[ faceQuads[i+1] ];
		int c = vertexIndices[ faceQuads[i+2] ];
		int d = vertexIndices[ faceQuads[i+3] ];

		if ( a == b )
		{
			mesh->addFace( a, Point2f::average( tex[ texQuads[i] ], tex[ texQuads[i+1] ] ),
							c, tex[ texQuads[i+2] ],
							d, tex[ texQuads[i+3] ] );
		}
		else if ( c == d )
		{
			mesh->addFace( a, tex[ texQuads[i] ],
							b, tex[ texQuads[i+1] ],
							c, Point2f::average( tex[ texQuads[i+2] ], tex[ texQuads[i+3] ] ) );
		}
		else
		{
			mesh->addFace( a, tex[ texQuads[i] ],
							b, tex[ texQuads[i+1] ],
							c, tex[ texQuads[i+2] ],
							d, tex[ texQuads[i+3] ] );
		}
	}


	mesh->finalise();
}





CylinderPrimitive::CylinderPrimitive(Axis axis, int angularSegments, int lengthSegments, int radialSegments, double radius1, double radius2, double length, bool bCapStart, bool bCapEnd)
						: axis( axis ), angularSegments( angularSegments ), lengthSegments( lengthSegments ), radialSegments( radialSegments ),
						radius1( radius1 ), radius2( radius2 ), length( length ), bCapStart( bCapStart ), bCapEnd( bCapEnd )
{
}


void CylinderPrimitive::createCylinder(GSProductMesh *mesh, MeshVertexList *meshVertices)
{
	Axes3 axes = Axes3( axis );



	bool startIsConePoint = radius1 == 0.0;
	bool endIsConePoint = radius2 == 0.0;

	//if both ends have a radius of 0.0, exit now.
	if ( startIsConePoint && endIsConePoint )
	{
		return;
	}

	int startSegment = startIsConePoint  ?  1 : 0;
	int endSegment = endIsConePoint  ?  lengthSegments - 1  :  lengthSegments;

	int lengthSegsCreated = endIsConePoint - startSegment;



	//compute the total number of vertices
	int numBodyVertices = angularSegments  *  ( lengthSegsCreated + 1 );
	int numVertices = numBodyVertices;
	int numCapVertices = angularSegments  *  ( radialSegments - 1 );
	if ( bCapStart  &&  !startIsConePoint )
	{
		numVertices += numCapVertices;
	}
	if ( bCapEnd  &&  !endIsConePoint  )
	{
		numVertices += numCapVertices;
	}
	if ( startIsConePoint )
	{
		numVertices++;
	}
	if ( endIsConePoint )
	{
		numVertices++;
	}

	//compute the total number of faces
	int numFaces = angularSegments * lengthSegments;
	int numCapFaces = angularSegments  *  ( radialSegments - 1 )  +  1;
	if ( bCapStart  &&  !startIsConePoint )
	{
		numFaces += numCapFaces;
	}
	if ( bCapEnd  &&  !endIsConePoint  )
	{
		numFaces += numCapFaces;
	}

	mesh->reserveVertices( numVertices );
	mesh->reserveFaces( numFaces );



	//compute various interpolated values
	double theta, thetaInc = 2.0 * M_PI / (double)angularSegments;
	double z = -length * 0.5, zInc = length / (double)lengthSegments;
	double u, uInc = 1.0 / (double)angularSegments;
	double v = 0.0, vInc = 1.0 / (double)lengthSegments;
	double radius = radius1;
	double radiusInc = ( radius2 - radius1 ) / (double)lengthSegments;


	if ( startIsConePoint )
	{
		z += zInc;
		v += vInc;
		radius += radiusInc;
	}


	Array<Point2f> tex;
	tex.reserve( numFaces * 4 );


	Array<int> vertexIndices;
	vertexIndices.reserve( numBodyVertices );

	//generate vertices and texture co-ordinates for the main body
	for (int j = startSegment; j <= endSegment; j++)
	{
		theta = 0.0;
		u = 0.0;
		for (int i = 0; i < angularSegments; i++)
		{
			int index = mesh->addVertex( Point3()  +  axes.i * cos( theta ) * radius  +  axes.j * sin( theta ) * radius  +  axes.k * z );
			vertexIndices.push_back( index );
			meshVertices->vertexIndices.push_back( index );
			theta += thetaInc;
		}
		for (int i = 0; i <= angularSegments; i++)
		{
			tex.push_back( Point2f( u, v ) );
			u += uInc;
		}
		z += zInc;
		v += vInc;
		radius += radiusInc;
	}

	//generate a list of quads to make the faces
	int firstRingIndex = 0;
	int lastRingIndex = ( endSegment - startSegment )  *  angularSegments;

	int lengthQuadSegments = lengthSegments;
	if ( startIsConePoint )
	{
		lengthQuadSegments--;
	}
	if ( endIsConePoint )
	{
		lengthQuadSegments--;
	}

	Array<int> faceQuads, texQuads;
	meshGenerateQuadMesh( faceQuads, angularSegments, lengthQuadSegments, true, false );
	meshGenerateQuadMesh( texQuads, angularSegments, lengthQuadSegments, false, false );

	//add the faces
	for (int i = 0; i < faceQuads.size(); i += 4)
	{
		mesh->addFace( vertexIndices[ faceQuads[i] ], tex[ texQuads[i] ],
						vertexIndices[ faceQuads[i+1] ], tex[ texQuads[i+1] ],
						vertexIndices[ faceQuads[i+2] ], tex[ texQuads[i+2] ],
						vertexIndices[ faceQuads[i+3] ], tex[ texQuads[i+3] ] );
	}


	//DEAL WITH THE ENDS:
	//
	//start:
	if ( startIsConePoint )
	{
		//cone point: generate the cone shaped triangle fan at the end
		int vertexIndex = mesh->addVertex( Point3() + axes.k * -length * 0.5 );
		generateConePoint( mesh, vertexIndices, firstRingIndex, vertexIndex, true, vInc, 0.0 );
		meshVertices->vertexIndices.push_back( vertexIndex );
	}
	else if ( bCapStart )
	{
		generateCap( mesh, vertexIndices, firstRingIndex, true, axes.k * length * -0.5, axes, radius1, meshVertices );

		//sharpen the edges
		Array<int> sharpEdges;
		sharpEdges.reserve( angularSegments );
		for (int i = 0; i < angularSegments; i++)
		{
			sharpEdges.push_back( firstRingIndex + i );
		}
		mesh->setEdgesAsNormalSharp( sharpEdges, 0, true );
	}

	if ( endIsConePoint )
	{
		//cone point: generate the cone shaped triangle fan at the end
		int vertexIndex = mesh->addVertex( Point3() + axes.k * length * 0.5 );
		generateConePoint( mesh, vertexIndices, lastRingIndex, vertexIndex, false, 1.0 - vInc, 1.0 );
		meshVertices->vertexIndices.push_back( vertexIndex );
	}
	else if ( bCapEnd )
	{
		//capped: generate the flat, circular triangular fan to cap the start
		generateCap( mesh, vertexIndices, lastRingIndex, false, axes.k * length * 0.5, axes, radius2, meshVertices );

		//sharpen the edges
		Array<int> sharpEdges;
		sharpEdges.reserve( angularSegments );
		for (int i = 0; i < angularSegments; i++)
		{
			sharpEdges.push_back( lastRingIndex + i );
		}
		mesh->setEdgesAsNormalSharp( sharpEdges, 0, true );
	}


	mesh->finalise();
}


void CylinderPrimitive::generateConePoint(GSProductMesh *mesh, const Array<int> &vertexIndices, int vertexRingOffset, int centreVertex,
									bool invert, double ringTexV, double pointTexV)
{
	//generate a triangle fan for the end of the cone
	Point2f texA = Point2f( 0.0, ringTexV );
	double uInc = 1.0 / (double)angularSegments, u = uInc * 0.5;

	for (int i = 0; i < angularSegments; i++)
	{
		int j = nextIndex( i, angularSegments );
		Point2f texB = texA + Vector2f( uInc, 0.0 );

		int a = vertexIndices[ i + vertexRingOffset ];
		int b = vertexIndices[ j + vertexRingOffset ];


		if ( invert )
		{
			mesh->addFace( b, texB, a, texA, centreVertex, Point2f( u, pointTexV) );
		}
		else
		{
			mesh->addFace( a, texA, b, texB, centreVertex, Point2f( u, pointTexV) );
		}

		texA = texB;
		u += uInc;
	}
}

void CylinderPrimitive::generateCap(GSProductMesh *mesh, const Array<int> &vertexIndices, int vertexRingOffset, bool invert,
							const Point3 &position, const Axes3 &axes, double radius, MeshVertexList *meshVertices)
{
	if ( radialSegments == 1 )
	{
		generateCapPolygon( mesh, vertexIndices, vertexRingOffset, invert, 1.0 );
	}
	else
	{
		Array<int> capVertexIndices;
		Array<Point2f> capTexCoords;
		capVertexIndices.reserve( radialSegments * angularSegments );
		capTexCoords.reserve( radialSegments * angularSegments );

		double radInc = radius / (double)radialSegments;
		double rad = radInc;
		double thetaInc = 2.0 * M_PI / (double)angularSegments;
		double textureDiameterInc = 1.0 / (double)radialSegments;
		double textureDiameter = textureDiameterInc;

		for (int i = 1; i < radialSegments; i++)
		{
			double theta = 0.0;

			for (int j = 0; j < angularSegments; j++)
			{
				double cosTheta = cos( theta );
				double sinTheta = sin( theta );
				int index = mesh->addVertex( position  +  axes.i * cosTheta * rad  +  axes.j * sinTheta * rad );
				capVertexIndices.push_back( index );
				meshVertices->vertexIndices.push_back( index );

				Point2f tex( 0.5  +  cosTheta * 0.5 * textureDiameter, 0.5  +  sinTheta * 0.5 * textureDiameter );
				capTexCoords.push_back( tex );

				theta += thetaInc;
			}

			rad += radInc;
			textureDiameter += textureDiameterInc;
		}

		capVertexIndices.extend( vertexIndices, vertexRingOffset, vertexRingOffset + angularSegments );
		double theta = 0.0;
		for (int j = 0; j < angularSegments; j++)
		{
			Point2f tex( 0.5  +  cos( theta ) * 0.5, 0.5  +  sin( theta ) * 0.5 );
			capTexCoords.push_back( tex );

			theta += thetaInc;
		}


		generateCapPolygon( mesh, capVertexIndices, 0, invert, textureDiameterInc );


		Array<int> faceQuads;
		meshGenerateQuadMesh( faceQuads, angularSegments, radialSegments - 1, true, false );

		//add the faces
		for (int i = 0; i < faceQuads.size(); i += 4)
		{
			if ( invert )
			{
				mesh->addFace( capVertexIndices[ faceQuads[i] ], capTexCoords[ faceQuads[i] ],
								capVertexIndices[ faceQuads[i+1] ], capTexCoords[ faceQuads[i+1] ],
								capVertexIndices[ faceQuads[i+2] ], capTexCoords[ faceQuads[i+2] ],
								capVertexIndices[ faceQuads[i+3] ], capTexCoords[ faceQuads[i+3] ] );
			}
			else
			{
				mesh->addFace( capVertexIndices[ faceQuads[i+3] ], capTexCoords[ faceQuads[i+3] ],
								capVertexIndices[ faceQuads[i+2] ], capTexCoords[ faceQuads[i+2] ],
								capVertexIndices[ faceQuads[i+1] ], capTexCoords[ faceQuads[i+1] ],
								capVertexIndices[ faceQuads[i] ], capTexCoords[ faceQuads[i] ] );
			}
		}
	}
}

void CylinderPrimitive::generateCapPolygon(GSProductMesh *mesh, const Array<int> &vertexIndices, int offset, bool invert, double textureDiameter)
{
	//generate a single polygon
	Array<int> faceVertexIndices;
	Array<Point2f> texCoords;

	//generate a list of vertices for the face
	faceVertexIndices.extend( vertexIndices, offset, offset + angularSegments );

	//generate texture co-ordinates
	texCoords.reserve( angularSegments );

	double thetaInc = 2.0 * M_PI / (double)angularSegments;
	double theta = 0.0;
	for (int i = 0; i < angularSegments; i++)
	{
		Point2f tex( 0.5  +  cos( theta ) * 0.5 * textureDiameter, 0.5  +  sin( theta ) * 0.5 * textureDiameter );
		texCoords.push_back( tex );

		theta += thetaInc;
	}

	//invert if necessary
	if ( invert )
	{
		faceVertexIndices.reverse();
		texCoords.reverse();
	}

	//add the face
	mesh->addFace( faceVertexIndices, texCoords );
}






TorusPrimitive::TorusPrimitive(Axis axis, int majorSegments, int minorSegments, double majorRadius, double minorRadius)
						: axis( axis ), majorSegments( majorSegments ), minorSegments( minorSegments ), majorRadius( majorRadius ), minorRadius( minorRadius )
{
}


void TorusPrimitive::createTorus(GSProductMesh *mesh, MeshVertexList *meshVertices)
{
	Axes3 axes = Axes3( axis );



	double majAng, majInc = 2.0 * M_PI / (double)majorSegments;
	double minAng, minInc = -2.0 * M_PI / (double)minorSegments;


	mesh->reserveVertices( majorSegments * minorSegments );
	mesh->reserveFaces( majorSegments * minorSegments );




	majAng = 0.0;
	for (int i = 0; i < majorSegments; i++)
	{
		double x, y, z, r;
		double cosMaj = cos( majAng );
		double sinMaj = sin( majAng );

		minAng = 0.0;
		for (int j = 0; j < minorSegments; j++)
		{
			z = minorRadius * sin( minAng );
			r = minorRadius * cos( minAng );

			x = ( r + majorRadius )  *  cosMaj;
			y = ( r + majorRadius )  *  sinMaj;

			int index = mesh->addVertex( Point3()  +  axes.i * x  +  axes.j * y  +  axes.k * z );
			meshVertices->vertexIndices.push_back( index );


			minAng += minInc;
		}

		majAng += majInc;
	}


	double u = 0.0, uInc = 1.0 / (double)minorSegments;
	double v = 0.0, vInc = 1.0 / (double)majorSegments;
	Array<Point2f> tex;
	tex.reserve( ( majorSegments + 1 ) * ( minorSegments + 1 ) );

	for (int i = 0; i <= majorSegments; i++)
	{
		u = 0.0;

		for (int j = 0; j <= minorSegments; j++)
		{
			tex.push_back( Point2f( u, v ) );
			u += uInc;
		}

		v += vInc;
	}

	Array<int> faceQuads;
	Array<int> texQuads;

	meshGenerateQuadMesh( faceQuads, minorSegments, majorSegments, true, true );
	meshGenerateQuadMesh( texQuads, minorSegments, majorSegments, false, false );

	for (int i = 0; i < faceQuads.size(); i += 4)
	{
		mesh->addFace( faceQuads[i], tex[ texQuads[i] ],
						faceQuads[i+1], tex[ texQuads[i+1] ],
						faceQuads[i+2], tex[ texQuads[i+2] ],
						faceQuads[i+3], tex[ texQuads[i+3] ] );
	}


	mesh->finalise();
}






TubePrimitive::TubePrimitive(Axis axis, int radialSegments, int lengthSegments, int slices, double innerRadius, double outerRadius, double length,
						bool bInnerSection, bool bOuterSection, bool bStartCap, bool bEndCap, CapMaterialMapping capMapping)
						: axis( axis ), radialSegments( radialSegments ), lengthSegments( lengthSegments ), slices( slices ),
						innerRadius( innerRadius ), outerRadius( outerRadius ), length( length ),
						bInnerSection( bInnerSection ), bOuterSection( bOuterSection ), bStartCap( bStartCap ), bEndCap( bEndCap ), capMapping( capMapping )
{
}


void TubePrimitive::createTube(GSProductMesh *mesh, MeshVertexList *meshVertices)
{
	int numVertices = 0, numFaces = 0, numIndices = 0;
	Array<int> vertexIndices;


	Axes3 axes = Axes3( axis );



	//COMPUTE NUMBER OF VERTICES, FACES, AND INDICES
	int sectionCount = bOuterSection ? 1 : 0;
	if ( bInnerSection )
	{
		sectionCount++;
	}
	if ( sectionCount > 0 )
	{
		numVertices += slices  *  ( lengthSegments - 1 )  *  sectionCount;
		numFaces += slices * lengthSegments  *  sectionCount;
		numIndices += ( slices + 1 )  *  ( lengthSegments - 1 )  *  sectionCount;
	}

	int capCount = bStartCap ? 1 : 0;
	if ( bEndCap )
	{
		capCount++;
	}
	if ( capCount > 0 )
	{
		numVertices += slices  *  ( radialSegments - 1 )  *  capCount;
		numFaces += slices * radialSegments  *  capCount;
		numIndices += ( slices + 1 )  *  ( radialSegments - 1 )  *  capCount;
	}

	int cornersCount = 0;
	if ( bStartCap || bOuterSection )
	{
		cornersCount++;
	}
	if ( bOuterSection || bEndCap )
	{
		cornersCount++;
	}
	if ( bEndCap || bInnerSection )
	{
		cornersCount++;
	}
	if ( bInnerSection || bStartCap )
	{
		cornersCount++;
	}

	if ( cornersCount > 0 )
	{
		numVertices += slices  *  cornersCount;
		numIndices += ( slices + 1 )  *  cornersCount;
	}


	//reserve space
	mesh->reserveVertices( numVertices );
	mesh->reserveFaces( numFaces );

	vertexIndices.reserve( numIndices );




	//CREATE VERTICES
	int innerSectionBegin = -1, outerSectionBegin = -1, startCapBegin = -1, endCapBegin = -1;


	if ( bStartCap || bOuterSection )
	{
		outerSectionBegin = 0;

		Point3 centre = axes.k * length * -0.5;
		createVertexRing( mesh, vertexIndices, centre, outerRadius, axes.i, axes.j, meshVertices );
	}

	if ( bOuterSection )
	{
		Point3 centre = axes.k * length * -0.5;

		createSectionVertices( mesh, vertexIndices, centre, centre  +  axes.k * length, outerRadius, outerRadius, axes.i, axes.j, lengthSegments, meshVertices );
	}

	if ( bOuterSection || bEndCap )
	{
		endCapBegin = vertexIndices.size();

		Point3 centre = axes.k * length * 0.5;
		createVertexRing( mesh, vertexIndices, centre, outerRadius, axes.i, axes.j, meshVertices );
	}

	if ( bEndCap )
	{
		Point3 centre = axes.k * length * 0.5;

		createSectionVertices( mesh, vertexIndices, centre, centre, outerRadius, innerRadius, axes.i, axes.j, radialSegments, meshVertices );
	}

	if ( bEndCap || bInnerSection )
	{
		innerSectionBegin = vertexIndices.size();

		Point3 centre = axes.k * length * 0.5;
		createVertexRing( mesh, vertexIndices, centre, innerRadius, axes.i, axes.j, meshVertices );
	}

	if ( bInnerSection )
	{
		Point3 centre = axes.k * length * 0.5;

		createSectionVertices( mesh, vertexIndices, centre, centre  -  axes.k * length, innerRadius, innerRadius, axes.i, axes.j, lengthSegments, meshVertices );
	}

	if ( bInnerSection || bStartCap )
	{
		startCapBegin = vertexIndices.size();

		Point3 centre = axes.k * length * -0.5;
		createVertexRing( mesh, vertexIndices, centre, innerRadius, axes.i, axes.j, meshVertices );
	}

	if ( bStartCap )
	{
		Point3 centre = axes.k * length * -0.5;

		createSectionVertices( mesh, vertexIndices, centre, centre, innerRadius, outerRadius, axes.i, axes.j, radialSegments, meshVertices );
	}

	//repeat the first row of vertices
	if ( bStartCap || bOuterSection )
	{
		for (int i = 0; i <= slices; i++)
		{
			int index = vertexIndices[i];
			vertexIndices.push_back( index );
		}
	}



	//CREATE FACES
	double innerTexRadius = innerRadius / outerRadius;

	if ( bOuterSection )
	{
		createSectionFaces( mesh, vertexIndices, outerSectionBegin, false );
	}

	if ( bEndCap )
	{
		createCapFaces( mesh, vertexIndices, endCapBegin, 1.0, innerTexRadius );
	}

	if ( bInnerSection )
	{
		createSectionFaces( mesh, vertexIndices, innerSectionBegin, true );
	}

	if ( bStartCap )
	{
		createCapFaces( mesh, vertexIndices, startCapBegin, innerTexRadius, 1.0 );
	}


	//normal sharpen edges
	int sectionsBegin[4] = { outerSectionBegin, endCapBegin, innerSectionBegin, startCapBegin };

	Array<int> sharpenIndices;
	sharpenIndices.reserve( slices + 1 );

	for (int s = 0; s < 4; s++)
	{
		if ( sectionsBegin[s] != -1 )
		{
			sharpenIndices.clear();
			for (int i = 0; i < slices; i++)
			{
				sharpenIndices.push_back( vertexIndices[ sectionsBegin[s] + i ] );
			}
			sharpenIndices.push_back( vertexIndices[ sectionsBegin[s] + slices ] );

			mesh->setEdgesAsNormalSharp( vertexIndices, 0, false );
		}
	}


	mesh->finalise();
}



void TubePrimitive::createVertexRing(GSProductMesh *mesh, Array<int> &vertexIndices, const Point3 &centre, double radius,
								const Vector3 &vi, const Vector3 &vj, MeshVertexList *meshVertices)
{
	double thetaInc = ( 2.0 * M_PI ) / (double)slices, theta = 0.0;

	int firstIndexPosition = vertexIndices.size();
	for (int i = 0; i < slices; i++)
	{
		Point3 p = centre  +  ( vi * cos( theta )  +  vj * sin( theta ) )  *  radius;

		int index = mesh->addVertex( p );
		vertexIndices.push_back( index );
		meshVertices->vertexIndices.push_back( index );

		theta += thetaInc;
	}

	int firstIndex = vertexIndices[firstIndexPosition];
	vertexIndices.push_back( firstIndex );
}

void TubePrimitive::createSectionVertices(GSProductMesh *mesh, Array<int> &vertexIndices, const Point3 &centreA, const Point3 &centreB,
									double radiusA, double radiusB, const Vector3 &vi, const Vector3 &vj, int numSegments,
									MeshVertexList *meshVertices)
{
	Vector3 centreInc = ( centreB - centreA ) * ( 1.0 / (double)numSegments );
	double radiusInc = ( radiusB - radiusA ) * ( 1.0 / (double)numSegments );

	Point3 centre = centreA + centreInc;
	double radius = radiusA + radiusInc;

	for (int i = 1; i < numSegments; i++)
	{
		createVertexRing( mesh, vertexIndices, centre, radius, vi, vj, meshVertices );
		centre += centreInc;
		radius += radiusInc;
	}
}

void TubePrimitive::createSectionFaces(GSProductMesh *mesh, const Array<int> &vertexIndices, int beginAt, bool innerSection)
{
	double uInc = 1.0 / (double)slices;
	double vInc = 1.0 / (double)lengthSegments, v0 = 0.0, v1 = vInc;

	if ( innerSection )
	{
		vInc = -vInc;
		v0 = 1.0;
		v1 = 1.0 + vInc;
	}

	int width = slices + 1;

	for (int y = 0; y < lengthSegments; y++)
	{
		double u0 = 0.0, u1 = uInc;
		for (int x = 0; x < slices; x++)
		{
			int a = y * width + x;
			int b = a + 1;
			int c = b + width;
			int d = a + width;

			mesh->addFace( vertexIndices[ beginAt + a ], Point2f( u0, v0 ),
							vertexIndices[ beginAt + b ], Point2f( u1, v0 ),
							vertexIndices[ beginAt + c ], Point2f( u1, v1 ),
							vertexIndices[ beginAt + d ], Point2f( u0, v1 ) );

			u0 = u1;
			u1 += uInc;
		}

		v0 = v1;
		v1 += vInc;
	}
}

void TubePrimitive::createCapFaces(GSProductMesh *mesh, const Array<int> &vertexIndices, int beginAt, double texRadiusA, double texRadiusB)
{
	Array<Point2f> texCoords;


	//compute cap tex coords
	texCoords.reserve( ( radialSegments + 1 )  *  ( slices + 1 ) );


	if ( capMapping == CAPMATERIALMAPPING_PLANAR )
	{
		Array<Point2f> ringTexCoords;

		//compute ring texture coords
		ringTexCoords.reserve( slices + 1 );
		double theta = 0.0, thetaInc = 2.0 * M_PI / (double)slices;
		for (int i = 0; i < slices; i++)
		{
			ringTexCoords.push_back( Point2f( cos( theta ), sin( theta ) ) );
			theta += thetaInc;
		}
		ringTexCoords.push_back( Point2f( 1.0, 0.0 ) );


		double texRadius = texRadiusA;
		double texRadiusInc = ( texRadiusB - texRadiusA ) / (double)radialSegments;

		for (int y = 0; y <= radialSegments; y++)
		{
			for (int x = 0; x <= slices; x++)
			{
				//scale by 0.5 and centre around 0.5
				texCoords.push_back( Point2f::mul( ringTexCoords[x], texRadius * 0.5 )  +  Vector2f( 0.5, 0.5 ) );
			}
			texRadius += texRadiusInc;
		}
	}
	else if ( capMapping == CAPMATERIALMAPPING_POLAR )
	{
		double v = 0.0, vInc = 1.0 / (double)radialSegments;

		if ( texRadiusB < texRadiusA )
		{
			//reverse direction
			v = 1.0;
			vInc = -vInc;
		}

		for (int y = 0; y <= radialSegments; y++)
		{
			double u = 0.0, uInc = 1.0 / (double)slices;
			for (int x = 0; x <= slices; x++)
			{
				texCoords.push_back( Point2f( u, v ) );

				u += uInc;
			}

			v += vInc;
		}
	}


	//create faces
	int width = slices + 1;

	for (int y = 0; y < radialSegments; y++)
	{
		for (int x = 0; x < slices; x++)
		{
			int a = y * width + x;
			int b = a + 1;
			int c = b + width;
			int d = a + width;

			mesh->addFace( vertexIndices[ beginAt + a ], texCoords[a],
							vertexIndices[ beginAt + b ], texCoords[b],
							vertexIndices[ beginAt + c ], texCoords[c],
							vertexIndices[ beginAt + d ], texCoords[d] );
		}
	}
}



#endif
