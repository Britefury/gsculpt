//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYLINE_CPP__
#define POLYLINE_CPP__

#include <algorithm>

#include <Math/index.h>

#include <Math/Spline.h>

#include <Polyline/Polyline.h>




/*
*******************************************************************************
										PolylineEdgeTexture
*******************************************************************************
*/

PolylineEdgeTexture::PolylineEdgeTexture()
{
	textureID = -1;
	u0 = u1 = 0.0;
}

PolylineEdgeTexture::PolylineEdgeTexture(int tid, double u0, double u1)
{
	textureID = tid;
	this->u0 = u0;
	this->u1 = u1;
}

int PolylineEdgeTexture::getTextureID() const
{
	return textureID;
}

double PolylineEdgeTexture::getU0() const
{
	return u0;
}

double PolylineEdgeTexture::getU1() const
{
	return u1;
}




/*
*******************************************************************************
									PolylineEdgeTextureTable
*******************************************************************************
*/

PolylineEdgeTextureTable::PolylineEdgeTextureTable()
{
	numTextures = 0;
}


const PolylineEdgeTexture &PolylineEdgeTextureTable::operator[](int i) const
{
	return table[i];
}

int PolylineEdgeTextureTable::getNumTextures() const
{
	return numTextures;
}


void PolylineEdgeTextureTable::reserve(int s)
{
	table.reserve( s );
}

void PolylineEdgeTextureTable::add(const PolylineEdgeTexture &t)
{
	table.push_back( t );
	numTextures = std::max( numTextures, t.getTextureID() + 1 );
}

void PolylineEdgeTextureTable::rotateBackward(int n)
{
	table.rotateBackward( n );
}




/*
*******************************************************************************
										Polyline
*******************************************************************************
*/

Polyline::Polyline()
{
	closedFlag = false;
	markedVertexCount = 0;
}


void Polyline::read(FILE *f)
{
	clear();

	int numVertices;
	fread( &numVertices, sizeof(int), 1, f );
	vertices.reserve( numVertices );

	for (int i = 0; i < numVertices; i++)
	{
		PVertex vertex;

		vertex.read( f );
		vertices.push_back( vertex );

		if ( vertex.isMarked() )
		{
			markedVertexCount++;
		}
	}

	fread( &closedFlag, sizeof(bool), 1, f );
}

void Polyline::write(FILE *f) const
{
	int numVertices = size();
	fwrite( &numVertices, sizeof(int), 1, f );

	for (int i = 0; i < size(); i++)
	{
		vertices[i].write( f );
	}

	fwrite( &closedFlag, sizeof(bool), 1, f );
}



void Polyline::copyFrom(const Polyline &p)
{
	vertices.resize( p.vertices.size() );

	for (int i = 0; i < p.vertices.size(); i++)
	{
		vertices[i].copyFrom( p.vertices[i] );
		if ( vertices[i].isMarked() )
		{
			markedVertexCount++;
		}
	}

	closedFlag = p.closedFlag;
}


int Polyline::computeMemoryUsage() const
{
	return vertices.computeMemoryUsage();
}



int Polyline::getNumVertices() const
{
	return vertices.size();
}

int Polyline::getNumEdges() const
{
	if ( closedFlag )
	{
		if ( vertices.size() <= 1 )
		{
			return 0;
		}
		else
		{
			return vertices.size();
		}
	}
	else
	{
		return std::max( vertices.size() - 1, 0 );
	}
}


int Polyline::size() const
{
	return vertices.size();
}

const PVertex& Polyline::operator[](int i) const
{
	return vertices[i];
}

const PVertex& Polyline::at(int i) const
{
	return vertices[i];
}



int Polyline::addVertex(const PVertex &v)
{
	if ( v.isMarked() )
	{
		markedVertexCount++;
	}

	return vertices.push_back( v );
}

int Polyline::addVertex(const Point3 &location, bool marked)
{
	int index = vertices.push_back( PVertex( location, marked ) );

	if ( marked )
	{
		markedVertexCount++;
	}

	return index;
}

int Polyline::addVertexAt(int position, const PVertex &v)
{
	if ( v.isMarked() )
	{
		markedVertexCount++;
	}

	vertices.insert( position, v );
	return position;
}

int Polyline::addVertexAt(int position, const Point3 &location, bool marked)
{
	vertices.insert( position, PVertex( location, marked ) );

	if ( marked )
	{
		markedVertexCount++;
	}

	return position;
}

void Polyline::removeVertex(PVertex *vtx)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( &vertices[i] == vtx )
		{
			if ( vtx->isMarked() )
			{
				markedVertexCount--;
			}

			vertices.remove( i );
			return;
		}
	}
}

void Polyline::swapVertices(PVertex *a, PVertex *b)
{
	PVertex swap = *a;
	*a = *b;
	*b = swap;
}

void Polyline::copyVertexPropertiesFrom(int destIndex, const PVertex &srcVertex)
{
	PVertex &destVertex = vertices[destIndex];
	bool wasMarked = destVertex.isMarked();
	destVertex.copyPropertiesFrom( srcVertex );
	bool isMarked = destVertex.isMarked();

	if ( wasMarked  &&  !isMarked )
	{
		markedVertexCount--;
	}
	else if ( !wasMarked  &&  isMarked )
	{
		markedVertexCount++;
	}
}



void Polyline::removeMarkedVertices()
{
	for (int i = vertices.size() - 1; i >= 0; i--)
	{
		if ( vertices[i].isMarked() )
		{
			vertices.remove( i );
		}
	}

	markedVertexCount = 0;
}


void Polyline::flip()
{
	vertices.reverse();
}


void Polyline::reserveVertices(int s)
{
	vertices.reserve( s );
}

void Polyline::clear()
{
	vertices.clear();
	markedVertexCount = 0;
}



void Polyline::rotateForward(int positions)
{
	vertices.rotateForward( positions );
}

void Polyline::rotateBackward(int positions)
{
	vertices.rotateBackward( positions );
}



void Polyline::close()
{
	closedFlag = true;
}

void Polyline::open()
{
	closedFlag = false;
}

void Polyline::setClosed(bool c)
{
	closedFlag = c;
}

bool Polyline::isClosed() const
{
	return closedFlag;
}




void Polyline::transform(const Matrix4 &m, bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		PVertex &vertex = vertices[i];

		if ( vertex.isMarked() || !markedOnly )
		{
			vertex.transform( m );
		}
	}
}

void Polyline::transform(const Transformation *x, bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		PVertex &vertex = vertices[i];

		if ( vertex.isMarked() || !markedOnly )
		{
			vertex.transform( x );
		}
	}
}

void Polyline::savePositions(bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		PVertex &vertex = vertices[i];

		if ( vertex.isMarked()  ||  !markedOnly )
		{
			vertex.savePosition();
		}
	}
}

void Polyline::restorePositions(bool markedOnly)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		PVertex &vertex = vertices[i];

		if ( vertex.isMarked()  ||  !markedOnly )
		{
			vertex.restorePosition();
		}
	}
}



void Polyline::mark(MarkPredicate predicate, const Region3d *region)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		PVertex &vertex = vertices[i];

		markedVertexCount += vertex.mark( predicate, region );
	}
}

void Polyline::markAll(MarkPredicate predicate)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		PVertex &vertex = vertices[i];

		vertex.mark( predicate );
	}

	markedVertexCount = vertices.size();
}

void Polyline::saveMark()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices[i].saveMark();
	}
}

void Polyline::restoreMark()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		markedVertexCount += vertices[i].restoreMark();
	}
}

MarkStatus Polyline::getMarkStatus() const
{
	if ( markedVertexCount == 0 )
	{
		return MARKSTATUS_NONE;
	}
	if ( markedVertexCount == vertices.size() )
	{
		return MARKSTATUS_ALL;
	}
	else
	{
		return MARKSTATUS_SOME;
	}
}

Side Polyline::areVerticesInside(const Region3d *region) const
{
	Side side = SIDE_NEGATIVE;

	if ( vertices.size() > 0 )
	{
		side = vertices[0].isInside( region )  ?  SIDE_POSITIVE  :  SIDE_NEGATIVE;

		for (int i = 0; i < vertices.size(); i++)
		{
			Side s = vertices[i].isInside( region )  ?  SIDE_POSITIVE  :  SIDE_NEGATIVE;

			if ( s != side )
			{
				return SIDE_BOTH;
			}
		}
	}

	return side;
}



BBox3 Polyline::getBBox(bool markedOnly) const
{
	bool foundFirstVertex = false;
	BBox3 box;

	for (int i = 0; i < size(); i++)
	{
		const PVertex &vertex = vertices[i];

		if ( vertex.isMarked() || !markedOnly )
		{
			if ( !foundFirstVertex )
			{
				box = BBox3( vertex.getPosition() );
				foundFirstVertex = true;
			}
			else
			{
				box.addPoint( vertex.getPosition() );
			}
		}
	}

	return box;
}

BBox3 Polyline::getBBox(const Matrix4 &m, bool markedOnly) const
{
	bool foundFirstVertex = false;
	BBox3 box;

	for (int i = 0; i < (int)size(); i++)
	{
		const PVertex &vertex = vertices[i];

		if ( vertex.isMarked() || !markedOnly )
		{
			if ( !foundFirstVertex )
			{
				box = BBox3( vertex.getPosition() * m );
				foundFirstVertex = true;
			}
			else
			{
				box.addPoint( vertex.getPosition() * m );
			}
		}
	}

	return box;
}

BBox3 Polyline::getBBox(const Transformation *x, bool markedOnly) const
{
	bool foundFirstVertex = false;
	BBox3 box;

	for (int i = 0; i < (int)size(); i++)
	{
		const PVertex &vertex = vertices[i];

		if ( vertex.isMarked() || !markedOnly )
		{
			if ( !foundFirstVertex )
			{
				box = BBox3( x->transformPoint( vertex.getPosition() ) );
				foundFirstVertex = true;
			}
			else
			{
				box.addPoint( x->transformPoint( vertex.getPosition() ) );
			}
		}
	}

	return box;
}

BBox3 Polyline::getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const
{
	bool foundFirstVertex = false;
	BBox3 box;

	for (int i = 0; i < (int)size(); i++)
	{
		const PVertex &vertex = vertices[i];

		if ( vertex.isMarked() || !markedOnly )
		{
			if ( !foundFirstVertex )
			{
				box = BBox3( x->transformPoint( vertex.getPosition() * m ) );
				foundFirstVertex = true;
			}
			else
			{
				box.addPoint( x->transformPoint( vertex.getPosition() * m ) );
			}
		}
	}

	return box;
}




void Polyline::transformVertices(const Array<int> &vertexIndices, const Matrix4 &xform)
{
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		vertices[ vertexIndices[i] ].transform( xform );
	}
}

void Polyline::saveVertexPositions(const Array<int> &vertexIndices)
{
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		vertices[ vertexIndices[i] ].savePosition();
	}
}

void Polyline::restoreVertexPositions(const Array<int> &vertexIndices)
{
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		vertices[ vertexIndices[i] ].restorePosition();
	}
}





void Polyline::mirror(const Plane &mirrorPlane, bool reverseOrder, bool markedOnly)
{
	Matrix4 reflectionMatrix = Matrix4::reflect( mirrorPlane );


	//need to store number of vertices, as the size of the vertex Array will
	//change as vertices are added
	int numVertices = getNumVertices();
	if ( reverseOrder )
	{
		for (int i = numVertices - 1; i >= 0; i--)
		{
			if ( vertices[i].isMarked()  ||  !markedOnly )
			{
				PVertex v = vertices[i];
				v.setPosition( v.getPosition() * reflectionMatrix );
				addVertex( v );
			}
		}
	}
	else
	{
		for (int i = 0; i < numVertices; i++)
		{
			if ( vertices[i].isMarked()  ||  !markedOnly )
			{
				PVertex v = vertices[i];
				v.setPosition( v.getPosition() * reflectionMatrix );
				addVertex( v );
			}
		}
	}
}

void Polyline::interleave(int markedStride, int unmarkedStride)
{
	Array<int> markedVertexIndices, unmarkedVertexIndices;

	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].isMarked() )
		{
			markedVertexIndices.push_back( i );
		}
		else
		{
			unmarkedVertexIndices.push_back( i );
		}
	}


	//take a copy of vertices
	Array<PVertex> oldVertices = vertices;
	//clear the vertex Array
	clear();


	//perform the interleaving
	int i, j;
	for (i = 0, j = 0;
		  i <= ( markedVertexIndices.size() - markedStride )  &&
		  j <= ( unmarkedVertexIndices.size() - unmarkedStride );
		  i += markedStride, j += unmarkedStride)
	{
		//copy @markedStride vertices from the marked set
		for (int k = 0; k < markedStride; k++)
		{
			addVertex( oldVertices[ markedVertexIndices[ i + k ] ] );
		}
		//copy @stride vertices from the unmarked set
		for (int k = 0; k < unmarkedStride; k++)
		{
			addVertex( oldVertices[ unmarkedVertexIndices[ j + k ] ] );
		}
	}

	//copy over any remaining marked vertices
	for (; i < markedVertexIndices.size(); i++)
	{
		addVertex( oldVertices[ markedVertexIndices[i] ] );
	}
	//copy over any remaining unmarked vertices
	for (; j < unmarkedVertexIndices.size(); j++)
	{
		addVertex( oldVertices[ unmarkedVertexIndices[j] ] );
	}
}



void Polyline::subdivide(int numToInsert, bool markedFlag, PSubdivCurveType curveType, bool betweenMarkedVerticesOnlyFlag)
{
	//take a copy of the old vertex list
	Array<PVertex> oldVertices = vertices;


	//clear the vertex list
	clear();

	for (int i = 0; i < oldVertices.size() - 1; i++)
	{
		int j = i + 1;

		vertices.push_back( oldVertices[i] );

		if ( ( oldVertices[i].isMarked()  &&  oldVertices[j].isMarked() )  ||  !betweenMarkedVerticesOnlyFlag )
		{
			subdivideEdge( i, i + 1, oldVertices, numToInsert, markedFlag, curveType, betweenMarkedVerticesOnlyFlag );
		}
	}

	vertices.push_back( oldVertices.back() );
	if ( isClosed() )
	{
		if ( ( oldVertices.back().isMarked()  &&  oldVertices.front().isMarked() )  ||  !betweenMarkedVerticesOnlyFlag )
		{
			subdivideEdge( oldVertices.size() - 1, 0, oldVertices, numToInsert, markedFlag, curveType, betweenMarkedVerticesOnlyFlag );
		}
	}
}

void Polyline::subdivideEdge(int n, int nPlus1, const Array<PVertex> &oldVertices, int numToInsert, bool markedFlag, PSubdivCurveType curveType, bool betweenMarkedVerticesOnlyFlag)
{
	if ( ( oldVertices[n].isMarked()  &&  oldVertices[nPlus1].isMarked() )  ||  !betweenMarkedVerticesOnlyFlag )
	{
		if ( curveType == PSUBDIVCURVETYPE_LINEAR )
		{
			const Point3 &pointAtN = oldVertices[n].getPosition();
			const Point3 &pointAtNPlus1 = oldVertices[nPlus1].getPosition();

			double deltaT = 1.0 / (double)( numToInsert + 1 );
			Vector3 delta = ( pointAtNPlus1 - pointAtN ) * deltaT;

			Point3 position = pointAtN + delta;

			for (int i = 0; i < numToInsert; i++)
			{
				addVertex( PVertex( position, markedFlag ) );

				position += delta;
			}
		}
		else if ( curveType == PSUBDIVCURVETYPE_CATMULLROM )
		{
			int nMinus1 = clampPrevIndex( n, oldVertices.size(), isClosed() );
			int nPlus2 = clampNextIndex( nPlus1, oldVertices.size(), isClosed() );


			if ( betweenMarkedVerticesOnlyFlag )
			{
				// If an outer control vertex is unmarked, then that makes the corresponding inner control vertex a boundary vertex, so use the index of the inner
				// control vertex
				if ( !oldVertices[nMinus1].isMarked() )
				{
					nMinus1 = n;
				}
				if ( !oldVertices[nPlus2].isMarked() )
				{
					nPlus2 = nPlus1;
				}
			}


			Vector3 a = oldVertices[nMinus1].getPosition().toVector3();
			Vector3 b = oldVertices[n].getPosition().toVector3();
			Vector3 c = oldVertices[nPlus1].getPosition().toVector3();
			Vector3 d = oldVertices[nPlus2].getPosition().toVector3();

			SplineBasisMatrix<Vector3> basisMatrix;
			cubicCatmulRomBasis( a, b, c, d, basisMatrix );

			double deltaT = 1.0 / (double)( numToInsert + 1 );
			double t = deltaT;

			for (int i = 0; i < numToInsert; i++)
			{
				double t2 = t * t;
				double t3 = t2 * t;

				Point3 position = basisMatrix.x[0] * t3  +  basisMatrix.x[1] * t2  +  basisMatrix.x[2] * t  +  basisMatrix.x[3];

				addVertex( PVertex( position, markedFlag ) );

				t += deltaT;
			}
		}
	}
	else
	{
		addVertex( oldVertices[n] );
	}
}


void Polyline::normalSharpenMarkedVertices()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].isMarked() )
		{
			vertices[i].setNormalSharp( true );
		}
	}
}

void Polyline::textureBreakMarkedVertices()
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if ( vertices[i].isMarked() )
		{
			vertices[i].setTextureBreak( true );
		}
	}
}






void Polyline::computeCumulativeLengthArray(Array<double> &lengthArray) const
{
	if ( vertices.size() == 0 )
	{
		lengthArray.push_back( 0.0 );
		return;
	}

	//reserve enough space
	lengthArray.reserve( isClosed() ? size() + 1  :  size() );


	//cumulative length is 0.0
	double cumulativeLength = 0.0;

	//add 0.0 to the beginning of the Array
	lengthArray.push_back( 0.0 );

	//accumulate the length, adding it to the end of the Array at each step
	for (int i = 1; i < vertices.size(); i++)
	{
		cumulativeLength += vertices[i-1].getPosition().distanceTo( vertices[i].getPosition() );
		lengthArray.push_back( cumulativeLength );
	}

	if ( isClosed() )
	{
		//add distance from end to beginning of polyline
		cumulativeLength += vertices.back().getPosition().distanceTo( vertices[0].getPosition() );
		lengthArray.push_back( cumulativeLength );
	}
}

void Polyline::computeUnitCumulativeLengthArray(Array<double> &lengthArray) const
{
	if ( vertices.size() == 0 )
	{
		lengthArray.push_back( 0.0 );
		return;
	}

	computeCumulativeLengthArray( lengthArray );

	double oneOverCumulativeLength = 1.0 / lengthArray.back();

	for (int i = 0; i < lengthArray.size(); i++)
	{
		lengthArray[i] *= oneOverCumulativeLength;
	}
}

double Polyline::computeLength() const
{
	if ( vertices.size() == 0 )
	{
		return 0.0;
	}


	//cumulative length is 0.0
	double cumulativeLength = 0.0;

	//accumulate the length, adding it to the end of the Array at each step
	for (int i = 1; i < size(); i++)
	{
		cumulativeLength += vertices[i-1].getPosition().distanceTo( vertices[i].getPosition() );
	}

	if ( isClosed() )
	{
		//add distance from end to beginning of polyline
		cumulativeLength += vertices.back().getPosition().distanceTo( vertices[0].getPosition() );
	}


	return cumulativeLength;
}





void Polyline::flatten(Axis flattenAxis)
{
	switch( flattenAxis )
	{
	case AXIS_NEGATIVE_X:
	case AXIS_X:
		for (int i = 0; i < size(); i++)
		{
			const Point3 &v = vertices[i].getPosition();
			vertices[i].setPosition( Point3( 0.0, v.y, v.z ) );
		}
		break;
	case AXIS_NEGATIVE_Y:
	case AXIS_Y:
		for (int i = 0; i < size(); i++)
		{
			const Point3 &v = vertices[i].getPosition();
			vertices[i].setPosition( Point3( v.x, 0.0, v.z ) );
		}
		break;
	case AXIS_NEGATIVE_Z:
	case AXIS_Z:
		for (int i = 0; i < size(); i++)
		{
			const Point3 &v = vertices[i].getPosition();
			vertices[i].setPosition( Point3( v.x, v.y, 0.0 ) );
		}
		break;
	default:
		break;
	}
}

void Polyline::flatten(Vector3 flattenAxis)
{
	flattenAxis.normalise();

	for (int i = 0; i < size(); i++)
	{
		const Point3 &v = vertices[i].getPosition();
		vertices[i].setPosition( v  -  flattenAxis * v.dot( flattenAxis ) );
	}
}



void Polyline::generatePointList(Array<Point3> &pointList) const
{
	pointList.reserve( size() );
	for (int i = 0; i < size(); i++)
	{
		pointList.push_back( vertices[i].getPosition() );
	}
}

void Polyline::generatePointList(Array<Point2> &pointList, Axis flattenAxis) const
{
	pointList.reserve( size() );
	switch( flattenAxis )
	{
	case AXIS_NEGATIVE_X:
	case AXIS_X:
		for (int i = 0; i < size(); i++)
		{
			const Point3 &vertex = vertices[i].getPosition();
			pointList.push_back( Point2( -vertex.z, vertex.y ) );
		}
		break;
	case AXIS_NEGATIVE_Y:
	case AXIS_Y:
		for (int i = 0; i < size(); i++)
		{
			const Point3 &vertex = vertices[i].getPosition();
			pointList.push_back( Point2( vertex.x, -vertex.z ) );
		}
		break;
	case AXIS_NEGATIVE_Z:
	case AXIS_Z:
		for (int i = 0; i < size(); i++)
		{
			const Point3 &vertex = vertices[i].getPosition();
			pointList.push_back( Point2( vertex.x, vertex.y ) );
		}
		break;
	default:
		break;
	}
}

void Polyline::computePlanarTextureCoords(Array<Point2f> &texCoords, Axis planeNormalAxis) const
{
	//exit if no vertices
	if ( size() == 0 )
	{
		return;
	}

	texCoords.reserve( size() );

	BBox3 boundingBox = getBBox( false );
	double oneOverWidth, oneOverHeight;
	switch( planeNormalAxis )
	{
	case AXIS_NEGATIVE_X:
	case AXIS_X:
		oneOverWidth = 1.0 / boundingBox.getDepth();
		oneOverHeight = 1.0 / boundingBox.getHeight();
		break;
	case AXIS_NEGATIVE_Y:
	case AXIS_Y:
		oneOverWidth = 1.0 / boundingBox.getWidth();
		oneOverHeight = 1.0 / boundingBox.getDepth();
		break;
	case AXIS_NEGATIVE_Z:
	case AXIS_Z:
		oneOverWidth = 1.0 / boundingBox.getWidth();
		oneOverHeight = 1.0 / boundingBox.getHeight();
		break;
	default:
		return;
	}

	for (int i = 0; i < size(); i++)
	{
		const Point3 &vtx = vertices[i].getPosition();

		double u, v;

		switch( planeNormalAxis )
		{
		case AXIS_NEGATIVE_X:
		case AXIS_X:
			u = ( vtx.z - boundingBox.getUpper().z )  *  oneOverWidth;
			v = ( vtx.y - boundingBox.getLower().y )  *  oneOverHeight;
			break;
		case AXIS_NEGATIVE_Y:
		case AXIS_Y:
			u = ( vtx.x - boundingBox.getLower().x )  *  oneOverWidth;
			v = ( vtx.z - boundingBox.getUpper().z )  *  oneOverHeight;
			break;
		case AXIS_NEGATIVE_Z:
		case AXIS_Z:
			u = ( vtx.x - boundingBox.getLower().x )  *  oneOverWidth;
			v = ( vtx.y - boundingBox.getLower().y )  *  oneOverHeight;
			break;
		default:
			return;
		}

		texCoords.push_back( Point2f( u, v ) );
	}
}

Plane Polyline::computePlane() const
{
	Array<Point3> pointList;
	generatePointList( pointList );
	return Plane( pointList );
}


void Polyline::getBlendingPolyline(BlendingPolyline &p) const
{
	p.clear();
	p.reserve( vertices.size() );

	for (int i = 0; i < vertices.size(); i++)
	{
		p.push_back( vertices[i].getPosition() );
	}

	p.setClosed( closedFlag );
}


void Polyline::computeEdgeTextureTable(PolylineEdgeTextureTable &table) const
{
	//generate a vertex list to work from; if *this is closed, then
	//repeat the first vertex at the end
	Array<PVertex> verts = vertices;
	int forwardRotation = 0;

	if ( isClosed() )
	{
		//ENSURE THAT VERTEX 0 IS A TEXTURE BREAK VERTEX
		//find the first vertex that is at a texture break
		for (int i = 0; i < vertices.size(); i++)
		{
			if ( vertices[i].isTextureBreak() )
			{
				//make this the first vertex
				verts.rotateForward( i );
				forwardRotation = i;

				//finished
				break;
			}
		}

		//repeat the first vertex at the end
		PVertex firstVertex = verts[0];
		verts.push_back( firstVertex );
	}



	int startOfRun = 0;
	Polyline run;
	run.open();
	int runCount = 0;

	int numEdges = verts.size() - 1;

	table.reserve( numEdges );

	//break the polyline down into runs, each one separated by a vertex
	//which is a texture break
	while ( startOfRun < numEdges )
	{
		//generate a run of vertices (until the next texture break vertex)
		//starting at vertex index start
		run.vertices.clear();
		run.vertices.push_back( verts[startOfRun] );
		int i;
		for (i = startOfRun + 1; i < verts.size(); i++)
		{
			run.vertices.push_back( verts[i] );
			if ( verts[i].isTextureBreak() )
			{
				break;
			}
		}
		//start of next run
		startOfRun = i;

		//procedure the vertex run
		Array<double> u;
		//generate texture co-ordinates
		run.computeUnitCumulativeLengthArray( u );
		//number of edges in the run
		int runEdges = run.size() - 1;

		//add an entry into the edge texture table
		for (int i = 0; i < runEdges; i++)
		{
			table.add( PolylineEdgeTexture( runCount, u[i], u[ i + 1 ] ) );
		}

		runCount++;
	}

	if ( isClosed() )
	{
		//if *this is closed, then rotate the run backward to 'undo' the forward
		//rotation applied at the beginning
		table.rotateBackward( forwardRotation );
	}
}

void Polyline::computeIdentityTextureTable(int numSegments, PolylineEdgeTextureTable &table)
{
	table.reserve( numSegments );

	double uInc = 1.0 / (double)numSegments;
	double u = 0.0;

	for (int i = 0; i < numSegments; i++)
	{
		table.add( PolylineEdgeTexture( 0, u, u + uInc ) );

		u += uInc;
	}
}



#endif
