//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MFACE_H__
#define MFACE_H__

#include <stdio.h>

#include <list>

#include <Util/Array.h>
#include <Util/InPlaceAllocator.h>
#include <Util/Pool.h>
#include <Util/SETree.h>

#include <Math/index.h>
#include <Math/Vector3.h>
#include <Math/Vector3f.h>
#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/BBox3.h>
#include <Math/Triangle3.h>
#include <Math/Projection.h>
#include <Math/IndexTriangle.h>

#include <Graphics/Region.h>
#include <Graphics/BackfaceCull.h>

#include <Transformation/Transformation.h>

#include <Model/MarkPredicate.h>

#include <Mesh/MMesh/MeshTypes.h>





class GS_DllExport MFace
{
private:
	static Pool<MFace> facePool;


	// Stored Centre - used for the inset faces and extrude edges operations
	struct StoredCentre;


	//each vertex also has:
	//an edge (linking it to the next vertex
	//a vertex attribute
	//a vertex normal
	class Vertex
	{
	public:
		MVertex *vertex;
		MEdge *edge;
		MVertexAttrib *attrib;
		MVertexNormal *normal;


		inline Vertex() : vertex( NULL ), edge( NULL ), attrib( NULL ), normal( NULL )
		{
		}
	};

	typedef Array<Vertex> FaceVertexList;
	typedef InPlaceAllocator<Vertex,4> VertexListAlloc;


	Array<Vertex,VertexListAlloc> vertices;				//80		(80)		(10 * 8)


	union											//12		(92)		(11.5 * 8)
	{
		struct
		{
			MVertex *vertex;
		} subdivision;

		struct
		{
			StoredCentre *centre;
			Plane *plane;
		} insetFace;

		struct
		{
			MVertexList *cutVertices;
		} knife;

		struct
		{
			Vector3 *groupDirection;
		} extrude;

		struct
		{
			Array<Matrix4> *matrices;
		} pathExtrude;
	} faceOpData;

	mutable Plane plane;									//32		(120)		(15 * 8)
	int materialID;										//4		(124)		(16 * 8)
	mutable Tesselation *tesselation;						//4		(128)		(16.5 * 8)
	MMesh *mesh;									//4		(132)		(17 * 8)



public:
	int index;											//4		(136)		(17.5 * 8)


private:
	struct Flags										//4		(140)		(18 * 8)
	{
		unsigned faceMark : 1;
		unsigned secondaryMark : 1;
		unsigned destroyed : 1;
		mutable unsigned planeRequiresRefresh : 1;
		unsigned faceMarkSave : 1;
		unsigned incidentVerticesTransformed : 1;
		mutable unsigned tesselationRequiresRefresh : 1;
		unsigned liveSubdVertexRequiresRefresh : 1;
	} flags;



private:
	void faceModified();

public:
	MFace(MMesh *m, int ndx);
	~MFace();


	void * operator new(size_t sz);
	void operator delete(void *data);
	static void poolAllocReserve(int numItmes);


	void construct(MMesh *m, int ndx);
	void shutdown();
	void init(const Array<MVertex*> &vts, const Array<MVertexAttrib> &attribs, int matID);
	void init(const Array<MVertex*> &vts, const Array<MVertexAttrib*> &attribs, int matID);
	void init(const Array<MVertex*> &vts, const Array<MEdge*> &eds, const Array<MVertexAttrib*> &attribs, int matID);
	void copyFrom(const MFace *face, MMesh *parentMesh, int verticesOffset, int edgesOffset);

	void read(FILE *f, MMesh *parentMesh);
	void write(FILE *f) const;
	void read(SENode &node, MMesh *parentMesh);
	void write(SENode &node) const;


	void preComputeFull();



	inline void optimiseMemoryUsage()
	{
		vertices.optimiseMemoryUsageLean();
		if ( tesselation != NULL )
		{
			tesselation->optimiseMemoryUsageLean();
		}
	}



	MMesh * getMesh();


	inline int getIndex() const
	{
		return index;
	}


	inline static int estimateCost()
	{
		return sizeof(MFace)  +  sizeof(Vertex) * 4;
	}


	//************************* VERTEX OPERATIONS *************************
	//*********************************************************************
	inline void incidentVertexModified()
	{
		faceModified();
		invalidateTesselation();
	}

	inline void incidentVertexTransformedLinear()
	{
		flags.incidentVerticesTransformed = true;
		faceModified();
	}


	inline int getSize() const
	{
		return vertices.size();
	}

	inline bool isTriangle() const
	{
		return vertices.size() == 3;
	}

	inline bool isQuad() const
	{
		return vertices.size() == 4;
	}

	inline MVertex* getVertex(int i)
	{
		return vertices[i].vertex;
	}

	inline const MVertex* getVertex(int i) const
	{
		return vertices[i].vertex;
	}

	inline int findVertex(const MVertex *v) const
	{
		//deal with common cases specially to save speed
		if ( vertices.size() == 3 )
		{
			int index = v == vertices[0].vertex  ?  0 : -1;
			index = v == vertices[1].vertex  ?  1 : index;
			index = v == vertices[2].vertex  ?  2 : index;
			return index;
		}
		else if ( vertices.size() == 4 )
		{
			int index = v == vertices[0].vertex  ?  0 : -1;
			index = v == vertices[1].vertex  ?  1 : index;
			index = v == vertices[2].vertex  ?  2 : index;
			index = v == vertices[3].vertex  ?  3 : index;
			return index;
		}
		else
		{
			for (int i = 0; i < vertices.size(); i++)
			{
				if ( vertices[i].vertex == v )
				{
					return i;
				}
			}
			return -1;
		}
	}

	inline bool isIncidentTo(const MVertex *v) const
	{
		return findVertex( v )  !=  -1;
	}



	void getFacePoints(Array<Point3> &facePoints) const;
	void getFaceNormals(Array<Vector3f> &faceNormals) const;

	MVertex * findClosestVertexTo(const Point3 &point);
	MVertex * getFurthestVertexFromPlane(const Plane &p);


	void replaceVertex(int vtxIndex, MVertex *vtxNew, MVertexAttrib *attrNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties);
	void replaceVertex(MVertex *vtxOld, MVertex *vtxNew, MVertexAttrib *attrNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties);
	void replaceVertex(int vtxIndex, MVertex *vNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties);
	void replaceVertex(MVertex *vtxOld, MVertex *vNew, bool removeFaceFromVOld, bool destroyIsolatedVertices, bool keepEdgeProperties);
	void removeVertex(int vtxIndex, bool removeFaceFromV, bool destroyIsolatedVertices);
	void removeVertex(MVertex *v, bool removeFaceFromV, bool destroyIsolatedVertices);
	bool checkRemoveVertex(int vtxIndex);
	bool checkRemoveVertex(const MVertex *v);
	void insertVertex(int position, MVertex *newVertex, MVertexAttrib *attrib);
	void insertVertex(MVertex *position, MVertex *newVertex, MVertexAttrib *attrib);

	int getSharedVertexCount(const MFace *f) const;



	//******************** VERTEX ATTRIBUTE OPERATIONS ********************
	//*********************************************************************
	inline MVertexAttrib * getVertexAttrib(int i)
	{
		return vertices[i].attrib;
	}

	inline const MVertexAttrib * getVertexAttrib(int i) const
	{
		return vertices[i].attrib;
	}

	inline MVertexAttrib * getVertexAttrib(const MVertex *v)
	{
		int index = findVertex( v );

		gs_assert( index != -1, "MFace::getVertexAttrib(): could not find vertex @v\n" );

		return getVertexAttrib( index );
	}

	inline const MVertexAttrib * getVertexAttrib(const MVertex *v) const
	{
		int index = findVertex( v );

		gs_assert( index != -1, "MFace::getVertexAttrib(): could not find vertex @v\n" );

		return getVertexAttrib( index );
	}


	//simple and stupid: does not ref() and unref() the attribs
	void setVertexAttrib(int index, MVertexAttrib *newAttrib);
	void setVertexAttrib(MVertexAttrib *oldAttrib, MVertexAttrib *newAttrib);



	//********************** VERTEX NORMAL OPERATIONS *********************
	//*********************************************************************
	inline void setVertexNormal(const MVertex *vertex, MVertexNormal *normal)
	{
		int index = findVertex( vertex );

		if ( index != -1 )
		{
			vertices[index].normal = normal;
		}
	}

	MVertexNormal * getVertexNormal(int i);
	const MVertexNormal * getVertexNormal(int i) const;

private:
	void refreshVertexNormals();
	void refreshVertexNormalsForVertex(int index);

public:
	//************************** EDGE OPERATIONS **************************
	//*********************************************************************
	inline MEdge * getEdge(int i)
	{
		return vertices[i].edge;
	}

	inline int findEdge(const MEdge *e) const
	{
		//deal with common cases specially to save time
		if ( vertices.size() == 3 )
		{
			int index = e == vertices[0].edge  ?  0 : -1;
			index = e == vertices[1].edge  ?  1 : index;
			index = e == vertices[2].edge  ?  2 : index;
			return index;
		}
		else if ( vertices.size() == 4 )
		{
			int index = e == vertices[0].edge  ?  0 : -1;
			index = e == vertices[1].edge  ?  1 : index;
			index = e == vertices[2].edge  ?  2 : index;
			index = e == vertices[3].edge  ?  3 : index;
			return index;
		}
		else
		{
			for (int i = 0; i < vertices.size(); i++)
			{
				if ( vertices[i].edge == e )
				{
					return i;
				}
			}
			return -1;
		}
	}

	inline bool isIncidentTo(const MEdge *e) const
	{
		return findEdge( e ) != -1;
	}



	inline MEdge * getPrevEdge(const MVertex *v)
	{
		int index = findVertex( v );

		gs_assert( index != -1, "MFace::getPrevEdge(): vertex @v is not incident to @this\n" );

		int prevEdgeIndex = prevIndex( index, vertices.size() );

		return vertices[prevEdgeIndex].edge;
	}

	inline MEdge * getNextEdge(const MVertex *v)
	{
		int index = findVertex( v );

		gs_assert( index != -1, "MFace::getNextEdge(): vertex @v is not incident to @this\n" );

		//the edge after this vertex is at the same index as v,
		//not nextIndex( index, vertices.size() )
		return vertices[index].edge;
	}


	inline void getAdjacentEdges(const MVertex *v, MEdge*& prevEdge, MEdge*& nextEdge, int &vertexIndex)
	{
		vertexIndex = findVertex( v );

		gs_assert( vertexIndex != -1, "MFace::getAdjacentEdges(): vertex @v is not incident to @this\n" );

		int prevEdgeIndex = prevIndex( vertexIndex, vertices.size() );

		prevEdge = vertices[prevEdgeIndex].edge;
		nextEdge = vertices[vertexIndex].edge;
	}

	inline void getAdjacentEdges(const MVertex *v, MEdge*& prevEdge, MEdge*& nextEdge)
	{
		int vertexIndex;
		getAdjacentEdges( v, prevEdge, nextEdge, vertexIndex );
	}


	void getAdjacentEdges(const MEdge *e, MEdge*& prevEdge, MEdge*& nextEdge);


	bool isEdgeInOrder(const MEdge *e, int index) const;
	bool isEdgeInOrder(const MEdge *e) const;
	bool isEdgeInOrder_search(const MEdge *e) const;
	MEdge * getNextEdge(const MEdge *currentEdge, const MVertex *pivotVertex);

	MEdge * getFaceMarkBoundaryEdge();
private:
	void refreshEdgeIndices(int start);

public:
	MEdge * findClosestEdgeTo(const Point3 &point);


	MEdge * quadGetOppositeEdge(MEdge *e);


	int getSharedEdgeCount(MFace *withFace) const;

	//the edges that are returned, are in the same order as they appear
	//in @this
	void findCommonEdges(const MFace *f, MEdgeList &commonEdges);

	//the edge indices that are returned, are in increasing order,
	//and index into the edge list in @this
	void findCommonEdgeIndices(const MFace *f, Array<int> &commonEdgeIndices) const;

	//returns true if the indices in @edgeIndices are continuous;
	//that is, if they are sequential, in order, and adjacent to one
	//another. They may wrap over, (e.g. 0,1,8,9, where @this has 10 vertices)
	//in which case the list may be rotated.
	//NOTE: @edgeIndices may be modified
	bool isEdgeIndexListContinuous(Array<int> &edgeIndices) const;




	//************************* DESTROY OPERATIONS ************************
	//*********************************************************************
	inline void markAsDestroyed()
	{
		flags.destroyed = true;
	}

	inline bool isDestroyed() const
	{
		return flags.destroyed;
	}

	void destroy(bool destroyIsolatedEdges, bool destroyIsolatedVertices);



	//************************ FACE MARK OPERATIONS ***********************
	//*********************************************************************
	void faceMark();
	void faceUnmark();
	void faceMarkInvert();
	void setFaceMark(bool mark);

	inline void setFaceMark(MarkPredicate pred, bool inRegion)
	{
		setFaceMark( markPredicateIsMarked( pred, flags.faceMark, inRegion ) );
	}

	inline bool isFaceMarked() const
	{
		return flags.faceMark;
	}

	inline void faceMarkSave()
	{
		flags.faceMarkSave = flags.faceMark;
	}

	inline void faceMarkRestore()
	{
		setFaceMark( flags.faceMarkSave );
	}


	bool isOnFaceMarkBoundary() const;
	bool isFaceMarkInternal() const;

	bool hasNeighbouringMarkedFaces() const;
	bool hasNeighbouringUnmarkedFaces() const;

	void visitFaceMarkedRegion();
	void getFaceMarkedRegion(Array<MFace*> &region);

	void discoverFaceMarkedRegionVerticesPopulateStack(std::list<MVertex*> &vertexStack);



	//********************* SECONDARY MARK OPERATIONS *********************
	//*********************************************************************
	inline void secondaryMark()
	{
		flags.secondaryMark = true;
	}

	inline void secondaryUnmark()
	{
		flags.secondaryMark = false;
	}

	inline bool isSecondaryMarked() const
	{
		return flags.secondaryMark;
	}



	//********************** TESSELATION OPERATIONS ***********************
	//*********************************************************************
private:
	inline void invalidateTesselation()
	{
		destroyTesselation();
		flags.tesselationRequiresRefresh = true;
	}

	bool computeTesselation(Tesselation &tess) const;
	void tesselate() const;
	void setTesselation(const Tesselation &t) const;
	void destroyTesselation() const;

	inline void refreshTesselation() const
	{
		//if the tesselation requires refreshing
		if ( flags.tesselationRequiresRefresh )
		{
			//refresh and return the tesselation
			tesselate();
		}
	}

public:
	inline const Tesselation * getTesselation() const
	{
		refreshTesselation();
		return tesselation;
	}





	//*********************** PROPERTIES OPERATIONS ***********************
	//*********************************************************************
private:
	inline void copyPropertiesFrom(const MFace *f)
	{
		setFaceMark( f->isFaceMarked() );
		flags.secondaryMark = f->flags.secondaryMark;

		changeMaterialID( f->materialID );
	}



public:
	//*********************** VERTEX MARK OPERATIONS **********************
	//*********************************************************************
	bool isVertexMarked() const;
	bool isWhollyVertexMarked() const;




	//************************ EDGE MARK OPERATIONS ***********************
	//*********************************************************************
	bool isEdgeMarked() const;
	bool isWhollyEdgeMarked() const;
	bool hasIncidentEdgeMarkedVertices() const;
	MEdge * getOtherMarkedEdge(const MEdge *e);




	//******************** MATERIAL OPERATIONS ********************
	//*********************************************************************
	inline int getMaterialID() const
	{
		return materialID;
	}

	void changeMaterialID(int m);




	//*********************** SUBDIVISION OPERATIONS **********************
	//*********************************************************************
	//get the vertex created for subdivision
	inline MVertex * getSubdivisionVertex()
	{
		return faceOpData.subdivision.vertex;
	}

	//create the subdivision vertex at the centre of the face
	void createSubdivisionVertex();
	void createLiveSubdivisionVertex(MMesh *liveMesh);
	void refreshLiveSubdivisionVertex(MMesh *liveMesh, int faceSubdVerticesOffset);
	void liveSubdivisionIncidentVertexModified();

	inline void liveSubdivisionRefreshed()
	{
		flags.liveSubdVertexRequiresRefresh = false;
	}

private:
	void setSubdivisionFaceProperties(MFace *src, MEdge *ea, MEdge *eb, MVertex *vb, bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness);
	void setLiveSubdivisionFaceProperties(MEdge *ea, MEdge *eb);

public:
	//subdivide this, generating the subdivided faces
	void createSubdivisionFaces(bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness);
	void createLiveSubdivisionFaces(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset, MPreserveNormalSharpness preserveNormalSharpness);




	//********************** EXTRUDE FACE OPERATIONS **********************
	//*********************************************************************
	inline void extrudeFaceSetGroupDirection(Vector3 *direction)
	{
		faceOpData.extrude.groupDirection = direction;
	}

	inline Vector3 * extrudeFaceGetGroupDirection()
	{
		return faceOpData.extrude.groupDirection;
	}


	void extrudeFace();
	void pathExtrudeSetMatrices(Array<Matrix4> *matrices);
	const Array<Matrix4>* getPathExtrudeMatrices() const;



	//****************** EXTRUDE FACE SINGULAR OPERATIONS *****************
	//*********************************************************************
	void extrudeSingular(MVertexAdjustList &vertexAdjusts, int numSegments,
					bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges, int sideMaterialID);



	//********************** DUPLICATE FACE OPERATIONS **********************
	//*********************************************************************
	MFace * createDuplicateFace();



	//******************* INSET FACE SINGULAR OPERATIONS ******************
	//*********************************************************************
	void insetSingular(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle,
					bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges);



	//*********************** INSET/EXPAND FACE OPERATIONS ***********************
	//*********************************************************************
	inline bool isInsetExpandTarget(MInsetExpandOperation insetExpandOp) const
	{
		if ( insetExpandOp == MINSETEXPANDOP_INSET )
		{
			return isFaceMarked();
		}
		else
		{
			return !isFaceMarked();
		}
	}

	bool insetExpandFaceStoreCentre();
	void insetExpandFaceDeleteCentre();

	bool insetExpandFaceStorePlane(MInsetExpandOperation insetExpandOp);
	void insetExpandFaceDeletePlane();
	void insetExpandFaceCopyStoredPlaneFrom(const MFace *face);
	const Plane & getInsetExpandFaceStoredPlane() const;

	void insetExpandFaceComputeToCentreInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, const Point3 &centre, const Point2f &attribCentre,
												double &uLower, double &uUpper, bool allowNegativeInset);
	void insetExpandFaceComputeToCentreInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, double &uLower, double &uUpper, bool allowNegativeInset);
	void insetExpandFaceComputeFromEdgeInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, bool projectOntoFacePlane, double &uLower, double &uUpper,
												bool allowNegativeInset);
	void computeFromEdgeInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, bool projectOntoFacePlane, double &uLower, double &uUpper,
												bool allowNegativeInset);

private:
	void computeFromEdgeInsetVectors(int vertexIndex, Vector3 &vertexInset, Vector2f &attribInset, bool projectOntoFacePlane, double &uLower, double &uUpper,
												bool allowNegativeInset, const Plane &facePlane);
public:


	//******************* INSET FACE TO POINT OPERATIONS ******************
	//*********************************************************************
	void insetToPoint(MVertexAdjustList &vertexAdjusts, int numSegments);



	//*********************** EXPAND EDGE OPERATIONS **********************
	//*********************************************************************
	void expandEdgeInsertCornerVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MEdgeList &edgesToCheck);


	//*********************** BEVEL EDGE OPERATIONS ***********************
	//*********************************************************************
private:
	MVertexAttrib * getBevelEdgeReplacementAttrib(MVertex *v, MEdge *e);
public:
	void bevelEdgeReplaceVerticesWithBoundaryVerts();


private:
	//KNIFE OPERATIONS
	enum KnifeCutVtxPRD
	{
		KNIFE_POS_NEG,
		KNIFE_NEG_POS,
		KNIFE_SAME_SIDE,
		KNIFE_ON_ON
	};

	class KnifeScreenPoly
	{
	private:
		Polygon2 poly;
		double areax2;

	public:
		KnifeScreenPoly(const FaceVertexList &verts, const Matrix4 &m);

		const Point2 & operator[](int index) const;

		bool isSelfIntersecting() const;
		bool isSplitAreaConsistent(int va, int vb) const;


	private:
		void computeSmallestSubPoly(int &splitA, int &splitB, int &numEdges) const;
	};

	struct KnifeCutEntry
	{
		int index;
		double positionAlongKnifeVector;
	};

	struct KnifeCompareFunc
	{
		inline bool operator()(const KnifeCutEntry &a, const KnifeCutEntry &b) const
		{
			return a.positionAlongKnifeVector < b.positionAlongKnifeVector;
		}
	};


	static bool knifeCanSplitPRD(KnifeCutVtxPRD a, KnifeCutVtxPRD b);


public:
	void knifeInitialise();
	void knifeFinish();
	void knifeAddCutVertex(MVertex *cutVertex);
	void knife(const Plane &knifePlane, const Point2 &knifeStart, const Point2 &knifeEnd, const Matrix4 &localToScreen, bool markCutEdges);



	//CONNECT OPERATIONS
	void connectVertices(bool markCutEdges);

	void cutSplitFace(MVertex *startVertex, MVertex *endVertex, bool hasFaceTargets, const MCutTargetList &targets, int targetStart, int numTargets);

	void nCutPerformSplits(MVertexList &src, MVertexList &dest, MEdgeList *splitEdges=NULL);


	//ROUND OPERATIONS
	void round();


	//EDGE SPLIT OPERATIONS
private:
	MVertexAttrib * edgeSplit(int edgeIndex, MEdge *newEdgeA, MVertex *splitVertex, MEdge *newEdgeB, double t, MVertexAttrib *useAttrib);
public:
	MVertexAttrib * splitEdge(MEdge *edge, MEdge *newEdgeA, MVertex *splitVertex, MEdge *newEdgeB, double t, MVertexAttrib *useAttrib);
	//fromVertex: determines the origin and therefore direction of the
	//adjustment adjustPeriod: the value at which the destination value is
	//reached
	MVertexAttrib * splitEdge(MEdge *edge, MEdge *newEdgeA, MVertex *splitVertex, MEdge *newEdgeB, MVertex *fromVertex,
						MVertexAttribAdjustList &vertexAttribAdjusts, double scaleFactor, MVertexAttrib *useAttrib);


	//FACE SPLIT OPERATIONS
	MFace * split(int vaIndex, int vbIndex, MEdge*& newEdge);
	MFace * split(MVertex *va, MVertex *vb, MEdge*& newEdge);
	MFace * splitOffTriangleIncidentToVertex(MVertex *v, MEdge*& newEdge);
	MFace * splitOffQuadIncidentToEdge(MEdge *e, MEdge*& newEdge);
	double computeVertexVertexSplitSuitability(int vaIndex, int vbIndex, bool &degenerateSubFaces) const;
	double computeVertexVertexSplitSuitability(MVertex *va, MVertex *vb, bool &degenerateSubFaces) const;
	double computeVertexEdgeSplitSuitability(int vIndex, int eIndex) const;
	double computeVertexEdgeSplitSuitability(MVertex *v, MEdge *e) const;
	double computeEdgeEdgeSplitSuitability(int eaIndex, int ebIndex) const;
	double computeEdgeEdgeSplitSuitability(MEdge *ea, MEdge *eb) const;
	MFace * splitWith(int vaIndex, int vbIndex, const Array<MVertex*> &splitVerts, const Array<MVertexAttrib*> &splitAttribs, MEdgeList &splitEdges);
	MFace * splitWith(MVertex *va, MVertex *vb, const Array<MVertex*> &splitVerts, const Array<MVertexAttrib*> &splitAttribs, MEdgeList &splitEdges);


	//IMPORT MESH OPERATIONS
	void importMeshSanityCheck(MFaceList &resultFaces);


	//EDGE COLLAPSE OPERATIONS
	void edgeCollapseAttribs(int edgeIndex, MCollapseEdgeTo collapseTo);

	//FACE COLLAPSE TO VERTEX OPERATIONS
	MVertex * collapseToVertex();

	//EDGE DISSOLVE OPERATIONS
	void mergeInto(MFace *destFace, const Array<int> &sharedEdgeIndices);



	// PLANE OPERATIONS
	inline const Plane & getPlane() const
	{
		if ( flags.planeRequiresRefresh )
		{
			refreshPlane();
		}

		return plane;
	}

private:
	void refreshPlane() const;


public:
	//************************** MISC OPERATIONS **************************
	//*********************************************************************
	Point3 computeCentroid() const;
	MVertexAttrib computeAttribCentroid() const;
	BBox3 computeBBox() const;
	Vector3 computeTwiceAreaNormalProduct() const;
	Vector3 computeSubfaceTwiceAreaNormalProduct(int vaIndex, int vbIndex) const;
	double computeAverageDistanceAlongNormalisedVector(const Vector3 &v) const;
	int getNumTriangles();
	Triangle3 getTriangle(int tri);
	IndexTriangle getIndexTriangle(int tri);
	Triangle2 getAttribTriangle(int tri);
	void generatePolygon(Polygon3 &poly) const;
	void generate2DPointList(Array<Point2> &points) const;

private:
	bool hitsTriangle(int tri, const Segment3 &ray, double &t, double &u, double &v);
public:
	bool hits(const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersectionPoint, int &tri);
	bool hits(const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersectionPoint);

	bool pick(const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersectionPoint);
	MEdge * pickEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, bool faceMarkedOnlyFlag, Point3 &closestPoint);
	MVertex * pickVertex(const Point2 &p, const Projection &projection, bool faceMarkedOnlyFlag);
	MVertex * pickVertexThruEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, bool faceMarkedOnlyFlag, MEdge *&thruEdge);

	void flip();
	void triangulate();
	void triangulate(const Tesselation &tess);
	bool isPlanar();
	bool isConvex();
	void fix();
	Side side(const Plane &pl, bool strict) const;
	Side side(const BBox3 &box, bool strict) const;


	inline bool isFrontFace(const BackfaceCull *faceCull) const
	{
		return faceCull->isFrontFacing( getPlane() );
	}

	//BOUNDARY / MARK METHODS
	bool isWhollyInside(const Region3d *region);
	bool isPartiallyInside(const Region3d *region);
	bool isWhollyInside(const Region3d *region, const BackfaceCull *faceCull);
	bool isPartiallyInside(const Region3d *region, const BackfaceCull *faceCull);


	//TRANSFORM METHODS
	void vertexTransformFinalise();


	//************************ MATERIAL OPERATIONS ************************
	//*********************************************************************
	void setMaterial(int mat);

	void transformMaterial(const Matrix4 &xform, bool fixMarkedVertices);
	void transformMaterialSave(bool fixMarkedVertices, MMaterialTransformBackup &b);
	void transformMaterialRestore(bool fixMarkedVertices, const MMaterialTransformBackup &b, int &position);

	void setMaterialCoords(const Array<Point2f> &c);
	void setMaterialCoords(const Array<Point2f> &c, int mat);
	void setMaterialCoords(int index, const Point2f &uv);
	void setMaterialCoords(MVertex *vertex, const Point2f &uv);

	BBox2 getMaterialBBox();
	void addToMaterialBBox(BBox2 &box);

	void getUVMapNormals(Array<Vector3f> &uvmapNormals);



	//DEBUG OPERATIONS
	void display() const;
	bool checkIntegrity();
};




#endif

