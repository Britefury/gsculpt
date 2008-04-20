//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MEDGE_H__
#define MEDGE_H__

#include <set>

#include <stdio.h>

#include <Util/Array.h>
#include <Util/Pool.h>
#include <Util/SETree.h>

#include <Math/Point3.h>
#include <Math/Plane.h>
#include <Math/Segment3.h>
#include <Math/Ray3.h>
#include <Math/Line3.h>

#include <Polyline/PolylineTypes.h>

#include <Graphics/Region.h>
#include <Graphics/BackfaceCull.h>

#include <Model/MarkPredicate.h>

#include <Mesh/MMesh/MeshTypes.h>
#include <Mesh/MMesh/MDijkstra.h>



//MEdge vertex ordering is the same as the vertex ordering of faceA

class GS_DllExport MEdge
{
private:
	static Pool<MEdge> edgePool;


	MVertex *vertexA, *vertexB;						//8		(8)		(1 * 8)
	MFace *faceA, *faceB;							//8		(16)		(2 * 8)
	int faceAIndex, faceBIndex;						//8		(24)		(3 * 8)
	float sharpness;								//4		(28)		(3.5 * 8)



	//
	// BevelEdgeTableVertexFlags
	//
	class BevelEdgeTableVertexFlags
	{
	public:
		bool bevelStripEndFlag, linearNeighbourhoodWithOneMarkedEdgeSpecialCaseFlag;

		inline BevelEdgeTableVertexFlags() : bevelStripEndFlag( false ), linearNeighbourhoodWithOneMarkedEdgeSpecialCaseFlag( false )
		{
		}
	};


	typedef ArrayMap<const MVertex *, BevelEdgeTableVertexFlags> BevelEdgeTable;






	union										//4		(32)		(4 * 8)
	{
		struct
		{
			MVertex *vertex;
		} subdivision;

		struct
		{
			MEdge *edge;
		} faceExtrude;

		struct
		{
			BevelEdgeTable *bevelEdgeTable;
		} bevel;

		struct
		{
			MDijkstraNode<MEdge, MEdge> *node;
		} dijkstra;
	} edgeOpData;

	struct Flags									//4		(36)		(4.5 * 8)
	{
		unsigned edgeMark : 1;
		unsigned edgeMarkSave : 1;
		unsigned destroyed : 1;
		unsigned indestructible : 1;
		unsigned secondaryMark : 1;
		unsigned creased : 1;
		unsigned normalSharp : 1;
		unsigned liveSubdVertexRequiresRefresh : 1;
		unsigned liveSubdSharpnessModified : 1;
		unsigned liveSubdNormalSharpnessModified : 1;
		unsigned liveSubdOdd : 1;
	} flags;

public:
	int index;										//4		(40)		(5 * 8)





	class EdgeProperties
	{
	private:
		float sharpness;
		bool edgeMark, secondaryMark, creased, normalSharp;


	public:
		inline EdgeProperties(float sharpness = 0.0f, bool edgeMark = false, bool secondaryMark = false, bool creased = false, bool normalSharp = false)
						: sharpness( sharpness ), edgeMark( edgeMark ), secondaryMark( secondaryMark ), creased( creased ), normalSharp( normalSharp )
		{
		}


		friend class MEdge;
	};







	MEdge(int ndx);
	~MEdge();


	void * operator new(size_t sz);
	void operator delete(void *data);
	static void poolAllocReserve(int numItmes);


	void init(MVertex *a, MVertex *b);
	void copyFrom(const MEdge *edge, MMesh *parentMesh, int verticesOffset, int facesOffset);

	void read(FILE *f, const MMesh *parentMesh);
	void write(FILE *f) const;
	void read(SENode &node, const MMesh *parentMesh);
	void write(SENode &node) const;


	void preComputeFull();




	MMesh* getMesh();

	inline int getIndex() const
	{
		return index;
	}


	inline static int estimateCost()
	{
		return sizeof(MEdge);
	}


	//************************* VERTEX OPERATIONS *************************
	//*********************************************************************
	inline MVertex * getVertexA()
	{
		return vertexA;
	}

	inline MVertex * getVertexB()
	{
		return vertexB;
	}

	inline const MVertex * getVertexA() const
	{
		return vertexA;
	}

	inline const MVertex * getVertexB() const
	{
		return vertexB;
	}

	inline MVertex * getOppositeVertex(const MVertex *v)
	{
		if ( v == vertexA )
		{
			return vertexB;
		}
		else if ( v == vertexB )
		{
			return vertexA;
		}
		else
		{
			gs_assert_not_reached( "MEdge::getOppositeVertex(): @v is not incident to @this\n" );
			return NULL;
		}
	}

	inline const MVertex * getOppositeVertex(const MVertex *v) const
	{
		if ( v == vertexA )
		{
			return vertexB;
		}
		else if ( v == vertexB )
		{
			return vertexA;
		}
		else
		{
			gs_assert_not_reached( "MEdge::getOppositeVertex(): @v is not incident to @this\n" );
			return NULL;
		}
	}


	inline bool isIncidentTo(const MVertex *v) const
	{
		return v == vertexA  ||  v == vertexB;
	}

	inline bool links(const MVertex *a, const MVertex *b) const
	{
		return ( a == vertexA  &&  b == vertexB ) || ( a == vertexB  &&  b == vertexA );
	}

	inline MVertex * getSharedVertex(const MEdge *e)
	{
		if ( vertexA == e->vertexA  ||  vertexA == e->vertexB )
		{
			return vertexA;
		}
		else if ( vertexB == e->vertexA  ||  vertexB == e->vertexB )
		{
			return vertexB;
		}
		else
		{
			return NULL;
		}
	}



	//******************** VERTEX ATTRIBUTE OPERATIONS ********************
	//*********************************************************************
	bool usesVertexAttrib(const MVertexAttrib *attrib) const;
	//if *this is a boundary edge (mesh boundary), then it is also a vertex
	//attrib boundary
	bool isVertexAttribBoundary() const;
	bool isVertexAttribBoundary(const MVertexAttrib *attrib) const;
	MVertexAttrib * getAttribAtOppositeVertex(const MVertexAttrib *attrib);
	MVertexAttrib * getVertexAttrib(MVertex *v, MFace *f);
	void getVertexAttribs(MVertex *v, MVertexAttrib *&attribA, MVertexAttrib *&attribB);
	void getVertexAttribs(MFace *f, MVertexAttrib *&attribA, MVertexAttrib *&attribB);
	Vector2f getVertexAttribVector(MFace *incidentTo, const MVertex *fromVertex);



	//********************** VERTEX NORMAL OPERATIONS *********************
	//*********************************************************************
	inline bool isNormalSharp() const
	{
		return flags.normalSharp;
	}

	void setNormalSharp();
	void setNormalSmooth();
	void setNormalSharpness(bool s);



	//************************** FACE OPERATIONS **************************
	//*********************************************************************
	inline MFace * getFaceA()
	{
		return faceA;
	}

	inline MFace * getFaceB()
	{
		return faceB;
	}

	inline const MFace * getFaceA() const
	{
		return faceA;
	}

	inline const MFace * getFaceB() const
	{
		return faceB;
	}

	inline MFace * getOppositeFace(const MFace *f)
	{
		if ( f == faceA )
		{
			return faceB;
		}
		else if ( f == faceB )
		{
			return faceA;
		}
		else
		{
			gs_assert_not_reached( "MEdge::getOppositeFace(): @f is not incident to @this\n" );
			return NULL;
		}
	}

	inline const MFace * getOppositeFace(const MFace *f) const
	{
		if ( f == faceA )
		{
			return faceB;
		}
		else if ( f == faceB )
		{
			return faceA;
		}
		else
		{
			gs_assert_not_reached( "MEdge::getOppositeFace(): @f is not incident to @this\n" );
			return NULL;
		}
	}



	inline bool isIncidentTo(const MFace *f) const
	{
		return  faceA == f  ||  faceB == f;
	}

	inline bool links(const MFace *a, const MFace *b)
	{
		return ( a == faceA  &&  b == faceB )  ||  ( a == faceB  &&  b == faceA );
	}

	inline int getNumFaces() const
	{
		int count = faceA == NULL  ?  0 : 1;
		return faceB == NULL  ?  count : count + 1;
	}



	inline void setFaceIndex(const MFace *f, int index)
	{
		if ( f == faceA )
		{
			faceAIndex = index;
		}
		else if ( f == faceB )
		{
			faceBIndex = index;
		}
		else
		{
			gs_assert_not_reached( "MEdge::setFaceIndex(): @this edge is not incident to @f\n" );
		}
	}

	int getFaceIndex(const MFace *f) const
	{
		if ( f == faceA )
		{
			return faceAIndex;
		}
		else if ( f == faceB )
		{
			return faceBIndex;
		}
		else
		{
			gs_assert_not_reached( "MEdge::getFaceIndex(): @this edge is not incident to @f\n" );
			return -1;
		}
	}


	inline MFace* getSharedFace(const MEdge *e)
	{
		if ( faceA == e->faceA  ||  faceA == e->faceB )
		{
			return faceA;
		}
		else if ( faceB == e->faceA  ||  faceB == e->faceB )
		{
			return faceB;
		}
		else
		{
			return NULL;
		}
	}

	MFace * getQuadFace();

	MFace * getSharedFace(const MVertex *v);

	MFace * getMarkedFace();
	MFace * getUnmarkedFace();
	MFace * getSecondaryUnmarkedFace();

	//out of the two vertices that make up @this, @v is the first to appear in @f, @index is the index of @this in @f
	void addFace(MFace *f, MVertex *v, int index);
	void removeFace(MFace *f, bool destroyIsolatedEdges, bool destroyIsolatedVertices);

	int getFaceAVertexIndex(const MVertex *v) const;
	int getFaceBVertexIndex(const MVertex *v) const;
	int getFaceVertexAIndex(const MFace *f) const;
	int getFaceVertexBIndex(const MFace *f) const;
	int getFaceVertexIndex(const MFace *f, const MVertex *v) const;

	MFace * findCommonFaceWith(const MVertex *v) const;

	MFace * findBestSplitFace(MVertex *splitTo);
	MFace * findBestSplitFace(const MVertexList &splitTargets);
	MFace * findBestSplitFace(MEdge *splitTo);



	//************************ EDGE LINK OPERATIONS ***********************
	//*********************************************************************
	MEdge * getNextEdge(MFace *f, const MVertex *v) const;
	void getAdjacentEdges(MVertex *v, MEdge *&thruFaceA, MEdge *&thruFaceB);



	//************************ DESTROY OPERATIONS *************************
	//*********************************************************************
	inline void markAsDestroyed()
	{
		flags.destroyed = true;
	}

	inline bool isDestroyed() const
	{
		return flags.destroyed;
	}

	inline void setIndestructible()
	{
		flags.indestructible = true;
	}

	inline void setDestructible()
	{
		flags.indestructible = false;
	}

	inline bool isIndestructible()
	{
		return flags.indestructible;
	}

	bool destroyIfIsolated(bool destroyIsolatedVertices);



	//************************ TOPOLOGY OPERATIONS ************************
	//*********************************************************************
	inline bool isBoundary() const
	{
		return faceB == NULL;
	}

	inline bool isIsolated() const
	{
		return faceA == NULL;
	}



	//*********************** SHARPNESS OPERATIONS ************************
	//*********************************************************************
	void setSharpness(float s);

	inline void setSharpness(float s, MSharpenFunction func)
	{
		switch ( func )
		{
		case MSHARPENFUNC_SET:
			setSharpness( s );
			setCreased( s > 0.0 );
			break;
		case MSHARPENFUNC_ACCUMULATE:
			setSharpness( sharpness + s );
			setCreased( s > 0.0 );
			break;
		default:
			break;
		}
	}

	inline float getSharpness() const
	{
		return sharpness;
	}

	inline void crease()
	{
		flags.creased = true;
	}

	inline void uncrease()
	{
		flags.creased = false;
	}

	inline void setCreased(bool c)
	{
		flags.creased = c;
	}

	inline bool isCreased() const
	{
		return flags.creased;
	}



	//************************* MISC OPERATIONS **************************
	//*********************************************************************
	inline void flip()
	{
		//swap vertices
		MVertex *vertexSwap = vertexA;
		vertexA = vertexB;
		vertexB = vertexSwap;

		//swap faces
		MFace *faceSwap = faceA;
		faceA = faceB;
		faceB = faceSwap;

		int faceIndexSwap = faceAIndex;
		faceAIndex = faceBIndex;
		faceBIndex = faceIndexSwap;
	}

	inline void swapVertices()
	{
		MVertex *vertexSwap = vertexA;
		vertexA = vertexB;
		vertexB = vertexSwap;
	}

	double getLength() const;
	double getLengthSquared() const;
	static double computeMaxLengthSquared(const MEdgeList &edgeList);
	static double computeMaxLength(const MEdgeList &edgeList);
	Segment3 getSegment() const;
	Segment3 getSegment(const MVertex *fromVertex) const;
	Ray3 getRay() const;
	Ray3 getRay(const MVertex *fromVertex) const;
	Line3 getLine() const;
	Vector3 getEdgeVector() const;
	Vector3 getEdgeVector(const MVertex *fromVertex) const;
	Vector3 getNormal() const;
	Vector3 getEdgeAndNormalPlaneNormal() const;
	Vector3 getEdgeAndNormalPlaneNormal(const MVertex *fromVertex) const;
	bool intersect(const Plane &plane, double &t) const;
	double computeIntersection(const Plane &plane) const;
	bool intersectStrict(const Plane &plane, double &t) const;
	bool intersects(const Plane &plane) const;
	Point3 getPoint(double t) const;
	Point3 getMidPoint() const;




	//*********************** EDGE MARK OPERATIONS ************************
	//*********************************************************************
	void edgeMark();
	void edgeUnmark();
	void edgeMarkInvert();
	void setEdgeMark(bool mark);

	inline void setEdgeMark(MarkPredicate pred, bool inRegion)
	{
		setEdgeMark( markPredicateIsMarked( pred, flags.edgeMark, inRegion ) );
	}

	inline bool isEdgeMarked() const
	{
		return flags.edgeMark;
	}

	inline void edgeMarkSave()
	{
		flags.edgeMarkSave = flags.edgeMark;
	}

	inline void edgeMarkRestore()
	{
		setEdgeMark( flags.edgeMarkSave );
	}


	bool hasNeighbouringMarkedEdges() const;
	bool hasNeighbouringUnmarkedEdges() const;


	void generateMarkedEdgeRun(MVertex *v, MEdgeRun &run, MVertex *&lastVertex);

	void getEdgeMarkedRegion(MEdgeList &region);



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

	inline void setSecondaryMark(bool m)
	{
		flags.secondaryMark = m;
	}

	inline bool isSecondaryMarked() const
	{
		return flags.secondaryMark;
	}



	//*********************** PROPERTIES OPERATIONS ***********************
	//*********************************************************************
	inline void copyPropertiesFrom(const MEdge *e)
	{
		setEdgeMark( e->isEdgeMarked() );
		flags.secondaryMark = e->flags.secondaryMark;
		flags.creased = e->flags.creased;
		flags.normalSharp = e->flags.normalSharp;
		sharpness = e->sharpness;
	}

	inline EdgeProperties getProperties() const
	{
		return EdgeProperties( getSharpness(), isEdgeMarked(), isSecondaryMarked(), isCreased(), isNormalSharp() );
	}

	inline void setProperties(const EdgeProperties &props)
	{
		setSharpness( props.sharpness );
		setEdgeMark( props.edgeMark );
		setSecondaryMark( props.secondaryMark );
		setCreased( props.creased );
		setNormalSharpness( props.normalSharp );
	}



	//********************** VERETX MARK OPERATIONS ***********************
	//*********************************************************************
	bool isVertexMarked() const;
	bool isWhollyVertexMarked() const;



	//*********************** FACE MARK OPERATIONS ************************
	//*********************************************************************
	int getFaceMarkCount() const;
	bool isFaceMarkInternal() const;
	bool isFaceMarkBoundary() const;
	bool isFaceMarked() const;
	bool hasFaceMarkedVertices() const;

	void getFaceMarkBoundaryEdges(MEdgeRun &boundary);


	int getFaceSecondaryMarkCount() const;
	bool isFaceSecondaryMarkInternal() const;
	bool isFaceSecondaryMarkBoundary() const;
	bool isFaceSecondaryMarked() const;




	//********************** PATH DISCOVERY OPERATIONS ***********************
	//*********************************************************************
	bool discoverEdgeRingPathTo(MEdge *target, MEdgeList &path);

	void discoverPathInit();
	void discoverPathFinish();


	inline MDijkstraNode<MEdge, MEdge> * getDijkstraNode()
	{
		return edgeOpData.dijkstra.node;
	}


private:
	void discoverPathSetCost(MDijkstraHeap<MEdge> &heap, double cumulativeCost, MEdge *previous);


public:
	//********************** SUBDIVISION OPERATIONS ***********************
	//*********************************************************************
	inline MVertex * getSubdivisionVertex()
	{
		return edgeOpData.subdivision.vertex;
	}

	void destroySubdivisionVertex();

	inline void liveSubdivisionIncidentFaceVertexModified()
	{
		flags.liveSubdVertexRequiresRefresh = true;
		//dont need to pass the message on to incident vertices, since
		//liveSubdivisionIncidentFaceVertexModified() is called by
		//MFace::liveSubdivisionIncidentVertexModified(), which sends
		//the message to incident vertices
	}

	void liveSubdivisionRefreshInit();

	inline void liveSubdivisionRefreshed()
	{
		flags.liveSubdVertexRequiresRefresh = false;
		flags.liveSubdSharpnessModified	= false;
		flags.liveSubdNormalSharpnessModified = false;
	}


	inline void setLiveSubdivisionOdd(bool odd)
	{
		flags.liveSubdOdd = odd;
	}

	inline void liveSubdivisionOdd()
	{
		flags.liveSubdOdd = true;
	}

	inline void liveSubdivisionEven()
	{
		flags.liveSubdOdd = false;
	}

	inline bool isLiveSubdivisionOdd() const
	{
		return flags.liveSubdOdd;
	}

	inline bool isLiveSubdivisionEven() const
	{
		return !flags.liveSubdOdd;
	}


private:
	Point3 computeLiveSubdivisionVertexPosition(MMesh *liveMesh, int faceSubdVerticesOffset);
public:
	void createLiveSubdivisionVertex(MMesh *liveMesh, int faceSubdVerticesOffset);
	void refreshLiveSubdivisionVertex(MMesh *liveMesh, int edgeSubdVerticesOffset, int faceSubdVerticesOffset);


	void createCatmullClarkSubdivisionVertex();
	void createLinearSubdivisionVertex();




	MVertexAttrib* getSubdivisionVertexAttrib(const MFace *f);
	void subdivideMarkedBoundary();
private:
	MVertexAttrib* subdivisionLinearSubdivideAttrib(MFace *f, MVertex *newVertex);
public:



	//EXTRUDE FACE OPERATIONS
	void extrudeFaceCreateExtrudedDuplicates(bool markBoundaryEdges, bool nsharpBoundaryEdges, const Polyline *extrusionPath);
	void extrudeFaceExtrudeToQuads(int numSegments, int materialID, float u[2],
								bool markExtrudedEdges, bool nsharpExtrudedEdges, const PolylineEdgeTextureTable &ptex);


	//INSET/EXPAND FACE OPERATIONS
private:
	void insetExpandFaceSplitOffIncidentQuad(MFace *fromFace, MInsetExpandOperation insetExpandOp, bool markBoundaryEdges, bool nsharpBoundaryEdges);
public:
	void insetExpandFaceSplitOffIncidentQuads(MInsetExpandOperation insetExpandOp, bool markBoundaryEdges, bool nsharpBoundaryEdges);

	inline bool isInsetFaceFlowEdge(bool flowThruMarkedEdges)
	{
		return flowThruMarkedEdges  &&  isFaceMarkBoundary()  &&  isEdgeMarked();
	}

	inline bool isInsetExpandFaceInternal(MInsetExpandOperation insetExpandOp) const
	{
		if ( insetExpandOp == MINSETEXPANDOP_INSET )
		{
			return isFaceMarkInternal();
		}
		else
		{
			return !isFaceMarked();
		}
	}


	//EXPAND FACE OPERATIONS
	inline bool isExpandFaceBoundary() const
	{
		return isFaceMarkBoundary()  &&  !isBoundary();
	}


	//EXPAND EDGE OPERATIONS
	void expandEdgeSplitOffIncidentQuads(MEdgeList &boundaryEdges);



	//*********************** BEVEL EDGE OPERATIONS ***********************
	//*********************************************************************
	void bevelInit();
	void bevelShutdown();


	inline void setBevelStripEndPoint(const MVertex *v, bool state)
	{
		BevelEdgeTable::iterator iter = edgeOpData.bevel.bevelEdgeTable->find( v );

		gs_assert( iter != edgeOpData.bevel.bevelEdgeTable->end(), "MEdge::setBevelStripEndPoint() @v is not incident to @this\n" );

		iter->second.bevelStripEndFlag = state;
	}

	inline bool isBevelStripEndPoint(const MVertex *v)
	{
		BevelEdgeTable::iterator iter = edgeOpData.bevel.bevelEdgeTable->find( v );

		gs_assert( iter != edgeOpData.bevel.bevelEdgeTable->end(), "MEdge::isBevelStripEndPoint() @v is not incident to @this\n" );

		return iter->second.bevelStripEndFlag;
	}

	inline void setBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase(const MVertex *v, bool state)
	{
		BevelEdgeTable::iterator iter = edgeOpData.bevel.bevelEdgeTable->find( v );

		gs_assert( iter != edgeOpData.bevel.bevelEdgeTable->end(), "MEdge::setBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase() @v is not incident to @this\n" );

		iter->second.linearNeighbourhoodWithOneMarkedEdgeSpecialCaseFlag = state;
	}

	inline bool isBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase(const MVertex *v)
	{
		BevelEdgeTable::iterator iter = edgeOpData.bevel.bevelEdgeTable->find( v );

		gs_assert( iter != edgeOpData.bevel.bevelEdgeTable->end(), "MEdge::isBevelEdgeLinearNeighbourhoodWithOneMarkedEdgeSpecialCase() @v is not incident to @this\n" );

		return iter->second.linearNeighbourhoodWithOneMarkedEdgeSpecialCaseFlag;
	}



	//********************** EXTUDE EDGE OPERATIONS ***********************
	//*********************************************************************
	inline bool isExtrudeable() const
	{
		return isEdgeMarked()  &&  isBoundary()  &&  !isSecondaryMarked();
	}

	void generateExtrudeableEdgeRun(MEdgeRun &run, MVertex *startAt);


	//KNIFE METHODS
	void knifeSplit(double t);



	//EDGE SPLIT OPERATIONS
private:
	void splitEdge(double fraction, MVertex *&splitVertex, MEdge *&newEdgeB);
public:
	MVertex * split(double fraction, MEdge *&newEdgeB);
	MVertex * split(MVertex *fromVertex, double fraction, MEdge *&newEdge0, MEdge *&newEdge1);
	MVertex * splitAdjustable(MVertex *fromVertex, MVertexAdjustList &vertexAdjusts, const Vector3 &adjustVectorV,		//split adjusts along U, this is V
						MVertexAttribAdjustList &vertexAttribAdjusts, double scaleFactor, MEdge *&newEdge0, MEdge *&newEdge1,
						bool constrainLowerAdjustment = true, bool constrainUpperAdjustment = true);
	void splitIntoSegments(MVertex *from, int numSegments, MVertexList &newVertices, MEdgeList &splitEdges);
	void splitIntoSegments(int numSegments, MVertexList &newVertices, MEdgeList &splitEdges);
	void splitIntoSegments(const std::set<double> &parameters, bool reverseFlag, MVertexList &newVertices, MEdgeList &splitEdges);


	//INTEROPOLATED ADJUSTMENT OPERATIONS
	void interpolatedAdjust(MVertex *fromVertex, MVertex *vertexToAdjust, MVertexAdjustList &vertexAdjusts, const Vector3 &adjustVectorV,
						MVertexAttribAdjustList &vertexAttribAdjusts, double scaleFactor);


	//FACE SPLIT OPERATIONS
	void replaceFace(MFace *fOld, MFace *fNew, int index);


	//EDGE COLLAPSE OPERATIONS
	MVertex * collapse(const Point3 &vertexPosition, bool markMergedEdges, MCollapseEdgeTo collapseTo);
	MVertex * collapse(bool markMergedEdges, bool lockMarkedVertices);
	MVertex * collapseTo(const Point3 &vertexPosition, bool markMergedEdges, const MVertex *v);
	bool canCollapse();


	//EDGE DISSOLVE OPERATIONS
private:
	void dissolveAttribs(MVertex *vertex, MFace *destFace);
	static void dissolve(MFace *from, MFace *into, bool dissolveAttribs);

public:
	MFace * dissolve(MEdgeDissolveStyle style = MEDGEDISSOLVESTYLE_MIDPOINT_MAT_A);
	bool canDissolve() const;


	//EDGE SPIN OPERATIONS
	bool spin(int turns);


	//REWIRE EDGE OPERATIONS
	bool canRewire() const;



	//UV-MAP OPERATIONS
	void uvmapEdge(const Point2f &uvVertexA, const Point2f &uvVertexB);


	//BOUNDARY / MARK METHODS
	bool isFrontFacing(const BackfaceCull *faceCull) const;
	bool isWhollyInside(const Region3d *region) const;
	bool isPartiallyInside(const Region3d *region) const;
	bool isWhollyInside(const Region3d *region, const BackfaceCull *faceCull) const;
	bool isPartiallyInside(const Region3d *region, const BackfaceCull *faceCull) const;

	//DEBUG METHODS
	void display();
	bool checkIntegrity();
};



class GS_DllExport MEdgeRun : public Array<MEdge*>
{
private:
	bool closedFlag, flippedFlag;

public:
	class GS_DllExport VertexIterator
	{
	private:
		MEdgeRun *edgeRun;
		MVertex *current;
		int index;

	public:
		inline VertexIterator()
		{
			edgeRun = NULL;
			current = NULL;
			index = -1;
		}

	private:
		inline VertexIterator(MEdgeRun *run, MVertex *cur, int ndx)
		{
			edgeRun = run;
			current = cur;
			index = ndx;
		}


	public:
		inline MVertex & operator*()
		{
			return *current;
		}

		inline MVertex * operator->()
		{
			return current;
		}

		inline MVertex * vertex()
		{
			return current;
		}


		VertexIterator& operator++();
		VertexIterator operator++(int);
		VertexIterator& operator--();
		VertexIterator operator--(int);

		friend class GS_DllExport MEdgeRun;
	};





	MEdgeRun();
	MEdgeRun(bool c);

	bool isClosed() const;
	void close();
	void open();

	int getNumVertices() const;

	void flip();


	//if the edge run is CLOSED, the first vertex is NOT repeated at the end
	void extractVertices(MVertexList &vertices);
	void extractVerticesAndAttribs(MVertexList &vertices, MVertexAttribList &attribs);
	MVertex * getFirstVertex();
	double computeLength() const;
	void generateCumulativeLengthArray(Array<double> &cumulativeLengthArray);
	void generateUnitCumulativeLengthArray(Array<double> &cumulativeLengthArray);
	bool isEdge0InOrder() const;
	bool isEdgeInOrder(int index) const;


	VertexIterator verticesBegin();
};



#endif
