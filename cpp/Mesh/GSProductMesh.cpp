//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCTMESH_CPP__
#define GSPRODUCTMESH_CPP__

#include <string>
#include <sstream>

#include <FileIO/fileio.h>

#include <Math/Triangle3.h>

#include <Mesh/MMesh/MVertex.h>
#include <Mesh/MMesh/MEdge.h>
#include <Mesh/MMesh/MFace.h>
#include <Mesh/MMesh/MMesh.h>

#include <Polyline/Polyline.h>



#include <Mesh/GSProductMesh.h>



#define __DEFAULT_MATERIALID_ -1




int meshBytesWritten = 0;


/***********************************************************************************
											GSProductMesh::MeshRep
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductMesh::MeshRep, GSProductModel::ModelRep );


GSProductMesh::MeshRep::MeshRep() : ModelRep()
{
	mesh = new MMesh();
}

GSProductMesh::MeshRep::MeshRep(const MeshRep &rep)
								: ModelRep()
{
	mesh = rep.mesh->clone();
}

GSProductMesh::MeshRep::~MeshRep()
{
	delete mesh;
}



void GSProductMesh::MeshRep::read(FILE *f, ObjectRepTable *repTable)
{
	ModelRep::read( f, repTable );

	mesh->read( f );
}

void GSProductMesh::MeshRep::write(FILE *f, ObjectRepTable *repTable)
{
	ModelRep::write( f, repTable );

	int posStart = ftell( f );
	mesh->write( f );
	int posEnd = ftell( f );

	meshBytesWritten += ( posEnd - posStart );
}

void GSProductMesh::MeshRep::read(SENode &node, ObjectRepTable *repTable)
{
	ModelRep::read( node[0], repTable );

	mesh->read( node[1] );
}

void GSProductMesh::MeshRep::write(SENode &node, ObjectRepTable *repTable)
{
	ModelRep::write( node.addList(), repTable );

	mesh->write( node.addList() );
}


int GSProductMesh::MeshRep::estimateCost() const
{
	int superClassCost = ModelRep::estimateCost();

	int cumulativeCost = sizeof(MeshRep) + superClassCost + mesh->estimateCost() - sizeof(ModelRep);

	return cumulativeCost;
}



MMesh * GSProductMesh::MeshRep::getMesh()
{
	return mesh;
}

const MMesh * GSProductMesh::MeshRep::getMesh() const
{
	return mesh;
}



void GSProductMesh::MeshRep::transform(const Matrix4 &xform)
{
	mesh->transformVertices( xform, false );
	mesh->finalise();
}

void GSProductMesh::MeshRep::markAll(bool markState)
{
	MarkPredicate pred = markState  ?  MARKPREDICATE_MARK  :  MARKPREDICATE_UNMARK;
	mesh->markVertices_all( pred );
	mesh->finalise();
}


void GSProductMesh::MeshRep::i_paint3dEndNotify()
{
	ModelRep::i_paint3dEndNotify();

	mesh->destroyLiveSubdivision();
}







/*********************************************************************************
													GSProductMesh
 *********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductMesh, GSProductModel );


GSProductMesh::GSProductMesh() : GSProductModel( new MeshRep() )
{
}

GSProductMesh::GSProductMesh(const GSProductMesh &m)
						: GSProductModel( m )
{
}

GSProductMesh::GSProductMesh(ObjectRep *rep)
						: GSProductModel( rep )
{
}

GSProductMesh::~GSProductMesh()
{
}




std::string GSProductMesh::getDescription() const
{
	std::ostringstream description;
	description << "Mesh: " <<  getNumVertices() << " vertices, " <<  getNumEdges() << " edges, " <<  getNumFaces() << " faces" << (char)0x00;
	return description.str();
}


int GSProductMesh::estimateCost() const
{
	int superClassCost = GSProductModel::estimateCost();

	return sizeof(GSProductMesh) + superClassCost - sizeof(GSProductModel);
}




void GSProductMesh::copyFrom(const GSProduct *o)
{
	GSProductModel::copyFrom( o );
}





void GSProductMesh::read(SENode &node, ObjectRepTable *repTable)
{
	GSProductModel::read( node[0], repTable );
}

void GSProductMesh::write(SENode &node, ObjectRepTable *repTable)
{
	GSProductModel::write( node.addList(), repTable );
}



void GSProductMesh::preComputeFull()
{
	GSProductModel::preComputeFull();

	MMesh *m = getReadOnlyRepMesh();
	m->preComputeFull();
}




MMesh * GSProductMesh::getRepMesh()
{
	MeshRep *rep = (MeshRep*)getRepresentation();
	return rep->getMesh();
}

MMesh * GSProductMesh::getReadOnlyRepMesh()
{
	MeshRep *rep = (MeshRep*)getReadOnlyRepresentation();
	return rep->getMesh();
}

const MMesh * GSProductMesh::getReadOnlyRepMesh() const
{
	const MeshRep *rep = (const MeshRep*)getReadOnlyRepresentation();
	return rep->getMesh();
}



int GSProductMesh::getNumVertices() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getNumVertices();
}

int GSProductMesh::getNumEdges() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getNumEdges();
}

int GSProductMesh::getNumFaces() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getNumFaces();
}




MMesh & GSProductMesh::getMMeshForDisplay()
{
	MMesh *mesh = getReadOnlyRepMesh();
	return *mesh;
}






void GSProductMesh::flip()
{
	GSProductModel::flip();

	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flip();
	lock();
}





void GSProductMesh::i_transform(const Matrix4 &m, bool markedOnly)
{
	GSProductModel::i_transform( m, markedOnly );

	MMesh *mesh = getRepMesh();
	mesh->transformVertices( m, markedOnly );
	finalise();
}

void GSProductMesh::i_transform(const Transformation *x, bool markedOnly)
{
	GSProductModel::i_transform( x, markedOnly );

	MMesh *mesh = getRepMesh();
	mesh->transformVertices( x, markedOnly );
	finalise();
}

void GSProductMesh::i_transformPointsSave(bool markedOnly)
{
	GSProductModel::i_transformPointsSave( markedOnly );

	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveVertexPositions( markedOnly );
}

void GSProductMesh::i_transformPointsRestore(bool markedOnly)
{
	GSProductModel::i_transformPointsRestore( markedOnly );

	MMesh *mesh = getRepMesh();
	mesh->restoreVertexPositions( markedOnly );
	finalise();
}


void GSProductMesh::i_markPoints(MarkPredicate predicate, const Region3d *region)
{
	GSProductModel::i_markPoints( predicate, region );

	MMesh *mesh = getRepMesh();
	mesh->markVertices_region( region, predicate );
}

void GSProductMesh::i_markAllPoints(MarkPredicate predicate)
{
	GSProductModel::i_markAllPoints( predicate );
	MMesh *mesh = getRepMesh();
	mesh->markVertices_all( predicate );
}

void GSProductMesh::i_markPointsSave()
{
	GSProductModel::i_markPointsSave();

	MMesh *mesh = getReadOnlyRepMesh();
	mesh->markVertices_save();
}

void GSProductMesh::i_markPointsRestore()
{
	GSProductModel::i_markPointsRestore();

	MMesh *mesh = getRepMesh();
	mesh->markVertices_restore();
}

MarkStatus GSProductMesh::i_getMarkStatus() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVertexMarkStatus();
}

Side GSProductMesh::i_arePointsInside(const Region3d *region) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->areVerticesInside( region );
}




BBox3 GSProductMesh::i_getBBox(bool markedOnly) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVerticesBBox( markedOnly );
}

BBox3 GSProductMesh::i_getBBox(const Matrix4 &m, bool markedOnly) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVerticesBBox( m, markedOnly );
}

BBox3 GSProductMesh::i_getBBox(const Transformation *x, bool markedOnly) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVerticesBBox( x, markedOnly );
}

BBox3 GSProductMesh::i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVerticesBBox( m, x, markedOnly );
}


bool GSProductMesh::i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection)
{
	MMesh *mesh = getReadOnlyRepMesh();
	MFace *f = mesh->raytraceFaceClipRay( ray, backfaceCullingFlag, false, t, intersection );
	return f != NULL;
}



bool GSProductMesh::canMergeWith(GSProductModel *withObject) const
{
	return withObject->isOfType( getType() );
}

GSProductModel * GSProductMesh::i_mergedWith(const Array<GSProductModel*> &models)
{
	// Build the list of MMesh object to get mesh data from
	Array<const MMesh*> sourceMeshes;
	sourceMeshes.resize( models.size() );
	for (int modelI = 0; modelI < models.size(); modelI++)
	{
		sourceMeshes[modelI] = static_cast<GSProductMesh*>( models[modelI] )->getReadOnlyRepMesh();
	}


	//.Create a clone of @this, to merge the meshes into
	GSProductMesh *destination = cloneTyped<GSProductMesh>();
	destination->unlock();

	MMesh *mesh = destination->getRepMesh();
	mesh->mergeWith( sourceMeshes );

	destination->lock();

	return destination;
}






// ********************* MESH SPECIFIC METHODS *********************

void GSProductMesh::clear()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->clear();
	lock();
}


void GSProductMesh::importMesh(const MImportMesh &importMesh)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->importMesh( importMesh );
	lock();
}




int GSProductMesh::addVertex(const Point3 &location, bool marked)
{
	unlock();

	MMesh *mesh = getRepMesh();
	MVertex *vertex = mesh->addVertex( location );
	if ( marked )
	{
		vertex->vertexMark();
	}

	lock();

	return vertex->getIndex();
}

int GSProductMesh::addFace(const Array<int> &vertexIndices, const Array<Point2f> &textureCoords )
{
	unlock();
	MMesh *mesh = getRepMesh();
	MFace *face = mesh->addFace( vertexIndices, textureCoords, __DEFAULT_MATERIALID_ );
	lock();

	return face->getIndex();
}

int GSProductMesh::addFace(int va, const Point2f &ta, int vb, const Point2f &tb, int vc, const Point2f &tc)
{
	Array<int> vts;
	Array<Point2f> textureCoords;

	vts.reserve( 3 );
	textureCoords.reserve( 3 );

	vts.push_back( va );
	vts.push_back( vb );
	vts.push_back( vc );

	textureCoords.push_back( ta );
	textureCoords.push_back( tb );
	textureCoords.push_back( tc );

	return addFace( vts, textureCoords );
}

int GSProductMesh::addFace(int va, const Point2f &ta, int vb, const Point2f &tb, int vc, const Point2f &tc, int vd, const Point2f &td)
{
	Array<int> vts;
	Array<Point2f> textureCoords;

	vts.reserve( 4 );
	textureCoords.reserve( 4 );

	vts.push_back( va );
	vts.push_back( vb );
	vts.push_back( vc );
	vts.push_back( vd );

	textureCoords.push_back( ta );
	textureCoords.push_back( tb );
	textureCoords.push_back( tc );
	textureCoords.push_back( td );

	return addFace( vts, textureCoords );
}



int GSProductMesh::addFace(const Array<Point2f> &textureCoordinates, const Array<int> &vertexIndices, int startAt, bool reverseVertexOrder)
{
	Array<int> faceVertexIndices;
	Array<Point2f> faceTexCoords = textureCoordinates;

	int numVertices = textureCoordinates.size();
	faceVertexIndices.reserve( numVertices );

	faceVertexIndices.extend( vertexIndices, startAt, startAt + numVertices );

	if ( reverseVertexOrder )
	{
		faceVertexIndices.reverse();
		faceTexCoords.reverse();
	}

	return addFace( faceVertexIndices, faceTexCoords );
}



void GSProductMesh::setEdgesAsNormalSharp(const Array<int> &vertexIndices, int offset, bool closed)
{
	unlock();

	MMesh *mesh = getRepMesh();

	Array<MVertex*> &vertices = mesh->getVertices();

	MVertexList verts;
	verts.reserve( vertexIndices.size() );
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		verts.push_back(  vertices[  vertexIndices[i] + offset  ]  );
	}

	verts.setClosed( closed );

	mesh->setEdgesByVertexAsNormalSharp( verts );

	lock();
}



void GSProductMesh::reserveVertices(int s)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->reserveVertices( s );
	lock();
}

void GSProductMesh::reserveFaces(int s)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->reserveFacesAndEdges( s );
	lock();
}




int GSProductMesh::getNumMarkedVertices() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getNumMarkedVertices();
}

int GSProductMesh::getNumMarkedEdges() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getNumMarkedEdges();
}

int GSProductMesh::getNumMarkedFaces() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getNumMarkedFaces();
}




void GSProductMesh::addLoftFaces(const Polyline &poly, int loftSegments, const Array<int> &vertexIndices)
{
	//compute the texture table
	PolylineEdgeTextureTable ptex;
	poly.computeEdgeTextureTable( ptex );



	//faces
	int polyEdges = poly.getNumEdges();
	int width = polyEdges + 1;


	double v0 = 0.0, vInc = 1.0 / (double)loftSegments;
	double v1 = vInc;

	int y0 = 0, y1 = width;

	for (int i = 0; i < loftSegments; i++)
	{
		double vAverage = ( v0 + v1 ) * 0.5;

		for (int j = 0; j < polyEdges; j++)
		{
			int a = y0 + j;
			int b = y0 + j + 1;
			int c = y1 + j + 1;
			int d = y1 + j;

			int va = vertexIndices[a];
			int vb = vertexIndices[b];
			int vc = vertexIndices[c];
			int vd = vertexIndices[d];

			if ( vb != vc  &&  vd == va )
			{
				addFace( vb, Point2f( ptex[j].getU1(), v0 ),
						vc, Point2f( ptex[j].getU1(), v1 ),
						vd, Point2f( ptex[j].getU0(), vAverage ) );
			}

			if ( vb == vc  &&  vd != va )
			{
				addFace( va, Point2f( ptex[j].getU0(), v0 ),
						vb, Point2f( ptex[j].getU1(), vAverage ),
						vd, Point2f( ptex[j].getU0(), v1 ) );
			}

			if ( vb != vc  &&  vd != va )
			{
				addFace( va, Point2f( ptex[j].getU0(), v0 ),
						vb, Point2f( ptex[j].getU1(), v0 ),
						vc, Point2f( ptex[j].getU1(), v1 ),
						vd, Point2f( ptex[j].getU0(), v1 ) );
			}
		}

		v0 = v1;
		v1 += vInc;
		y0 = y1;
		y1 += width;
	}


	unlock();

	MMesh *mesh = getRepMesh();

	//normal sharpen edges
	Array<MVertex*> &vertices = mesh->getVertices();

	int x0 = 0;
	int x1 = width;
	for (int j = 0; j < loftSegments; j++)
	{
		for (int i = 0; i < poly.size(); i++)
		{
			if ( poly[i].isNormalSharp() )
			{
				MVertex *v0 = vertices[ vertexIndices[ x0 + i ] ];
				MVertex *v1 = vertices[ vertexIndices[ x1 + i ] ];

				MEdge *e = v0->findEdgeTo( v1, MFINDPOLICY_RETURNNULL );

				if ( e != NULL )
				{
					e->setNormalSharp();
				}
			}
		}
		x0 = x1;
		x1 += width;
	}

	lock();
}



void GSProductMesh::addLoftFaces(const Polyline &poly1, const Polyline &poly0, const Array<int> &vertexIndices)
{
	PolylineEdgeTextureTable ptex0, ptex1;
	poly0.computeEdgeTextureTable( ptex0 );
	poly1.computeEdgeTextureTable( ptex1 );



	//faces
	int poly0Edges = poly0.getNumEdges();
	int poly1Edges = poly1.getNumEdges();

	int width = poly1Edges + 1;


	int y0 = 0, y1 = width;

	for (int i = 0; i < poly0Edges; i++)
	{
		double vAverage = ( ptex0[i].getU0() + ptex0[i].getU1() ) * 0.5;

		for (int j = 0; j < poly1Edges; j++)
		{
			double uAverage = ( ptex1[j].getU0() + ptex1[j].getU1() ) * 0.5;

			int a = y0 + j;
			int b = y0 + j + 1;
			int c = y1 + j + 1;
			int d = y1 + j;

			int va = vertexIndices[a];
			int vb = vertexIndices[b];
			int vc = vertexIndices[c];
			int vd = vertexIndices[d];

			if ( va == vb  &&  vb != vc  &&  vc != vd  &&  vd != va )
			{
				addFace( va, Point2f( uAverage, ptex0[i].getU0() ),
						vc, Point2f( ptex1[j].getU1(), ptex0[i].getU1() ),
						vd, Point2f( ptex1[j].getU0(), ptex0[i].getU1() ) );
			}

			if ( va != vb  &&  vb == vc  &&  vc != vd  &&  vd != va )
			{
				addFace( va, Point2f( ptex1[j].getU0(), ptex0[i].getU0() ),
						vb, Point2f( ptex1[j].getU1(), vAverage ),
						vd, Point2f( ptex1[j].getU0(), ptex0[i].getU1() ) );
			}

			if ( va != vb  &&  vb != vc  &&  vc == vd  &&  vd != va )
			{
				addFace( va, Point2f( ptex1[j].getU0(), ptex0[i].getU0() ),
						vb, Point2f( ptex1[j].getU1(), ptex0[i].getU0() ),
						vc, Point2f( uAverage, ptex0[i].getU1() ) );
			}

			if ( va != vb  &&  vb != vc  &&  vc != vd  &&  vd == va )
			{
				addFace( vb, Point2f( ptex1[j].getU1(), ptex0[i].getU0() ),
						vc, Point2f( ptex1[j].getU1(), ptex0[i].getU1() ),
						vd, Point2f( ptex1[j].getU0(), vAverage ) );
			}

			if ( va != vb  &&  vb != vc  &&  vc != vd  &&  vd != va )
			{
				addFace( va, Point2f( ptex1[j].getU0(), ptex0[i].getU0() ),
						vb, Point2f( ptex1[j].getU1(), ptex0[i].getU0() ),
						vc, Point2f( ptex1[j].getU1(), ptex0[i].getU1() ),
						vd, Point2f( ptex1[j].getU0(), ptex0[i].getU1() ) );
			}
		}

		y0 = y1;
		y1 += width;
	}


	unlock();

	MMesh *mesh = getRepMesh();

	//normal sharpen edges
	Array<MVertex*> &vertices = mesh->getVertices();



	int x0 = 0;
	int x1 = width;
	for (int j = 0; j < poly0Edges; j++)
	{
		for (int i = 0; i < poly1.size(); i++)
		{
			if ( poly1[i].isNormalSharp() )
			{
				MVertex *v0 = vertices[ vertexIndices[ x0 + i ] ];
				MVertex *v1 = vertices[ vertexIndices[ x1 + i ] ];

				MEdge *e = v0->findEdgeTo( v1, MFINDPOLICY_RETURNNULL );

				if ( e != NULL )
				{
					e->setNormalSharp();
				}
			}
		}
		x0 = x1;
		x1 += width;
	}


	for (int j = 0; j < poly1Edges; j++)
	{
		for (int i = 0; i < poly0.size(); i++)
		{
			if ( poly0[i].isNormalSharp() )
			{
				int x = i * width;

				MVertex *v0 = vertices[ vertexIndices[ x + j ] ];
				MVertex *v1 = vertices[ vertexIndices[ x + j + 1 ] ];

				MEdge *e = v0->findEdgeTo( v1, MFINDPOLICY_RETURNNULL );

				if ( e != NULL )
				{
					e->setNormalSharp();
				}
			}
		}
	}

	lock();
}


void GSProductMesh::addCapFace(const Polyline &poly, const Array<int> &vertexIndices, Axis planeNormal, int capOffset, bool flipCap,bool sharpenBoundaries)
{
	unlock();

	//GENERATE TEXTURE CO-ORDINATES
	Array<Point2f> textureCoordinates;
	poly.computePlanarTextureCoords( textureCoordinates, planeNormal );


	Array<int> sharpEdgeVertices;
	sharpEdgeVertices.reserve( poly.size() );


	addFace( textureCoordinates, vertexIndices, capOffset, flipCap );

	if ( sharpenBoundaries )
	{
		sharpEdgeVertices.clear();
		for (int i = 0; i < poly.size(); i++)
		{
			sharpEdgeVertices.push_back( vertexIndices[ capOffset + i ] );
		}
		setEdgesAsNormalSharp( sharpEdgeVertices, 0, poly.isClosed() );
	}

	lock();
}

void GSProductMesh::addCapFaces(const Polyline &poly, const Array<int> &vertexIndices, Axis planeNormal, int startCapOffset, int endCapOffset,
								bool flipStartCap, bool flipEndCap, bool sharpenBoundaries)
{
	unlock();

	if ( startCapOffset == -1  &&  endCapOffset == -1 )
	{
		return;
	}

	//GENERATE TEXTURE CO-ORDINATES
	Array<Point2f> textureCoordinates;
	poly.computePlanarTextureCoords( textureCoordinates, planeNormal );


	Array<int> sharpEdgeVertices;
	sharpEdgeVertices.reserve( poly.size() );


	if ( startCapOffset != -1 )
	{
		addFace( textureCoordinates, vertexIndices, startCapOffset, flipStartCap );

		if ( sharpenBoundaries )
		{
			sharpEdgeVertices.clear();
			for (int i = 0; i < poly.size(); i++)
			{
				sharpEdgeVertices.push_back( vertexIndices[ startCapOffset + i ] );
			}
			setEdgesAsNormalSharp( sharpEdgeVertices, 0, poly.isClosed() );
		}
	}

	if ( endCapOffset != -1 )
	{
		addFace( textureCoordinates, vertexIndices, endCapOffset, flipEndCap );

		if ( sharpenBoundaries )
		{
			sharpEdgeVertices.clear();
			for (int i = 0; i < poly.size(); i++)
			{
				sharpEdgeVertices.push_back( vertexIndices[ endCapOffset + i ] );
			}
			setEdgesAsNormalSharp( sharpEdgeVertices, 0, poly.isClosed() );
		}
	}

	lock();
}




void GSProductMesh::addTesselatedPolyline(const Polyline &poly, bool flip, Axis planeNormal)
{
	//CREATE THE VERTICES
	Array<int> vertexIndices;
	vertexIndices.reserve( poly.size() );
	for (int i = 0; i < poly.size(); i++)
	{
		vertexIndices.push_back( addVertex( poly[i].getPosition() ) );
	}


	//COMPUTE TEXTURE CO-ORDINATES
	Array<Point2f> textureCoordinates;
	poly.computePlanarTextureCoords( textureCoordinates, planeNormal );


	addFace( textureCoordinates, vertexIndices, 0, flip );
}





// ********************* MESH RAYTRACE METHODS *********************

int GSProductMesh::raytraceFace(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersection)
{
	Segment3 tempRay = ray * getLocalTransformationInverse();

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *face = mesh->raytraceFace( tempRay, backfaceCullingFlag, faceMarkedOnlyFlag, t, intersection );

	if ( face != NULL )
	{
		intersection *= getLocalTransformation();
		return face->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::raytraceFaceAttribPoint(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersection, Point2 &attribPoint)
{
	Segment3 tempRay = ray * getLocalTransformationInverse();

	MMesh *mesh = getReadOnlyRepMesh();
	int triangleIndex;

	MFace *face = mesh->raytraceFace( tempRay, backfaceCullingFlag, faceMarkedOnlyFlag, t, intersection, triangleIndex );

	if ( face != NULL )
	{
		Triangle3 tri = face->getTriangle( triangleIndex );
		Triangle2 attrTri = face->getAttribTriangle( triangleIndex );

		Triangle3 attrTri3 = Triangle3( attrTri );

		Matrix4 xform = Matrix4::spaceToSpace( tri, attrTri3 );
		Point3 materialHit = intersection * xform;
		attribPoint = materialHit.toPoint2();

		intersection *= getLocalTransformation();

		return face->getIndex();
	}
	else
	{
		return -1;
	}
}



int GSProductMesh::pickFace(const MPick &pick, Point3 &intersectionPoint)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *face = mesh->pickFace( localPick, intersectionPoint );

	if ( face != NULL )
	{
		return face->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickFace(const MPick &pick)
{
	Point3 intersectionPoint;
	return pickFace( pick, intersectionPoint );
}

int GSProductMesh::pickEdge(const MPick &pick, Point3 &closestPoint, int &thruFaceIndex)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *thruFace = NULL;
	MEdge *edge = mesh->pickEdge( localPick, closestPoint, thruFace );

	thruFaceIndex = -1;
	if ( thruFace != NULL )
	{
		thruFaceIndex = thruFace->getIndex();
	}

	if ( edge != NULL )
	{
		return edge->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickEdge(const MPick &pick, Point3 &closestPoint)
{
	int thruFaceIndex = -1;
	return pickEdge( pick, closestPoint, thruFaceIndex );
}

int GSProductMesh::pickVertex(const MPick &pick, int &thruFaceIndex)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *thruFace = NULL;
	MVertex *vertex = mesh->pickVertex( localPick, thruFace );

	thruFaceIndex = -1;
	if ( thruFace != NULL )
	{
		thruFaceIndex = thruFace->getIndex();
	}

	if ( vertex != NULL )
	{
		return vertex->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickVertex(const MPick &pick)
{
	int thruFaceIndex = -1;
	return pickVertex( pick, thruFaceIndex );
}

int GSProductMesh::pickVertexThruEdge(const MPick &pick, int &thruFaceIndex, int &thruEdgeIndex)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *thruFace = NULL;
	MEdge *thruEdge = NULL;
	MVertex *vertex = mesh->pickVertexThruEdge( localPick, thruFace, thruEdge );

	thruFaceIndex = -1;
	if ( thruFace != NULL )
	{
		thruFaceIndex = thruFace->getIndex();
	}

	thruEdgeIndex = -1;
	if ( thruEdge != NULL )
	{
		thruEdgeIndex = thruEdge->getIndex();
	}

	if ( vertex != NULL )
	{
		return vertex->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickEdgeRing(Array<int> &ringEdgeIndices, const MPick &pick, Point3 &closestPoint, int &thruFaceIndex)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *thruFace = NULL;
	MEdgeList ring;
	MEdge *seed = mesh->pickEdgeRing( ring, localPick, closestPoint, thruFace );

	thruFaceIndex = -1;
	if ( thruFace != NULL )
	{
		thruFaceIndex = thruFace->getIndex();
	}

	if ( seed != NULL )
	{
		ringEdgeIndices.resize( ring.size() );
		for (int edgeI = 0; edgeI < ring.size(); edgeI++)
		{
			ringEdgeIndices[edgeI] = ring[edgeI]->getIndex();
		}

		return seed->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickEdgeRing(Array<int> &ringEdgeIndices, const MPick &pick, Point3 &closestPoint)
{
	int thruFaceIndex = -1;
	return pickEdgeRing( ringEdgeIndices, pick, closestPoint, thruFaceIndex );
}

int GSProductMesh::pickEdgeLoop(Array<int> &loopEdgeIndices, const MPick &pick, Point3 &closestPoint, int &thruFaceIndex)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *thruFace = NULL;
	MEdgeList loop;
	MEdge *seed = mesh->pickEdgeLoop( loop, localPick, closestPoint, thruFace );

	thruFaceIndex = -1;
	if ( thruFace != NULL )
	{
		thruFaceIndex = thruFace->getIndex();
	}

	if ( seed != NULL )
	{
		loopEdgeIndices.resize( loop.size() );
		for (int edgeI = 0; edgeI < loop.size(); edgeI++)
		{
			loopEdgeIndices[edgeI] = loop[edgeI]->getIndex();
		}

		return seed->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickEdgeLoop(Array<int> &loopEdgeIndices, const MPick &pick, Point3 &closestPoint)
{
	int thruFaceIndex = -1;
	return pickEdgeLoop( loopEdgeIndices, pick, closestPoint, thruFaceIndex );
}

int GSProductMesh::pickEdgeBoundary(Array<int> &boundaryEdgeIndices, const MPick &pick, Point3 &closestPoint, int &thruFaceIndex)
{
	MPick localPick = pick.preTransformed( getLocalTransformation(), getLocalTransformationInverse() );

	MMesh *mesh = getReadOnlyRepMesh();
	MFace *thruFace = NULL;
	MEdgeList boundary;
	MEdge *seed = mesh->pickEdgeBoundary( boundary, localPick, closestPoint, thruFace );

	thruFaceIndex = -1;
	if ( thruFace != NULL )
	{
		thruFaceIndex = thruFace->getIndex();
	}

	if ( seed != NULL )
	{
		boundaryEdgeIndices.resize( boundary.size() );
		for (int edgeI = 0; edgeI < boundary.size(); edgeI++)
		{
			boundaryEdgeIndices[edgeI] = boundary[edgeI]->getIndex();
		}

		return seed->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::pickEdgeBoundary(Array<int> &boundaryEdgeIndices, const MPick &pick, Point3 &closestPoint)
{
	int thruFaceIndex = -1;
	return pickEdgeBoundary( boundaryEdgeIndices, pick, closestPoint, thruFaceIndex );
}




void GSProductMesh::discoverEdgeRing(int seedIndex, Array<int> &ringEdgeIndices)
{
	if ( seedIndex != -1 )
	{
		MEdgeList ring;

		MMesh *mesh = getReadOnlyRepMesh();

		MEdge *seed = mesh->getEdges()[seedIndex];
		mesh->discoverEdgeRing( seed, ring );

		ringEdgeIndices.resize( ring.size() );
		for (int edgeI = 0; edgeI < ring.size(); edgeI++)
		{
			ringEdgeIndices[edgeI] = ring[edgeI]->getIndex();
		}
	}
}

void GSProductMesh::discoverEdgeLoop(int seedIndex, Array<int> &loopEdgeIndices)
{
	if ( seedIndex != -1 )
	{
		MEdgeList loop;

		MMesh *mesh = getReadOnlyRepMesh();

		MEdge *seed = mesh->getEdges()[seedIndex];
		mesh->discoverEdgeLoop( seed, loop );

		loopEdgeIndices.resize( loop.size() );
		for (int edgeI = 0; edgeI < loop.size(); edgeI++)
		{
			loopEdgeIndices[edgeI] = loop[edgeI]->getIndex();
		}
	}
}

void GSProductMesh::discoverEdgeBoundary(int seedIndex, Array<int> &boundaryEdgeIndices)
{
	if ( seedIndex != -1 )
	{
		MEdgeList loop;

		MMesh *mesh = getReadOnlyRepMesh();

		MEdge *seed = mesh->getEdges()[seedIndex];
		mesh->discoverEdgeBoundary( seed, loop );

		boundaryEdgeIndices.resize( loop.size() );
		for (int edgeI = 0; edgeI < loop.size(); edgeI++)
		{
			boundaryEdgeIndices[edgeI] = loop[edgeI]->getIndex();
		}
	}
}


bool GSProductMesh::discoverEdgeLoopPath(int fromVertexIndex, int toVertexIndex, Array<int> &pathEdgeIndices)
{
	if ( fromVertexIndex != -1  &&  toVertexIndex != -1 )
	{
		MEdgeRun path;

		MMesh *mesh = getReadOnlyRepMesh();

		MVertex *from = mesh->getVertices()[fromVertexIndex];
		MVertex *to = mesh->getVertices()[toVertexIndex];

		bool result = mesh->discoverEdgeLoopPath( from, to, path );

		if ( !result )
		{
			return false;
		}

		pathEdgeIndices.resize( path.size() );
		for (int edgeI = 0; edgeI < path.size(); edgeI++)
		{
			pathEdgeIndices[edgeI] = path[edgeI]->getIndex();
		}

		return true;
	}
	else
	{
		return false;
	}
}

bool GSProductMesh::discoverEdgeRingPath(int fromEdgeIndex, int toEdgeIndex, Array<int> &pathEdgeIndices)
{
	if ( fromEdgeIndex != -1  &&  toEdgeIndex != -1 )
	{
		MEdgeRun path;

		MMesh *mesh = getReadOnlyRepMesh();

		MEdge *from = mesh->getEdges()[fromEdgeIndex];
		MEdge *to = mesh->getEdges()[toEdgeIndex];

		bool result = mesh->discoverEdgeRingPath( from, to, path );

		if ( !result )
		{
			return false;
		}

		pathEdgeIndices.resize( path.size() );
		for (int edgeI = 0; edgeI < path.size(); edgeI++)
		{
			pathEdgeIndices[edgeI] = path[edgeI]->getIndex();
		}

		return true;
	}
	else
	{
		return false;
	}
}




void GSProductMesh::py_indexListToPyList(boost::python::list pyList, const Array<int> &indexList)
{
	for (int i = 0; i < indexList.size(); i++)
	{
		pyList.append( indexList[i] );
	}
}


boost::python::tuple GSProductMesh::py_pickVertex(const MPick &pick)
{
	int thruFaceIndex = -1;
	int vertexIndex = pickVertex( pick, thruFaceIndex );
	return boost::python::make_tuple( vertexIndex, thruFaceIndex );
}

boost::python::tuple GSProductMesh::py_pickVertexThruEdge(const MPick &pick)
{
	int thruFaceIndex = -1, thruEdgeIndex = -1;
	int vertexIndex = pickVertexThruEdge( pick, thruFaceIndex, thruEdgeIndex );
	return boost::python::make_tuple( vertexIndex, thruFaceIndex, thruEdgeIndex );
}

boost::python::tuple GSProductMesh::py_pickEdge(const MPick &pick)
{
	Point3 closestPoint;
	int thruFaceIndex = -1;

	int edgeIndex = pickEdge( pick, closestPoint, thruFaceIndex );

	return boost::python::make_tuple( edgeIndex, closestPoint, thruFaceIndex );
}

boost::python::tuple GSProductMesh::py_pickFace(const MPick &pick)
{
	Point3 intersectionPoint;

	int faceIndex = pickFace( pick, intersectionPoint );

	return boost::python::make_tuple( faceIndex, intersectionPoint );
}

boost::python::tuple GSProductMesh::py_pickEdgeRing(const MPick &pick)
{
	Point3 closestPoint;
	int thruFaceIndex = -1;

	boost::python::list pyEdgeIndices;

	Array<int> edgeIndices;
	int seedIndex = pickEdgeRing( edgeIndices, pick, closestPoint, thruFaceIndex );
	py_indexListToPyList( pyEdgeIndices, edgeIndices );

	return boost::python::make_tuple( pyEdgeIndices, seedIndex, closestPoint, thruFaceIndex );
}

boost::python::tuple GSProductMesh::py_pickEdgeLoop(const MPick &pick)
{
	Point3 closestPoint;
	int thruFaceIndex = -1;

	boost::python::list pyEdgeIndices;

	Array<int> edgeIndices;
	int seedIndex = pickEdgeLoop( edgeIndices, pick, closestPoint, thruFaceIndex );
	py_indexListToPyList( pyEdgeIndices, edgeIndices );

	return boost::python::make_tuple( pyEdgeIndices, seedIndex, closestPoint, thruFaceIndex );
}

boost::python::tuple GSProductMesh::py_pickEdgeBoundary(const MPick &pick)
{
	Point3 closestPoint;
	int thruFaceIndex = -1;

	boost::python::list pyEdgeIndices;

	Array<int> edgeIndices;
	int seedIndex = pickEdgeBoundary( edgeIndices, pick, closestPoint, thruFaceIndex );
	py_indexListToPyList( pyEdgeIndices, edgeIndices );

	return boost::python::make_tuple( pyEdgeIndices, seedIndex, closestPoint, thruFaceIndex );
}


boost::python::list GSProductMesh::py_discoverEdgeRing(int seedIndex)
{
	Array<int> indices;
	discoverEdgeRing( seedIndex, indices );
	boost::python::list result;
	py_indexListToPyList( result, indices );
	return result;
}

boost::python::list GSProductMesh::py_discoverEdgeLoop(int seedIndex)
{
	Array<int> indices;
	discoverEdgeLoop( seedIndex, indices );
	boost::python::list result;
	py_indexListToPyList( result, indices );
	return result;
}

boost::python::list GSProductMesh::py_discoverEdgeBoundary(int seedIndex)
{
	Array<int> indices;
	discoverEdgeBoundary( seedIndex, indices );
	boost::python::list result;
	py_indexListToPyList( result, indices );
	return result;
}

boost::python::tuple GSProductMesh::py_discoverEdgeLoopPath(int fromVertexIndex, int toVertexIndex)
{
	Array<int> indices;
	bool bSuccess = discoverEdgeLoopPath( fromVertexIndex, toVertexIndex, indices );
	boost::python::list result;
	py_indexListToPyList( result, indices );
	return boost::python::make_tuple( bSuccess, result );
}

boost::python::tuple GSProductMesh::py_discoverEdgeRingPath(int fromEdgeIndex, int toEdgeIndex)
{
	Array<int> indices;
	bool bSuccess = discoverEdgeRingPath( fromEdgeIndex, toEdgeIndex, indices );
	boost::python::list result;
	py_indexListToPyList( result, indices );
	return boost::python::make_tuple( bSuccess, result );
}




void GSProductMesh::finalise()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->finalise();
	lock();
}




//MESH OPS
void GSProductMesh::mirror(const Plane &mirrorPlane)
{
	unlock();

	MMesh *mesh = getRepMesh();

	MMesh reflection( *mesh );
	reflection.transform( Matrix4::reflect( mirrorPlane ) );
	reflection.flip();

	mesh->mergeFrom( reflection );

	lock();
}


void GSProductMesh::subdivide(bool markedFacesOnly, bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness)
{
	unlock();

	MMesh *mesh = getRepMesh();

	if ( markedFacesOnly )
	{
		mesh->subdivideMarked( catmullClark, preserveNormalSharpness );
	}
	else
	{
		mesh->subdivide( catmullClark, preserveNormalSharpness );
	}

	lock();
}



void GSProductMesh::knife(const MKnifeTarget &startTarget, const MKnifeTarget &endTarget, bool faceMarkedOnly, const BackfaceCull *cull, bool markCutEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->knife( startTarget, endTarget, faceMarkedOnly, cull, markCutEdges );
	lock();
}



//VERTEX OPS

void GSProductMesh::expandMarkedVertices(MeshExpandVerticesAdjuster &adjuster, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	mesh->expandMarkedVertices( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, markBoundaryEdges, nsharpBoundaryEdges );
	lock();
}

void GSProductMesh::expandMarkedVerticesInMarkedFaces(MeshExpandVerticesInMarkedFacesAdjuster &adjuster, bool markBoundaryEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	mesh->expandMarkedVerticesInMarkedFaces( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, markBoundaryEdges );
	lock();
}


void GSProductMesh::bevelMarkedVertices(MeshBevelVerticesAdjuster &adjuster, bool useExistingTexturing, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	mesh->bevelMarkedVertices( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, useExistingTexturing, markBoundaryEdges, nsharpBoundaryEdges );
	lock();
}


void GSProductMesh::connectMarkedVertices(bool markedFacesOnly, bool markCutEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->connectMarkedVertices( markedFacesOnly, markCutEdges );
	lock();
}


void GSProductMesh::weldMarkedVertices(double threshold, bool weldLinkedVertices, bool markMergedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->weldMarkedVertices( threshold, weldLinkedVertices, markMergedEdges );
	lock();
}



void GSProductMesh::targetWeld(const MWeldTargetList &weldTargets)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->targetWeld( weldTargets );
	lock();
}

bool GSProductMesh::isValidWeldTargetVertex(int sourceVertexIndex, int targetVertexIndex)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MVertex *sourceVertex = mesh->getVertices()[sourceVertexIndex];
	MVertex *targetVertex = mesh->getVertices()[targetVertexIndex];

	bool result = mesh->isValidWeldTargetVertex( sourceVertex, targetVertex );

	lock();

	return result;
}

bool GSProductMesh::isValidWeldTargetEdge(int sourceVertexIndex, int targetEdgeIndex, MWeldTarget::TargetType targetType)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MVertex *sourceVertex = mesh->getVertices()[sourceVertexIndex];
	MEdge *targetEdge = mesh->getEdges()[targetEdgeIndex];

	bool result = mesh->isValidWeldTargetEdge( sourceVertex, targetEdge, targetType );

	lock();

	return result;
}



int GSProductMesh::dissolveMarkedVertices(bool twoEdgesOnly, bool twoFacesOnly, bool markCreatedFaces)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->dissolveMarkedVertices( twoEdgesOnly, twoFacesOnly, markCreatedFaces );
	lock();

	return result;
}


void GSProductMesh::setMarkedVertexSharpness(double sharpness, MSharpenFunction sharpenFunction)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->setMarkedVertexSharpness( sharpness, sharpenFunction );
	lock();
}

void GSProductMesh::saveMarkedVertexSharpness(MeshSharpnessBackup &sharpnessBackup)
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveMarkedVertexSharpness( sharpnessBackup.sharpnessBackup );
}

void GSProductMesh::restoreMarkedVertexSharpness(MeshSharpnessBackup &sharpnessBackup)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->restoreMarkedVertexSharpness( sharpnessBackup.sharpnessBackup );
	lock();
}


void GSProductMesh::setMarkedVertexNormalSharpness(bool sharpness)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->setMarkedVertexNormalSharpness( sharpness );
	lock();
}




//EDGE OPS

void GSProductMesh::expandMarkedEdges(MeshExpandEdgesAdjuster &adjuster, bool markBoundaryEdges, bool nsharpBoundaryEdges, bool unmarkExtrudedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	mesh->expandMarkedEdges( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, markBoundaryEdges, nsharpBoundaryEdges, unmarkExtrudedEdges );
	lock();
}


void GSProductMesh::bevelMarkedEdges(MeshBevelEdgesAdjuster &adjuster, bool useExistingTexturing, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	mesh->bevelMarkedEdges( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, useExistingTexturing, markBoundaryEdges, nsharpBoundaryEdges );
	lock();
}


int GSProductMesh::extrudeMarkedEdges(MeshExtrudeEdgesAdjuster &adjuster, int numSegments, bool useExistingTextures, bool bMarkExtrudedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	int result = mesh->extrudeMarkedEdges( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, numSegments, useExistingTextures, bMarkExtrudedEdges );
	lock();

	return result;
}


int GSProductMesh::fillMarkedEdges()
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->fillMarkedEdges();
	lock();

	return result;
}


int GSProductMesh::riftMarkedEdges(MeshRiftEdgesAdjuster &adjuster, MEdgeRiftStyle style, int numSegments, bool fill, bool pinch)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	int result = mesh->riftMarkedEdges( *adjuster.vertexAdjust.adjust, style, numSegments, fill, pinch );
	lock();

	return result;
}


void GSProductMesh::slideMarkedEdges(MeshSlideEdgesAdjuster &adjuster, int slideVersion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );
	mesh->slideMarkedEdges( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, slideVersion );
	lock();
}


void GSProductMesh::connectMarkedEdges(const MPickList &cutPickList, const MPickList &terminalVertexPickList, bool unmarkOriginalEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->connectMarkedEdges( cutPickList, terminalVertexPickList, unmarkOriginalEdges );
	lock();
}

bool GSProductMesh::connectMarkedEdgesGetRingSegments(int seedEdgeIndex, Array<Segment3> &ringSegments)
{
	unlock();
	MMesh *mesh = getRepMesh();
	MEdge *seedEdge = mesh->getEdge( seedEdgeIndex );
	bool result = mesh->connectMarkedEdgesGetRingSegments( seedEdge, ringSegments );
	lock();
	return result;
}

void GSProductMesh::connectMarkedEdgesGetTerminalVertices(const MPickList &terminalVertexPickList, Array<int> &terminalVertexIndices)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MVertexList terminalVertices;
	mesh->connectMarkedEdgesGetTerminalVertices( terminalVertexPickList, terminalVertices );

	terminalVertexIndices.resize( terminalVertices.size() );
	for (int vertexI = 0; vertexI < terminalVertices.size(); vertexI++)
	{
		terminalVertexIndices[vertexI] = terminalVertices[vertexI]->getIndex();
	}

	lock();
}

boost::python::tuple GSProductMesh::py_connectMarkedEdgesGetRingSegments(int seedEdgeIndex)
{
	Array<Segment3> ringSegments;
	bool closed = connectMarkedEdgesGetRingSegments( seedEdgeIndex, ringSegments );
	boost::python::list pyRingSegs;
	for (int i = 0; i < ringSegments.size(); i++)
	{
		pyRingSegs.append( ringSegments[i] );
	}
	return boost::python::make_tuple( pyRingSegs, closed );
}

boost::python::list GSProductMesh::py_connectMarkedEdgesGetTerminalVertices(const MPickList &terminalVertexPickList)
{
	Array<int> terminalVertexIndices;
	connectMarkedEdgesGetTerminalVertices( terminalVertexPickList, terminalVertexIndices );
	boost::python::list pyTerminalVertexIndices;
	for (int i = 0; i < terminalVertexIndices.size(); i++)
	{
		pyTerminalVertexIndices.append( terminalVertexIndices[i] );
	}
	return pyTerminalVertexIndices;
}

void GSProductMesh::nConnectMarkedEdges(int numSegments)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->nConnectMarkedEdges( numSegments );
	lock();
}

void GSProductMesh::splitEdges(const MPickList &pickList)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->splitEdges( pickList );
	lock();
}

void GSProductMesh::nSplitMarkedEdges(int numSegments)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->nSplitMarkedEdges( numSegments );
	lock();
}

void GSProductMesh::cut(const MCutTargetList &cutTargets, MeshCutState *cutState)
{
	unlock();

	MMesh *mesh = getRepMesh();

	MVertex *lastSplitVertex;
	MFace *currentFace;
	mesh->cut( cutTargets, lastSplitVertex, currentFace );

	if ( cutState != NULL )
	{
		if ( lastSplitVertex != NULL )
		{
			cutState->lastSplitVertexIndex = lastSplitVertex->getIndex();
			cutState->lastSplitVertexPosition = lastSplitVertex->getPosition();
		}
		else
		{
			cutState->lastSplitVertexIndex = -1;
		}

		if ( currentFace != NULL )
		{
			cutState->currentFaceIndex = currentFace->getIndex();
		}
		else
		{
			cutState->currentFaceIndex = -1;
		}
	}

	lock();
}

bool GSProductMesh::isValidCutTarget(const MCutTarget &target, const MeshCutState &cutState)
{
	unlock();

	MMesh *mesh = getRepMesh();

	MVertex *lastSplitVertex = NULL;
	MFace *currentFace = NULL;

	if ( cutState.lastSplitVertexIndex != -1 )
	{
		lastSplitVertex = mesh->getVertex( cutState.lastSplitVertexIndex );
	}

	if ( cutState.currentFaceIndex != -1 )
	{
		currentFace = mesh->getFace( cutState.currentFaceIndex );
	}


	bool result = mesh->isValidCutTarget( target, lastSplitVertex, currentFace );

	lock();

	return result;
}


void GSProductMesh::nCut(int numSegments, const MCutTargetList &cutTargets, MeshNCutState *cutState)
{
	unlock();
	MMesh *mesh = getRepMesh();

	if ( cutState != NULL )
	{
		MVertexList lastSplitVertices;

		mesh->nCut( numSegments, cutTargets, &lastSplitVertices );

		cutState->lastSplitVertexIndices.resize( lastSplitVertices.size() );
		cutState->lastSplitVertexPositions.resize( lastSplitVertices.size() );

		for (int i = 0; i < lastSplitVertices.size(); i++)
		{
			cutState->lastSplitVertexIndices[i] = lastSplitVertices[i]->getIndex();
			cutState->lastSplitVertexPositions[i] = lastSplitVertices[i]->getPosition();
		}
	}
	else
	{
		mesh->nCut( numSegments, cutTargets );
	}

	lock();
}

bool GSProductMesh::isValidNCutTarget(const MCutTarget &target, const MeshNCutState &cutState)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MVertexList lastSplitVertices;

	lastSplitVertices.resize( cutState.lastSplitVertexIndices.size() );

	for (int i = 0; i < cutState.lastSplitVertexIndices.size(); i++)
	{
		lastSplitVertices[i] = mesh->getVertex( cutState.lastSplitVertexIndices[i] );
	}

	bool result = mesh->isValidNCutTarget( target, lastSplitVertices );

	lock();

	return result;
}



void GSProductMesh::bandsaw(const MBandsawTargetList &targetList)
{
	unlock();
	MMesh *mesh = getRepMesh();

	mesh->bandsaw( targetList );

	lock();
}

void GSProductMesh::nBandsaw(const MNBandsawTargetList &targetList)
{
	unlock();
	MMesh *mesh = getRepMesh();

	mesh->nBandsaw( targetList );

	lock();
}

bool GSProductMesh::bandsawGetRingSegments(int seedEdgeIndex, int seedFaceIndex, Array<Segment3> &ringSegments, bool stopAtMarkedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MEdge *seed = mesh->getEdge( seedEdgeIndex );
	MFace *seedFace = NULL;

	if ( seedFaceIndex != -1 )
	{
		seedFace = mesh->getFace( seedFaceIndex );
	}

	bool result = mesh->bandsawGetRingSegments( seed, seedFace, ringSegments, stopAtMarkedEdges );

	lock();

	return result;
}


boost::python::tuple GSProductMesh::py_bandsawGetRingSegments(int seedEdgeIndex, int seedFaceIndex, bool stopAtMarkedEdges)
{
	Array<Segment3> ringSegments;
	bool closed = bandsawGetRingSegments( seedEdgeIndex, seedFaceIndex, ringSegments, stopAtMarkedEdges );
	boost::python::list pyRingSegs;
	for (int i = 0; i < ringSegments.size(); i++)
	{
		pyRingSegs.append( ringSegments[i] );
	}
	return boost::python::make_tuple( pyRingSegs, closed );
}


int GSProductMesh::bridge(const PolyBlend *blend, int segments)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->bridge( blend, segments );
	lock();

	return result;
}


int GSProductMesh::weldMarkedEdges(const PolyBlend *blend, MWeldStyle style)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->weldMarkedEdges( blend, style );
	lock();

	return result;
}


int GSProductMesh::collapseMarkedEdges(bool lockMarkedVertices)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->collapseMarkedEdges( lockMarkedVertices );
	lock();

	return result;
}


int GSProductMesh::dissolveMarkedEdges(MEdgeDissolveStyle dissolveStyle, bool dissolveSimpleValenceTwoVertices)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->dissolveMarkedEdges( dissolveStyle, dissolveSimpleValenceTwoVertices );
	lock();

	return result;
}


int GSProductMesh::spinMarkedEdges(int turns)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->spinMarkedEdges( turns );
	lock();

	return result;
}


int GSProductMesh::rewireEdges(const MRewireEdgeTargetList &rewireTargets)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->rewireEdges( rewireTargets );
	lock();
	return result;
}

bool GSProductMesh::isValidRewireEdgeTarget(int edgeToRewireIndex, int sourceVertexIndex, int targetVertexIndex)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MEdge *edgeToRewire = mesh->getEdges()[edgeToRewireIndex];
	MVertex *sourceVertex = mesh->getVertices()[sourceVertexIndex];
	MVertex *targetVertex = mesh->getVertices()[targetVertexIndex];

	bool result = mesh->isValidRewireEdgeTarget( edgeToRewire, sourceVertex, targetVertex );

	lock();

	return result;
}

bool GSProductMesh::canEdgeBeRewired(int edgeToRewireIndex)
{
	unlock();
	MMesh *mesh = getRepMesh();

	MEdge *edgeToRewire = mesh->getEdges()[edgeToRewireIndex];

	bool result = edgeToRewire->canRewire();

	lock();

	return result;
}


void GSProductMesh::extractMarkedEdgesAsPolylines(Array<Polyline> &polylines)
{
	//get the edge runs from the mesh
	MMesh *mesh = getReadOnlyRepMesh();
	Array<MEdgeRun> edgeRuns;
	mesh->discoverMarkedEdgeRuns( edgeRuns );

	polylines.clear();
	polylines.reserve( edgeRuns.size() );


	const Matrix4 &localMatrix = getLocalTransformation();


	for (int i = 0; i < edgeRuns.size(); i++)
	{
		Polyline &p = polylines.push_back();
		convertEdgeRunToPolyline( edgeRuns[i], p );
		p.transform( localMatrix, false );
	}
}

void GSProductMesh::setMarkedEdgeSharpness(double sharpness, MSharpenFunction sharpenFunction)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->setMarkedEdgeSharpness( sharpness, sharpenFunction );
	lock();
}

void GSProductMesh::saveMarkedEdgeSharpness(MeshSharpnessBackup &sharpnessBackup)
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveMarkedEdgeSharpness( sharpnessBackup.sharpnessBackup );
}

void GSProductMesh::restoreMarkedEdgeSharpness(MeshSharpnessBackup &sharpnessBackup)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->restoreMarkedEdgeSharpness( sharpnessBackup.sharpnessBackup );
	lock();
}


void GSProductMesh::setMarkedEdgeNormalSharpness(bool sharpness)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->setMarkedEdgeNormalSharpness( sharpness );
	lock();
}




//FACE OPS

void GSProductMesh::insetMarkedFaces(MeshInsetFacesAdjuster &adjuster, MInsetFaceStyle style, bool flowThruMarkedEdges,
								bool markBoundaryEdges, bool markInsetEdges)
{
	unlock();

	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );

	mesh->insetMarkedFaces( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, style, flowThruMarkedEdges, markBoundaryEdges, markInsetEdges, false, false );

	lock();
}


void GSProductMesh::bevelMarkedFaces(MeshBevelFacesAdjuster &adjuster, MInsetFaceStyle style, bool flowThruMarkedEdges,
								bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges)
{
	unlock();

	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );

	mesh->insetMarkedFaces( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, style, flowThruMarkedEdges, markBoundaryEdges, markInsetEdges, nsharpBoundaryEdges, nsharpInsetEdges );

	lock();
}


void GSProductMesh::insetMarkedFacesSingular(MeshInsetFacesAdjuster &adjuster, MInsetFaceStyle style, bool markBoundaryEdges, bool markInsetEdges)
{
	unlock();

	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );

	mesh->insetMarkedFacesSingular( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, style, markBoundaryEdges, markInsetEdges, false, false );

	lock();
}


void GSProductMesh::bevelMarkedFacesSingular(MeshBevelFacesAdjuster &adjuster, MInsetFaceStyle style,
									bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges)
{
	unlock();

	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );

	mesh->insetMarkedFacesSingular( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, style, markBoundaryEdges, markInsetEdges, nsharpBoundaryEdges, nsharpInsetEdges );

	lock();
}


void GSProductMesh::expandMarkedFaces(MeshExpandFacesAdjuster &adjuster,  bool markBoundaryEdges, bool markExpansionEdges, bool nsharpBoundaryEdges, bool nsharpExpansionEdges)
{
	unlock();

	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	adjuster.attribAdjust.setMesh( this );

	mesh->expandMarkedFaces( *adjuster.vertexAdjust.adjust, *adjuster.attribAdjust.adjust, markBoundaryEdges, markExpansionEdges, nsharpBoundaryEdges, nsharpExpansionEdges );

	lock();
}


void GSProductMesh::insetMarkedFacesToPoints(MeshVertexAdjustment &vertexAdjusts, int segments, bool markBoundaryEdges, bool nsharpBoundaryEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	vertexAdjusts.setMesh( this );
	mesh->insetMarkedFacesToPoints( *vertexAdjusts.adjust, segments, markBoundaryEdges, nsharpBoundaryEdges );
	lock();
}


void GSProductMesh::extrudeMarkedFaces(MeshExtrudeFacesAdjuster &adjuster, MExtrudeFaceDirection extrudeDirection, int numSegments,
									bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->extrudeMarkedFaces( *adjuster.vertexAdjust.adjust, extrudeDirection, numSegments, markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges );
	lock();
}

void GSProductMesh::extrudeMarkedFacesSingular(MeshExtrudeFacesAdjuster &adjuster, int numSegments,
											bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->extrudeMarkedFacesSingular( *adjuster.vertexAdjust.adjust, numSegments, markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges );
	lock();
}


int GSProductMesh::pathExtrudeMarkedFaces(const Polyline &extrusionPath, MPathExtrudeFaceMode mode, bool correctiveScaling, PolylinePathOrientation orientation, double pathRotation,
										bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->pathExtrudeMarkedFaces( extrusionPath, mode, correctiveScaling, orientation, pathRotation,
										markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges );
	lock();

	return result;
}


void GSProductMesh::vectorExtrudeMarkedFaces(MeshVectorExtrudeFacesAdjuster &adjuster, int numSegments,
											bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->extrudeMarkedFaces( *adjuster.vertexAdjust.adjust, numSegments, markBoundaryEdges, markExtrudedEdges, nsharpBoundaryEdges, nsharpExtrudedEdges );
	lock();
}


void GSProductMesh::duplicateMarkedFaces(MeshVectorExtrudeFacesAdjuster &adjuster,bool markOriginalFaces, bool markDuplicateFaces)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->duplicateMarkedFaces( *adjuster.vertexAdjust.adjust, markOriginalFaces, markDuplicateFaces );
	lock();
}


void GSProductMesh::roundMarkedFaces()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->roundMarkedFaces();
	lock();
}


int GSProductMesh::tunnel(const PolyBlend *blend, int segments)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->tunnel( blend, segments );
	lock();

	return result;
}


int GSProductMesh::weldMarkedFaces(const PolyBlend *blend, MWeldStyle style)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->weldMarkedFaces( blend, style );
	lock();

	return result;
}


int GSProductMesh::collapseMarkedFaces()
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->collapseMarkedFaces();
	lock();

	return result;
}


int GSProductMesh::dissolveMarkedFaces(bool bMarkMergedFaces)
{
	unlock();
	MMesh *mesh = getRepMesh();
	int result = mesh->dissolveMarkedFaces( bMarkMergedFaces );
	lock();

	return result;
}


void GSProductMesh::removeMarkedFaces()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->removeMarkedFaces();
	lock();
}



void GSProductMesh::drawQuadsWithState(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip,
								MeshDrawQuadsState *state)
{
	unlock();
	MMesh *mesh = getRepMesh();
	state->init( this );
	mesh->drawQuads( points, bCloseRows, bCloseColumns, numColumns, bFlip, *state->state );
	lock();
}

void GSProductMesh::drawQuads(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->drawQuads( points, bCloseRows, bCloseColumns, numColumns, bFlip );
	lock();
}




void GSProductMesh::drawFaceWithState(const MDrawFacePointList &points, bool bFlip, MeshDrawFaceState *state)
{
	unlock();
	MMesh *mesh = getRepMesh();
	state->init( this );
	mesh->drawFace( points, bFlip, *state->state );
	lock();
}

void GSProductMesh::drawFace(const MDrawFacePointList &points, bool bFlip)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->drawFace( points, bFlip );
	lock();
}






//MARK OPS

void GSProductMesh::markVertices_save()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->markVertices_save();
}

void GSProductMesh::markVertices_restore()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_restore();
	lock();
}

void GSProductMesh::markEdges_save()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->markEdges_save();
}

void GSProductMesh::markEdges_restore()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_restore();
	lock();
}

void GSProductMesh::markFaces_save()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->markFaces_save();
}

void GSProductMesh::markFaces_restore()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_restore();
	lock();
}



void GSProductMesh::markVertices_all(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_all( pred );
	lock();
}

void GSProductMesh::markEdges_all(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_all( pred );
	lock();
}

void GSProductMesh::markFaces_all(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_all( pred );
	lock();
}



void GSProductMesh::markVertices_edgeMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_edgeMarked( pred );
	lock();
}

void GSProductMesh::markVertices_faceMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_faceMarked( pred );
	lock();
}

void GSProductMesh::markVertices_faceMarkBoundary(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_faceMarkBoundary( pred );
	lock();
}

void GSProductMesh::markVertices_faceMarkInternal(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_faceMarkInternal( pred );
	lock();
}

void GSProductMesh::markVertices_boundary(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_boundary( pred );
	lock();
}

void GSProductMesh::markVertices_internal(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_internal( pred );
	lock();
}

void GSProductMesh::markEdges_vertexMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_vertexMarked( pred );
	lock();
}

void GSProductMesh::markEdges_whollyVertexMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_whollyVertexMarked( pred );
	lock();
}

void GSProductMesh::markEdges_faceMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_faceMarked( pred );
	lock();
}

void GSProductMesh::markEdges_faceMarkBoundary(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_faceMarkBoundary( pred );
	lock();
}

void GSProductMesh::markEdges_faceMarkInternal(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_faceMarkInternal( pred );
	lock();
}

void GSProductMesh::markEdges_boundary(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_boundary( pred );
	lock();
}

void GSProductMesh::markEdges_internal(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_internal( pred );
	lock();
}

void GSProductMesh::markFaces_vertexMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_vertexMarked( pred );
	lock();
}

void GSProductMesh::markFaces_whollyVertexMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_whollyVertexMarked( pred );
	lock();
}

void GSProductMesh::markFaces_edgeMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_edgeMarked( pred );
	lock();
}

void GSProductMesh::markFaces_whollyEdgeMarked(MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_whollyEdgeMarked( pred );
	lock();
}



void GSProductMesh::markVertices_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred)
{
	unlock();
	MMesh *mesh = getRepMesh();
	if ( faceCull != NULL )
	{
		mesh->markVertices_region( region, faceCull, pred );
	}
	else
	{
		mesh->markVertices_region( region, pred );
	}
	lock();
}

void GSProductMesh::markEdges_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf)
{
	unlock();
	MMesh *mesh = getRepMesh();
	if ( faceCull != NULL )
	{
		mesh->markEdges_region( region, faceCull, pred, containsAllOf );
	}
	else
	{
		mesh->markEdges_region( region, pred, containsAllOf );
	}
	lock();
}

void GSProductMesh::markFaces_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf)
{
	unlock();
	MMesh *mesh = getRepMesh();
	if ( faceCull != NULL )
	{
		mesh->markFaces_region( region, faceCull, pred, containsAllOf );
	}
	else
	{
		mesh->markFaces_region( region, pred, containsAllOf );
	}
	lock();
}




void GSProductMesh::markVertices_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markVertices_pick( pick );
	lock();
}

void GSProductMesh::markEdges_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdges_pick( pick );
	lock();
}

void GSProductMesh::markEdgeRings_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeRings_pick( pick );
	lock();
}

void GSProductMesh::markEdgeLoops_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeLoops_pick( pick );
	lock();
}

void GSProductMesh::markEdgeBoundaries_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeBoundaries_pick( pick );
	lock();
}

void GSProductMesh::markEdgeLoopPath_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeLoopPath_pick( pick );
	lock();
}

void GSProductMesh::markEdgeRingPath_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeRingPath_pick( pick );
	lock();
}

void GSProductMesh::markFaces_pick(const MPickList &pick)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markFaces_pick( pick );
	lock();
}


void GSProductMesh::markEdgeRing(bool stopAtMarkedFaces, bool traverseFacesMultipleTimes)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeRing( stopAtMarkedFaces, traverseFacesMultipleTimes );
	lock();
}

void GSProductMesh::markEdgeLoop(bool stopAtMarkedVertices, bool traverseVerticesMultipleTimes)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeLoop( stopAtMarkedVertices, traverseVerticesMultipleTimes );
	lock();
}

void GSProductMesh::markEdgeBoundary()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->markEdgeBoundary();
	lock();
}


void GSProductMesh::growMarkedVertices()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->growMarkedVertices();
	lock();
}

void GSProductMesh::shrinkMarkedVertices()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->shrinkMarkedVertices();
	lock();
}

void GSProductMesh::growMarkedEdges()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->growMarkedEdges();
	lock();
}

void GSProductMesh::shrinkMarkedEdges()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->shrinkMarkedEdges();
	lock();
}

void GSProductMesh::growMarkedFaces()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->growMarkedFaces();
	lock();
}

void GSProductMesh::shrinkMarkedFaces()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->shrinkMarkedFaces();
	lock();
}






//TRANSFORMATION OPS

void GSProductMesh::initVertexTransformation(MeshTransformation &meshXform, bool perGroup)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformVerticesInit( *meshXform.target, perGroup );
	meshXform.setMesh( this );
	lock();
}

void GSProductMesh::initEdgeTransformation(MeshTransformation &meshXform, bool perGroup)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformEdgesInit( *meshXform.target, perGroup );
	meshXform.setMesh( this );
	lock();
}

void GSProductMesh::initFaceTransformation(MeshTransformation &meshXform, bool perGroup)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformFacesInit( *meshXform.target, perGroup );
	meshXform.setMesh( this );
	lock();
}




void GSProductMesh::transformVertices(const Array<int> &vertexIndices, const Matrix4 &xform)
{
	unlock();
	MMesh *mesh = getRepMesh();
	Array<MVertex*> &vertices = mesh->getVertices();
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		vertices[ vertexIndices[i] ]->transformPosition( xform );
	}
	lock();
}

void GSProductMesh::transformMarkedVertices(const Matrix4 &xform)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformVertices( xform, true );
	lock();
}

void GSProductMesh::saveVertexPositions(const Array<int> &vertexIndices)
{
	MMesh *mesh = getReadOnlyRepMesh();
	Array<MVertex*> &vertices = mesh->getVertices();
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		vertices[ vertexIndices[i] ]->savePosition();
	}
}

void GSProductMesh::restoreVertexPositions(const Array<int> &vertexIndices)
{
	unlock();
	MMesh *mesh = getRepMesh();
	Array<MVertex*> &vertices = mesh->getVertices();
	for (int i = 0; i < vertexIndices.size(); i++)
	{
		vertices[ vertexIndices[i] ]->restorePosition();
	}
	lock();
}

void GSProductMesh::saveMarkedVertexPositions()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveVertexPositions( true );
}

void GSProductMesh::restoreMarkedVertexPositions()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->restoreVertexPositions( true );
	lock();
}


void GSProductMesh::transformEdgeMarkedVertices(const Matrix4 &xform)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformEdgeMarkedVertices( xform );
	lock();
}

void GSProductMesh::saveEdgeMarkedVertexPositions()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveEdgeMarkedVertexPositions();
}

void GSProductMesh::restoreEdgeMarkedVertexPositions()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->restoreEdgeMarkedVertexPositions();
	lock();
}


void GSProductMesh::transformFaceMarkedVertices(const Matrix4 &xform)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformFaceMarkedVertices( xform );
	lock();
}

void GSProductMesh::saveFaceMarkedVertexPositions()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveFaceMarkedVertexPositions();
}

void GSProductMesh::restoreFaceMarkedVertexPositions()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->restoreFaceMarkedVertexPositions();
	lock();
}


void GSProductMesh::transformMarkedComponents(const Matrix4 &xform)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformMarkedComponents( xform );
	lock();
}

void GSProductMesh::saveMarkedComponentPositions()
{
	MMesh *mesh = getReadOnlyRepMesh();
	mesh->saveMarkedComponentPositions();
}

void GSProductMesh::restoreMarkedComponentPositions()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->restoreMarkedComponentPositions();
	lock();
}




// TWEAK OPERATIONS

void GSProductMesh::tweakComponents(const MTweakComponentList &tweaks, bool bRestoreSelection)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->tweakComponents( tweaks, bRestoreSelection );
	lock();
}


void GSProductMesh::surfaceTweakComponents(const MSurfaceTweakComponentList &tweaks, bool bRestoreSelection)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->surfaceTweakComponents( tweaks, bRestoreSelection );
	lock();
}



void GSProductMesh::surfaceTweakAdjustMarkedVertices(MeshSurfaceTweakAdjuster &adjuster, const Vector3 &viewVector)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->surfaceTweakAdjustMarkedVertices( *adjuster.vertexAdjust.adjust, viewVector );
	lock();
}

void GSProductMesh::surfaceTweakAdjustMarkedEdges(MeshSurfaceTweakAdjuster &adjuster, const Vector3 &viewVector)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->surfaceTweakAdjustMarkedEdges( *adjuster.vertexAdjust.adjust, viewVector );
	lock();
}

void GSProductMesh::surfaceTweakAdjustMarkedFaces(MeshSurfaceTweakAdjuster &adjuster, const Vector3 &viewVector)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->surfaceTweakAdjustMarkedFaces( *adjuster.vertexAdjust.adjust, viewVector );
	lock();
}

void GSProductMesh::surfaceTweakNormalAdjustMarkedVertices(MeshSurfaceTweakNormalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->surfaceTweakNormalAdjustMarkedVertices( *adjuster.vertexAdjust.adjust );
	lock();
}

void GSProductMesh::surfaceTweakNormalAdjustMarkedEdges(MeshSurfaceTweakNormalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->surfaceTweakNormalAdjustMarkedEdges( *adjuster.vertexAdjust.adjust );
	lock();
}

void GSProductMesh::surfaceTweakNormalAdjustMarkedFaces(MeshSurfaceTweakNormalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->surfaceTweakNormalAdjustMarkedFaces( *adjuster.vertexAdjust.adjust );
	lock();
}




void GSProductMesh::repositionVertices(const MVertexRepositionList &targets)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->repositionVertices( targets );
	lock();
}

void GSProductMesh::setVertexPosition(int vertexIndex, const Point3 &position)
{
	unlock();
	MMesh *mesh = getRepMesh();
	MVertex *v = mesh->getVertices()[vertexIndex];
	mesh->setVertexPosition( v, position );
	lock();
}




// PROPORTIONAL OPERATIONS

void GSProductMesh::proportionalAdjusterInit(const Point3 &brushPosition, const Brush &brush, MeshProportionalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.setMesh( this );
	mesh->proportionalAdjusterInit( brushPosition, brush, adjuster.proportionalAdjuster );
	lock();
}


void GSProductMesh::proportionalMove(const Point3 &brushPosition, const Brush &brush, const Vector3 &translation, MeshProportionalAdjuster *adjuster)
{
	MProportionalAdjuster *mAdj = NULL;
	if ( adjuster != NULL )
	{
		mAdj = adjuster->proportionalAdjuster;
	}

	unlock();
	MMesh *mesh = getRepMesh();
	mesh->proportionalMove( brushPosition, brush, translation, mAdj );
	lock();
}

void GSProductMesh::proportionalMoveNormal(const Point3 &brushPosition, const Brush &brush, double distance, MeshProportionalAdjuster *adjuster)
{
	MProportionalAdjuster *mAdj = NULL;
	if ( adjuster != NULL )
	{
		mAdj = adjuster->proportionalAdjuster;
	}

	unlock();
	MMesh *mesh = getRepMesh();
	mesh->proportionalMoveNormal( brushPosition, brush, distance, mAdj );
	lock();
}

void GSProductMesh::proportionalScale(const Point3 &brushPosition, const Brush &brush, double scaleFactor, MeshProportionalAdjuster *adjuster)
{
	MProportionalAdjuster *mAdj = NULL;
	if ( adjuster != NULL )
	{
		mAdj = adjuster->proportionalAdjuster;
	}

	unlock();
	MMesh *mesh = getRepMesh();
	mesh->proportionalScale( brushPosition, brush, scaleFactor, mAdj );
	lock();
}

void GSProductMesh::proportionalRotate(const Point3 &brushPosition, const Brush &brush, const Vector3 &rotationAxis, double rotation, MeshProportionalAdjuster *adjuster)
{
	MProportionalAdjuster *mAdj = NULL;
	if ( adjuster != NULL )
	{
		mAdj = adjuster->proportionalAdjuster;
	}

	unlock();
	MMesh *mesh = getRepMesh();
	mesh->proportionalRotate( brushPosition, brush, rotationAxis, rotation, mAdj );
	lock();
}

void GSProductMesh::proportionalRelax(const Point3 &brushPosition, const Brush &brush, double relaxAmount, MeshProportionalAdjuster *adjuster)
{
	MProportionalAdjuster *mAdj = NULL;
	if ( adjuster != NULL )
	{
		mAdj = adjuster->proportionalAdjuster;
	}

	unlock();
	MMesh *mesh = getRepMesh();
	mesh->proportionalRelax( brushPosition, brush, relaxAmount, mAdj );
	lock();
}




// MOVE-NORMAL OPERATIONS

void GSProductMesh::moveVerticesNormal(MeshMoveNormalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->moveVerticesNormal( *adjuster.vertexAdjust.adjust );
	lock();
}

void GSProductMesh::moveEdgesNormal(MeshMoveNormalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->moveEdgesNormal( *adjuster.vertexAdjust.adjust );
	lock();
}

void GSProductMesh::moveFacesNormal(MeshMoveNormalAdjuster &adjuster)
{
	unlock();
	MMesh *mesh = getRepMesh();
	adjuster.vertexAdjust.setMesh( this );
	mesh->moveFacesNormal( *adjuster.vertexAdjust.adjust );
	lock();
}




// FLATTEN OPERATIONS

void GSProductMesh::flattenVertices(const Vector3 &axis, bool perRegion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenVertices( axis, perRegion );
	lock();
}

void GSProductMesh::flattenVerticesNormal(bool perRegion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenVerticesNormal( perRegion );
	lock();
}

void GSProductMesh::flattenEdges(const Vector3 &axis, bool perRegion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenEdges( axis, perRegion );
	lock();
}

void GSProductMesh::flattenEdgesNormal(bool perRegion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenEdgesNormal( perRegion );
	lock();
}

void GSProductMesh::flattenEdgeRuns()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenEdgeRuns();
	lock();
}

void GSProductMesh::straightenEdgeRuns()
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->straightenEdgeRuns();
	lock();
}

void GSProductMesh::flattenFaces(const Vector3 &axis, bool perRegion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenFaces( axis, perRegion );
	lock();
}

void GSProductMesh::flattenFacesNormal(bool perRegion)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->flattenFacesNormal( perRegion );
	lock();
}






//MATERIAL BY MARKED REGION

void GSProductMesh::setMaterial(int mat)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->setMaterial( mat );
	lock();
}

void GSProductMesh::transformMaterial(const Matrix4 &xform, bool fixMarkedVertices)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->transformMaterial( xform, fixMarkedVertices );
	lock();
}

void GSProductMesh::transformMaterialSave(bool fixMarkedVertices, MeshMaterialTransformBackup &b)
{
	MMesh *mesh = getReadOnlyRepMesh();

	b.fixMarkedVertices = fixMarkedVertices;

	mesh->transformMaterialSave( fixMarkedVertices, *b.backup );
}

void GSProductMesh::normaliseMaterialCoords(bool perGroup)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->normaliseMaterialCoords( perGroup );
	lock();
}

void GSProductMesh::transformMaterialRestore(const MeshMaterialTransformBackup &b)
{
	unlock();
	MMesh *mesh = getRepMesh();

	mesh->transformMaterialRestore( b.fixMarkedVertices, *b.backup );

	lock();
}



//MATERIAL UV MAPPING

void GSProductMesh::uvMap(UVMapper *mapper)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->uvMap( mapper );
	lock();
}

void GSProductMesh::uvmapMarkedEdges(bool syncAtMarkedVertices, bool invert, const Point2f &uvStart, const Point2f &uvEnd)
{
	unlock();
	MMesh *mesh = getRepMesh();
	mesh->uvmapMarkedEdges( syncAtMarkedVertices, invert, uvStart, uvEnd );
	lock();
}



void GSProductMesh::getPoints(Array<Point3> &points) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	mesh->getPoints( points );

	if ( hasLocalTransformation() )
	{
		const Matrix4 &localMatrix = getLocalTransformation();
		for (int i = 0; i < points.size(); i++)
		{
			points[i] *= localMatrix;
		}
	}
}




Point3 GSProductMesh::getVertexPosition(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getVertices()[index]->getPosition()  *  getLocalTransformation();
	}
	else
	{
		return mesh->getVertices()[index]->getPosition();
	}
}

bool GSProductMesh::isVertexMarked(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasMarkOverride() )
	{
		return getMarkOverride();
	}
	else
	{
		return mesh->getVertices()[index]->isVertexMarked();
	}
}


Point2f GSProductMesh::getVertexTexturePoint(int vertexIndex, int textureIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVertices()[vertexIndex]->getVertexAttrib( textureIndex )->getPoint();
}

int GSProductMesh::getNumVertexTextures(int vertexIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVertices()[vertexIndex]->getNumVertexAttribs();
}


Vector3f GSProductMesh::getVertexNormal(int vertexIndex, int normalIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getVertices()[vertexIndex]->getVertexNormal( normalIndex )->getNormal()  *  getLocalTransformation();
	}
	else
	{
		return mesh->getVertices()[vertexIndex]->getVertexNormal( normalIndex )->getNormal();
	}
}

int GSProductMesh::getNumVertexNormals(int vertexIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getVertices()[vertexIndex]->getNumVertexNormals();
}



Segment3 GSProductMesh::getEdgeSegment(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getEdges()[index]->getSegment()  *  getLocalTransformation();
	}
	else
	{
		return mesh->getEdges()[index]->getSegment();
	}
}

bool GSProductMesh::isEdgeMarked(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	return mesh->getEdges()[index]->isEdgeMarked();
}

bool GSProductMesh::isEdgeBoundary(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	return mesh->getEdges()[index]->isBoundary();
}

int GSProductMesh::getEdgeVertexAIndex(int edgeIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	const MEdge *edge = mesh->getEdges()[edgeIndex];

	return edge->getVertexA()->getIndex();
}

int GSProductMesh::getEdgeVertexBIndex(int edgeIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	const MEdge *edge = mesh->getEdges()[edgeIndex];

	return edge->getVertexB()->getIndex();
}

int GSProductMesh::getEdgeOppositeVertexIndex(int edgeIndex, int vertexIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	const MEdge *edge = mesh->getEdges()[edgeIndex];
	const MVertex *vertex = mesh->getVertices()[vertexIndex];

	if ( edge->isIncidentTo( vertex ) )
	{
		return edge->getOppositeVertex( vertex )->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::getEdgeFaceAIndex(int edgeIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	const MEdge *edge = mesh->getEdges()[edgeIndex];

	const MFace *face = edge->getFaceA();

	if ( face != NULL )
	{
		return face->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::getEdgeFaceBIndex(int edgeIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	const MEdge *edge = mesh->getEdges()[edgeIndex];

	const MFace *face = edge->getFaceB();

	if ( face != NULL )
	{
		return face->getIndex();
	}
	else
	{
		return -1;
	}
}

int GSProductMesh::getEdgeOppositeFaceIndex(int edgeIndex, int faceIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	const MEdge *edge = mesh->getEdges()[edgeIndex];
	const MFace *face = mesh->getFaces()[faceIndex];

	if ( face->isIncidentTo( edge ) )
	{
		const MFace *oppositeFace = edge->getOppositeFace( face );

		if ( oppositeFace != NULL )
		{
			return oppositeFace->getIndex();
		}
	}

	return -1;
}


int GSProductMesh::getFaceSize(int faceIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getFaces()[faceIndex]->getSize();
}

void GSProductMesh::getFaceVertex(int faceIndex, int faceVertexIndex, int &vertexIndex, int &textureIndex, int &normalIndex) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	const MFace *face = mesh->getFaces()[faceIndex];
	vertexIndex = face->getVertex( faceVertexIndex )->index;
	textureIndex = face->getVertexAttrib( faceVertexIndex )->index;
	normalIndex = face->getVertexNormal( faceVertexIndex )->index;
}


int GSProductMesh::getNumTrianglesInFace(int index)
{
	const MMesh *mesh = getReadOnlyRepMesh();

	return mesh->getFaces()[index]->getNumTriangles();
}

IndexTriangle GSProductMesh::getFaceIndexTriangle(int faceIndex, int triIndex)
{
	MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getFaces()[faceIndex]->getIndexTriangle( triIndex );
}

Triangle3 GSProductMesh::getFaceTriangle(int faceIndex, int triIndex)
{
	MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getFaces()[faceIndex]->getTriangle( triIndex )  *  getLocalTransformation();
	}
	else
	{
		return mesh->getFaces()[faceIndex]->getTriangle( triIndex );
	}
}

bool GSProductMesh::isFaceMarked(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	return mesh->getFaces()[index]->isFaceMarked();
}

int GSProductMesh::getFaceMaterialID(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getFaces()[index]->getMaterialID();
}

Point3 GSProductMesh::getFaceCentroid(int index) const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	return mesh->getFaces()[index]->computeCentroid();
}


boost::python::tuple GSProductMesh::py_getFaceVertex(int faceIndex, int faceVertexIndex)
{
	int vertexIndex, textureIndex, normalIndex;
	getFaceVertex( faceIndex, faceVertexIndex, vertexIndex, textureIndex, normalIndex );
	return boost::python::make_tuple( vertexIndex, textureIndex, normalIndex );
}

boost::python::list GSProductMesh::py_getFaceIndexTriangles(int index)
{
	boost::python::list result;
	int numTris = getNumTrianglesInFace( index );
	for (int i = 0; i < numTris; i++)
	{
		result.append( getFaceIndexTriangle( index, i ) );
	}
	return result;
}

boost::python::list GSProductMesh::py_getFaceTriangles(int index)
{
	boost::python::list result;
	int numTris = getNumTrianglesInFace( index );
	for (int i = 0; i < numTris; i++)
	{
		result.append( getFaceTriangle( index, i ) );
	}
	return result;
}







BBox3 GSProductMesh::getMarkedVerticesBBox() const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getVerticesBBox( getLocalTransformation(), true );
	}
	else
	{
		return mesh->getVerticesBBox( true );
	}
}

BBox3 GSProductMesh::getMarkedVerticesBBox(const Matrix4 &m) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getVerticesBBox( getLocalTransformation() * m, true );
	}
	else
	{
		return mesh->getVerticesBBox( m, true );
	}
}

BBox3 GSProductMesh::getMarkedVerticesBBox(const Transformation *x) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getVerticesBBox( getLocalTransformation(), x, true );
	}
	else
	{
		return mesh->getVerticesBBox( x, true );
	}
}

Point3 GSProductMesh::getMarkedVerticesCentre() const
{
	return getMarkedVerticesBBox().getCentre();
}


BBox3 GSProductMesh::py_getMarkedVerticesBBox() const
{
	return getMarkedVerticesBBox();
}

Point3 GSProductMesh::py_getMarkedVerticesCentre() const
{
	return getMarkedVerticesCentre();
}



BBox3 GSProductMesh::getMarkedEdgesBBox() const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getMarkedEdgesBBox( getLocalTransformation() );
	}
	else
	{
		return mesh->getMarkedEdgesBBox();
	}
}

BBox3 GSProductMesh::getMarkedEdgesBBox(const Matrix4 &m) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getMarkedEdgesBBox( getLocalTransformation() * m );
	}
	else
	{
		return mesh->getMarkedEdgesBBox( m );
	}
}

BBox3 GSProductMesh::getMarkedEdgesBBox(const Transformation *x) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getMarkedEdgesBBox( getLocalTransformation(), x );
	}
	else
	{
		return mesh->getMarkedEdgesBBox( x );
	}
}

Point3 GSProductMesh::getMarkedEdgesCentre() const
{
	return getMarkedEdgesBBox().getCentre();
}


BBox3 GSProductMesh::py_getMarkedEdgesBBox() const
{
	return getMarkedEdgesBBox();
}

Point3 GSProductMesh::py_getMarkedEdgesCentre() const
{
	return getMarkedEdgesCentre();
}



BBox3 GSProductMesh::getMarkedFacesBBox() const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getMarkedFacesBBox( getLocalTransformation() );
	}
	else
	{
		return mesh->getMarkedFacesBBox();
	}
}

BBox3 GSProductMesh::getMarkedFacesBBox(const Matrix4 &matrix) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getMarkedFacesBBox( getLocalTransformation() * matrix );
	}
	else
	{
		return mesh->getMarkedFacesBBox( matrix );
	}
}

BBox3 GSProductMesh::getMarkedFacesBBox(const Transformation *x) const
{
	const MMesh *mesh = getReadOnlyRepMesh();

	if ( hasLocalTransformation() )
	{
		return mesh->getMarkedFacesBBox( getLocalTransformation(), x );
	}
	else
	{
		return mesh->getMarkedFacesBBox( x );
	}
}

Point3 GSProductMesh::getMarkedFacesCentre() const
{
	return getMarkedFacesBBox().getCentre();
}


BBox3 GSProductMesh::py_getMarkedFacesBBox() const
{
	return getMarkedFacesBBox();
}

Point3 GSProductMesh::py_getMarkedFacesCentre() const
{
	return getMarkedFacesCentre();
}



Vector3 GSProductMesh::computeMarkedFacesWeightedNormal() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	Vector3 normal = mesh->computeMarkedFacesWeightedNormal();

	if ( hasLocalTransformation() )
	{
		return normal * getLocalTransformationInverse();
	}
	else
	{
		return normal;
	}
}

Vector3 GSProductMesh::computeWeightedNormal() const
{
	const MMesh *mesh = getReadOnlyRepMesh();
	Vector3 normal = mesh->computeWeightedNormal();

	if ( hasLocalTransformation() )
	{
		return normal * getLocalTransformationInverse();
	}
	else
	{
		return normal;
	}
}




//MISC OPS

void GSProductMesh::convertEdgeRunToPolyline(MEdgeRun &run, Polyline &poly) const
{
	//get the vertices
	MVertexList vertices;
	run.extractVertices( vertices );

	//convert to a polyline
	for (int i = 0; i < vertices.size(); i++)
	{
		poly.addVertex( vertices[i]->getPosition() );
	}

	//copy closed setting
	poly.setClosed( run.isClosed() );
}










/*********************************************************************************
												MeshModifier
 *********************************************************************************/

MeshModifier::MeshModifier()
{
	mesh = NULL;
}

void MeshModifier::lock()
{
	gs_assert( mesh != NULL, "MeshModifier::lock(): mesh is NULL\n" );
	mesh->lock();
}

void MeshModifier::unlock()
{
	gs_assert( mesh != NULL, "MeshModifier::unlock(): mesh is NULL\n" );
	mesh->unlock();
}

void MeshModifier::finalise()
{
	gs_assert( mesh != NULL, "MeshModifier::finalise(): mesh is NULL\n" );
	mesh->finalise();
}

void MeshModifier::setMesh(GSProductMesh *m)
{
	mesh = m;
}





/*********************************************************************************
											MeshVertexAdjustment
 *********************************************************************************/

MeshVertexAdjustment::MeshVertexAdjustment() : MeshModifier()
{
	adjust = new MVertexAdjustList();
}

MeshVertexAdjustment::~MeshVertexAdjustment()
{
	delete adjust;
}


void MeshVertexAdjustment::setParameters(double alpha, double beta)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->setParameters( alpha, beta );
		lock();
	}
}




/*********************************************************************************
										MeshVertexVectorAdjustment
 *********************************************************************************/

MeshVertexVectorAdjustment::MeshVertexVectorAdjustment() : MeshModifier()
{
	adjust = new MVertexVectorAdjustList();
}

MeshVertexVectorAdjustment::~MeshVertexVectorAdjustment()
{
	delete adjust;
}


void MeshVertexVectorAdjustment::setParameters(const Vector3 &v)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->setParameters( v );
		lock();
	}
}




/*********************************************************************************
										MeshVertexSlideAdjustment
 *********************************************************************************/

MeshVertexSlideAdjustment::MeshVertexSlideAdjustment() : MeshModifier()
{
	adjust = new MVertexSlideAdjustList();
}

MeshVertexSlideAdjustment::~MeshVertexSlideAdjustment()
{
	delete adjust;
}


void MeshVertexSlideAdjustment::setSlideFactor(double slide)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->setSlideFactor( slide );
		lock();
	}
}




/*********************************************************************************
										MeshVertexSurfaceTweakAdjustment
 *********************************************************************************/

MeshVertexSurfaceTweakAdjustment::MeshVertexSurfaceTweakAdjustment() : MeshModifier()
{
	adjust = new MVertexSurfaceTweakAdjustList();
}

MeshVertexSurfaceTweakAdjustment::~MeshVertexSurfaceTweakAdjustment()
{
	delete adjust;
}


void MeshVertexSurfaceTweakAdjustment::tweak(const Vector3 &displacement)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->tweak( displacement );
		lock();
	}
}

void MeshVertexSurfaceTweakAdjustment::debugDraw()
{
	adjust->debugDraw();
}




/*********************************************************************************
											MeshAttribAdjustment
 *********************************************************************************/

MeshAttribAdjustment::MeshAttribAdjustment() : MeshModifier()
{
	adjust = new MVertexAttribAdjustList();
}

MeshAttribAdjustment::~MeshAttribAdjustment()
{
	delete adjust;
}


void MeshAttribAdjustment::setParameters(float alpha)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->setParameters( alpha );
		lock();
	}
}




/*********************************************************************************
										MeshAttribVectorAdjustment
 *********************************************************************************/

MeshAttribVectorAdjustment::MeshAttribVectorAdjustment() : MeshModifier()
{
	adjust = new MVertexAttribVectorAdjustList();
}

MeshAttribVectorAdjustment::~MeshAttribVectorAdjustment()
{
	delete adjust;
}


void MeshAttribVectorAdjustment::setParameters(const Vector2f &v)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->setParameters( v );
		lock();
	}
}




/*********************************************************************************
										MeshAttribSlideAdjustment
 *********************************************************************************/

MeshAttribSlideAdjustment::MeshAttribSlideAdjustment() : MeshModifier()
{
	adjust = new MVertexAttribSlideAdjustList();
}

MeshAttribSlideAdjustment::~MeshAttribSlideAdjustment()
{
	delete adjust;
}


void MeshAttribSlideAdjustment::setSlideFactor(double slide)
{
	if ( adjust->size() > 0 )
	{
		unlock();
		adjust->setSlideFactor( slide );
		lock();
	}
}




/*********************************************************************************
											MeshTransformation
 *********************************************************************************/

MeshTransformation::MeshTransformation() : MeshModifier()
{
	target = new MTransformationTarget();
}

MeshTransformation::~MeshTransformation()
{
	delete target;
}


const Point3 &MeshTransformation::getCentre() const
{
	return target->getCentre();
}

void MeshTransformation::translate(const Vector3 &translation)
{
	if ( target->size() > 0 )
	{
		unlock();
		target->translate( translation );
		lock();
		finalise();
	}
}

void MeshTransformation::centredTransform(const Matrix4 &xform)
{
	if ( target->size() > 0 )
	{
		unlock();
		target->centredTransform( xform );
		lock();
		finalise();
	}
}




/*********************************************************************************
									MeshMaterialTransformBackup
 *********************************************************************************/

MeshMaterialTransformBackup::MeshMaterialTransformBackup()
{
	backup = new MMaterialTransformBackup();
}

MeshMaterialTransformBackup::~MeshMaterialTransformBackup()
{
	delete backup;
}





/*********************************************************************************
									MeshSharpnessBackup
 *********************************************************************************/

MeshSharpnessBackup::MeshSharpnessBackup()
{
}

MeshSharpnessBackup::~MeshSharpnessBackup()
{
}





/*********************************************************************************
						MeshSurfaceTweakAdjuster
 *********************************************************************************/

MeshSurfaceTweakAdjuster::MeshSurfaceTweakAdjuster()
{
}


void MeshSurfaceTweakAdjuster::tweak(const Vector3 &displacement)
{
	vertexAdjust.tweak( displacement );
	vertexAdjust.finalise();
}


void MeshSurfaceTweakAdjuster::debugDraw()
{
	vertexAdjust.debugDraw();
}



/*********************************************************************************
						MeshSurfaceTweakNormalAdjuster
 *********************************************************************************/

MeshSurfaceTweakNormalAdjuster::MeshSurfaceTweakNormalAdjuster()
{
}


void MeshSurfaceTweakNormalAdjuster::tweak(double displacement)
{
	vertexAdjust.setParameters( displacement, 0.0 );
	vertexAdjust.finalise();
}




/*********************************************************************************
					MeshProportionalAdjuster
 *********************************************************************************/

MeshProportionalAdjuster::MeshProportionalAdjuster()
{
	proportionalAdjuster = new MProportionalAdjuster();
}

MeshProportionalAdjuster::~MeshProportionalAdjuster()
{
	delete proportionalAdjuster;
}



void MeshProportionalAdjuster::save()
{
	proportionalAdjuster->save();
	finalise();
}

void MeshProportionalAdjuster::restore()
{
	proportionalAdjuster->restore();
	finalise();
}

void MeshProportionalAdjuster::move(const Vector3 &translation)
{
	proportionalAdjuster->move( translation );
	finalise();
}

void MeshProportionalAdjuster::moveNormal(double distance)
{
	proportionalAdjuster->moveNormal( distance );
	finalise();
}

void MeshProportionalAdjuster::scale(const Point3 &centre, double scaleFactor)
{
	proportionalAdjuster->scale( centre, scaleFactor );
	finalise();
}

void MeshProportionalAdjuster::rotate(const Point3 &centre, const Vector3 &rotationAxis, double rotation)
{
	proportionalAdjuster->rotate( centre, rotationAxis, rotation );
	finalise();
}

void MeshProportionalAdjuster::relax(double relaxAmount)
{
	proportionalAdjuster->relax( relaxAmount );
	finalise();
}




/*********************************************************************************
						MeshMoveNormalAdjuster
 *********************************************************************************/

MeshMoveNormalAdjuster::MeshMoveNormalAdjuster()
{
}


void MeshMoveNormalAdjuster::setParameters(double distance)
{
	vertexAdjust.setParameters( distance, 0.0 );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshExpandVerticesAdjuster
 *********************************************************************************/

MeshExpandVerticesAdjuster::MeshExpandVerticesAdjuster()
{
}


void MeshExpandVerticesAdjuster::setParameters(double expansion, double extrusion)
{
	vertexAdjust.setParameters( expansion, extrusion );
	attribAdjust.setParameters( expansion );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshExpandVerticesInMarkedFacesAdjuster
 *********************************************************************************/

MeshExpandVerticesInMarkedFacesAdjuster::MeshExpandVerticesInMarkedFacesAdjuster()
{
}


void MeshExpandVerticesInMarkedFacesAdjuster::setParameters(double expansion)
{
	vertexAdjust.setParameters( expansion, 0.0 );
	attribAdjust.setParameters( expansion );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshBevelVerticesAdjuster
 *********************************************************************************/

MeshBevelVerticesAdjuster::MeshBevelVerticesAdjuster()
{
}


void MeshBevelVerticesAdjuster::setParameters(double bevel)
{
	vertexAdjust.setParameters( bevel, 0.0 );
	attribAdjust.setParameters( bevel );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshExpandEdgesAdjuster
 *********************************************************************************/

MeshExpandEdgesAdjuster::MeshExpandEdgesAdjuster()
{
}


void MeshExpandEdgesAdjuster::setParameters(double expansion, double extrusion)
{
	vertexAdjust.setParameters( expansion, extrusion );
	attribAdjust.setParameters( expansion );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshBevelEdgesAdjuster
 *********************************************************************************/

MeshBevelEdgesAdjuster::MeshBevelEdgesAdjuster()
{
}


void MeshBevelEdgesAdjuster::setParameters(double bevel)
{
	vertexAdjust.setParameters( bevel, 0.0 );
	attribAdjust.setParameters( bevel );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshExtrudeEdgesAdjuster
 *********************************************************************************/

MeshExtrudeEdgesAdjuster::MeshExtrudeEdgesAdjuster()
{
}


void MeshExtrudeEdgesAdjuster::setParameters(const Vector3 &extrude, const Vector2 &texExtrude)
{
	vertexAdjust.setParameters( extrude );
	attribAdjust.setParameters( texExtrude );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshRiftEdgesAdjuster
 *********************************************************************************/

MeshRiftEdgesAdjuster::MeshRiftEdgesAdjuster()
{
}


void MeshRiftEdgesAdjuster::setParameters(const Vector3 &rift)
{
	vertexAdjust.setParameters( rift );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshSlideEdgesAdjuster
 *********************************************************************************/

MeshSlideEdgesAdjuster::MeshSlideEdgesAdjuster()
{
}


void MeshSlideEdgesAdjuster::setParameters(double slideFactor)
{
	vertexAdjust.setSlideFactor( slideFactor );
	attribAdjust.setSlideFactor( slideFactor );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshInsetFacesAdjuster
 *********************************************************************************/

MeshInsetFacesAdjuster::MeshInsetFacesAdjuster()
{
}


void MeshInsetFacesAdjuster::setParameters(double inset)
{
	vertexAdjust.setParameters( inset, 0.0 );
	attribAdjust.setParameters( inset );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshBevelFacesAdjuster
 *********************************************************************************/

MeshBevelFacesAdjuster::MeshBevelFacesAdjuster()
{
}


void MeshBevelFacesAdjuster::setParameters(double inset, double extrusion)
{
	vertexAdjust.setParameters( inset, extrusion );
	attribAdjust.setParameters( inset );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshExpandFacesAdjuster
 *********************************************************************************/

MeshExpandFacesAdjuster::MeshExpandFacesAdjuster()
{
}


void MeshExpandFacesAdjuster::setParameters(double expansion, double extrusion)
{
	vertexAdjust.setParameters( expansion, extrusion );
	attribAdjust.setParameters( expansion );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshExtrudeFacesAdjuster
 *********************************************************************************/

MeshExtrudeFacesAdjuster::MeshExtrudeFacesAdjuster()
{
}


void MeshExtrudeFacesAdjuster::setParameters(double extrusion)
{
	vertexAdjust.setParameters( extrusion, 0.0 );
	vertexAdjust.finalise();
}



/*********************************************************************************
						MeshVectorExtrudeFacesAdjuster
 *********************************************************************************/

MeshVectorExtrudeFacesAdjuster::MeshVectorExtrudeFacesAdjuster()
{
}


void MeshVectorExtrudeFacesAdjuster::setParameters(const Vector3 &extrusion)
{
	vertexAdjust.setParameters( extrusion );
	vertexAdjust.finalise();
}








/*********************************************************************************
						MeshNCutState
 *********************************************************************************/

MeshNCutState::MeshNCutState()
{
}


int MeshNCutState::size()
{
	return lastSplitVertexIndices.size();
}

int MeshNCutState::getLastSplitVertexIndex(int index)
{
	return lastSplitVertexIndices[index];
}

const Point3 & MeshNCutState::getLastSplitVertexPosition(int index)
{
	return lastSplitVertexPositions[index];
}








/*********************************************************************************
						MeshDrawQuadsState
 *********************************************************************************/

MeshDrawQuadsState::MeshDrawQuadsState() : mesh( NULL )
{
	state = new MDrawQuadsState();
}

MeshDrawQuadsState::~MeshDrawQuadsState()
{
	delete state;
}



int MeshDrawQuadsState::getColumn() const
{
	return state->getColumn();
}

int MeshDrawQuadsState::getRow() const
{
	return state->getRow();
}

int MeshDrawQuadsState::getNumColumns() const
{
	return state->getNumColumns();
}

int MeshDrawQuadsState::getNumRows() const
{
	return state->getNumRows();
}

bool MeshDrawQuadsState::isRowDirectionForward() const
{
	return state->isRowDirectionForward();
}

bool MeshDrawQuadsState::areRowsClosed() const
{
	return state->areRowsClosed();
}

bool MeshDrawQuadsState::areColumnsClosed() const
{
	return state->areColumnsClosed();
}

bool MeshDrawQuadsState::getQuadMeshVertexPosition(int column, int row, Point3 &pos) const
{
	return state->getQuadMeshVertexPosition( column, row, pos );
}


boost::python::tuple MeshDrawQuadsState::py_getQuadMeshVertexPosition(int column, int row) const
{
	Point3 pos;
	bool result = getQuadMeshVertexPosition( column, row, pos );
	return boost::python::make_tuple( result, pos );
}



bool MeshDrawQuadsState::checkVertex(int vertexIndex)
{
	gs_assert( mesh != NULL, "MeshDrawQuadState::checkVertex(): mesh is NULL, not initialised properly\n" );

	MMesh *mmesh = mesh->getReadOnlyRepMesh();
	MVertex *vertex = mmesh->getVertices()[vertexIndex];

	return state->checkVertex( vertex );
}



void MeshDrawQuadsState::init(GSProductMesh *mesh)
{
	this->mesh = mesh;
}









/*********************************************************************************
						MeshDrawFaceState
 *********************************************************************************/

MeshDrawFaceState::MeshDrawFaceState() : mesh( NULL )
{
	state = new MDrawFaceState();
}

MeshDrawFaceState::~MeshDrawFaceState()
{
	delete state;
}



int MeshDrawFaceState::getNumVertices() const
{
	return state->getNumVertices();
}

const Point3 & MeshDrawFaceState::getFaceVertexPosition(int n) const
{
	return state->getFaceVertexPosition( n );
}



MDrawFaceVertexCheckResult MeshDrawFaceState::checkVertex(int vertexIndex)
{
	gs_assert( mesh != NULL, "MeshDrawFaceState::checkVertex(): mesh is NULL, not initialised properly\n" );

	MMesh *mmesh = mesh->getReadOnlyRepMesh();
	MVertex *vertex = mmesh->getVertices()[vertexIndex];

	return state->checkVertex( vertex );
}



void MeshDrawFaceState::init(GSProductMesh *mesh)
{
	this->mesh = mesh;
}




#endif
