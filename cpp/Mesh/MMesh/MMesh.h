//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MMESH_H__
#define MMESH_H__

#include <stdio.h>

#include <set>


#include <Util/Array.h>
#include <Util/RefCountTable.h>

#include <Util/SETree.h>

#include <Math/Point3.h>
#include <Math/BBox3.h>
#include <Math/Projection.h>

#include <Brush/Brush.h>

#include <Transformation/Transformation.h>

#include <PolyBlend/BlendingPolyline.h>
#include <PolyBlend/PolyBlend.h>

#include <Graphics/Region.h>
#include <Graphics/BackfaceCull.h>

#include <Model/MarkPredicate.h>

#include <UVMap/UVMapper.h>

#include <Polyline/PolylineTypes.h>
#include <Polyline/PolylinePath.h>

#include <Mesh/MMesh/MeshTypes.h>



#define MMESH_EPSILON					3.2e-7
#define MMESH_SQR_EPSILON				( MMESH_EPSILON * MMESH_EPSILON )
#define MMESH_ONEMINUSEPSILON			( 1.0 - MMESH_EPSILON )



class GS_DllExport MMesh
{
private:
	struct MarkedFaceRegion
	{
		MFaceList faces;
		Array<MEdgeRun> boundaries;
	};

	typedef Array<MarkedFaceRegion> MarkedFaceRegionList;


	class BandsawRingEntry
	{
	public:
		MEdge *edge;
		bool flipFlag;

		inline BandsawRingEntry(MEdge *edge = NULL, bool flipFlag = false)
							: edge( edge ), flipFlag( flipFlag )
		{
		}
	};

	class BandsawRing : public Array<BandsawRingEntry>
	{
	public:
		inline BandsawRing()
		{
		}


		inline int find(const MEdge *e)
		{
			for (int edgeI = 0; edgeI < size(); edgeI++)
			{
				if ( at( edgeI ).edge == e )
				{
					return edgeI;
				}
			}

			return -1;
		}
	};

	class MeshImportRegionManager;




	Array<MVertex*> vertices;
	Array<MEdge*> edges;
	Array<MFace*> faces;


	int firstDeletedVertex, firstDeletedEdge, firstDeletedFace;

	int markedVertexCount, markedEdgeCount, markedFaceCount;



	struct Flags
	{
		unsigned finalised : 1;
		unsigned bbTreeRequiresRefresh : 1;
		unsigned bbTreeRequiresRebuild : 1;
		unsigned boundingBoxRequiresRefresh : 1;
		unsigned liveSubdRequiresRefresh : 1;
	} flags;

	MBBTree *bbTree;
	BBox3 boundingBox;


	RefCountTable<int> materialRefTable;

	MMesh *liveSubdMesh;
	int liveSubdEdgeVerticesOffset, liveSubdFaceVerticesOffset;
	MPreserveNormalSharpness liveSubdPreserveNormalSharpness;



public:
	MMesh();
	MMesh(const MMesh &mesh);
	~MMesh();


	MMesh & operator=(const MMesh &mesh);


	MMesh * clone() const;

	void read(FILE *f);
	void write(FILE *f);
	void read(SENode &node);
	void write(SENode &node);

	void clear();
	void mergeFrom(const MMesh &mesh);
	void mergeWith(const Array<const MMesh*> &meshes);



	void importMesh(const MImportMesh &importMesh);


	void preComputeFull();



	void reserveVertices(int n);
private:
	void reserveEdges(int n);
	void reserveFaces(int n);
public:
	void reserveFacesAndEdges(int nFaces);




private:
	void copyMeshDataFrom(const MMesh *mesh);


	void optimiseMemoryUsage();


	inline void incrementMarkedVertexCount()
	{
		markedVertexCount++;
	}

	inline void decrementMarkedVertexCount()
	{
		markedVertexCount--;
	}

	inline void incrementMarkedEdgeCount()
	{
		markedEdgeCount++;
	}

	inline void decrementMarkedEdgeCount()
	{
		markedEdgeCount--;
	}

	inline void incrementMarkedFaceCount()
	{
		markedFaceCount++;
	}

	inline void decrementMarkedFaceCount()
	{
		markedFaceCount--;
	}


	void vertexModified();
	void clearVertexPositionChangedForMeshFlags();
	void faceModified(MFace *face);
	void topologyModified();				// called by vertices when their topology is modified
	void destroyLiveSubdivisionMesh();
	void edgeNormalSharpnessChanged();
	void vertexNormalSharpnessChanged();
	void edgeSharpnessChanged();
	void vertexSharpnessChanged();


	MVertex * newVertex();
	MVertexAttrib * newVertexAttrib(int index);
	MVertexNormal * newVertexNormal(int index);
	MEdge * newEdge();
	MFace * newFace();



public:
	//******** MESH BUILD OPERATIONS ********
	MVertex * addVertex(const Point3 &p);

private:
	MVertexAttrib * addVertexAttrib(const MVertexAttrib &va, int index);
	MEdge * addEdge(MVertex *va, MVertex *vb);

public:
	MFace * addFace(const Array<int> &vts, const Array<Point2f> &texCoords, int materialID);
	MFace * addFace(const Array<int> &vts, const Array<MVertexAttrib> &attribs, int materialID);
	MFace * addFace(const Array<MVertex*> &vts, const Array<Point2f> &texCoords, int materialID);
	MFace * addFace(const Array<MVertex*> &vts, const Array<MVertexAttrib> &attribs, int materialID);
	MFace * addFaceAutoTexture(const Array<int> &vts, bool flipFlag, int materialID);
	MFace * addFaceAutoTexture(const Array<MVertex*> &vts, bool flipFlag, int materialID);

private:
	MFace * addFace(const Array<MVertex*> &vts, const Array<MVertexAttrib*> &attribs, int materialID);
	MFace * addFace(const Array<MVertex*> &vts, const Array<MEdge*> &eds, const Array<MVertexAttrib*> &attribs, int materialID);


public:
	inline const Array<MVertex*> &getVertices() const
	{
		return vertices;
	}

	inline const Array<MEdge*> &getEdges() const
	{
		return edges;
	}

	inline const Array<MFace*> &getFaces() const
	{
		return faces;
	}


	inline Array<MVertex*> &getVertices()
	{
		return vertices;
	}

	inline Array<MEdge*> &getEdges()
	{
		return edges;
	}

	inline Array<MFace*> &getFaces()
	{
		return faces;
	}



	inline const MVertex* getVertex(int i) const
	{
		return vertices[i];
	}

	inline const MEdge* getEdge(int i) const
	{
		return edges[i];
	}

	inline const MFace* getFace(int i) const
	{
		return faces[i];
	}


	inline MVertex* getVertex(int i)
	{
		return vertices[i];
	}

	inline MEdge* getEdge(int i)
	{
		return edges[i];
	}

	inline MFace* getFace(int i)
	{
		return faces[i];
	}



	inline int getNumVertices() const
	{
		return vertices.size();
	}

	inline int getNumEdges() const
	{
		return edges.size();
	}

	inline int getNumFaces() const
	{
		return faces.size();
	}


	inline int getNumMarkedVertices() const
	{
		return markedVertexCount;
	}

	inline int getNumMarkedEdges() const
	{
		return markedEdgeCount;
	}

	inline int getNumMarkedFaces() const
	{
		return markedFaceCount;
	}


	inline MarkStatus getVertexMarkStatus() const
	{
		if ( markedVertexCount == 0 )
		{
			return MARKSTATUS_NONE;
		}
		else if ( markedVertexCount == getNumVertices() )
		{
			return MARKSTATUS_ALL;
		}
		else
		{
			return MARKSTATUS_SOME;
		}
	}

	inline MarkStatus getEdgeMarkStatus() const
	{
		if ( markedEdgeCount == 0 )
		{
			return MARKSTATUS_NONE;
		}
		else if ( markedEdgeCount == getNumEdges() )
		{
			return MARKSTATUS_ALL;
		}
		else
		{
			return MARKSTATUS_SOME;
		}
	}

	inline MarkStatus getFaceMarkStatus() const
	{
		if ( markedFaceCount == 0 )
		{
			return MARKSTATUS_NONE;
		}
		else if ( markedFaceCount == getNumFaces() )
		{
			return MARKSTATUS_ALL;
		}
		else
		{
			return MARKSTATUS_SOME;
		}
	}



	void getPoints(Array<Point3> &points) const;


	int estimateCost() const;



private:
	void compactVertices();
	void compactEdges();
	void compactFaces();
	void compactAll();

	void destroyVertex(MVertex *v);
	void destroyEdge(MEdge *e);
	void destroyFace(MFace *f);
	void destroyVertexAttrib(MVertexAttrib *va);
	void destroyVertexNormal(MVertexNormal *n);



public:
	// Edge runs: a run will only be marked as closed if it forms a closed loop that intersects no other loop e.g. all vertices in the edge run
	// have only two incident edges that are 'edge marked'. In the case an edge run loops back to its start vertex, where the start vertex
	// has more than two incident marked edges, the edge run will loop back on itself, but will be marked as open.
	void discoverMarkedEdgeRuns(Array<MEdgeRun> &runs);
	void discoverMarkedEdgeRegions(Array<MEdgeList> &regions);

	void discoverMarkedFaces(MFaceList &markedFaces);
	void discoverMarkedFaceRegionBoundaries(MFaceList &region, Array<MEdgeRun> &boundaries);
	void discoverMarkedFaceRegions(Array<MFaceList> &regions);
	int countMarkedFaceRegions();
	void discoverMarkedFaceRegionsAndBoundaries(MarkedFaceRegionList &regions);
	void discoverMarkedFaceBoundaries(Array<MEdgeRun> &boundaries);





	//******** MESH EDIT OPERATIONS ********
	void removeMarkedFaces();


	MMesh* getLiveSubdivisionMesh(int iterations, int maxFaces, MPreserveNormalSharpness preserveNormalSharpness);

	void destroyLiveSubdivision();

private:
	void liveSubdivisionRefreshed();


	void subdivideLive(MPreserveNormalSharpness preserveNormalSharpness);
	void refreshLiveSubdivision();


public:
	void subdivide(bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness);
	void subdivideMarked(bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness);

	void extrudeMarkedFaces(MVertexAdjustList &vertexAdjusts, MExtrudeFaceDirection direction, int numSegments,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);

	void extrudeMarkedFaces(MVertexVectorAdjustList &vertexAdjusts, int numSegments,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);

	int pathExtrudeMarkedFaces(const Polyline &extrusionPath, MPathExtrudeFaceMode mode, bool correctiveScaling, PolylinePathOrientation orientation, double pathRotation,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);

private:
	void faceExtrudeFaces(int numSegments, bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges, const Polyline *extrusionPath);

public:
	void extrudeMarkedFacesSingular(MVertexAdjustList &vertexAdjusts, int numSegments,
								bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);


	void duplicateMarkedFaces(MVertexVectorAdjustList &vertexAdjusts, bool markOriginalFaces, bool markDuplicateFaces);

private:
	void insetExpandMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle,
								MInsetExpandOperation insetExpandOp, bool flowThruMarkedEdges, bool markBoundaryEdges, bool markInsetEdges,
								bool nsharpBoundaryEdges, bool nsharpInsetEdges);

public:
	void insetMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle, bool flowThruMarkedEdges,
								 bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges);
	void expandMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
								 bool markBoundaryEdges, bool markExpansionEdges, bool nsharpBoundaryEdges, bool nsharpExpansionEdges);
	void insetMarkedFacesSingular(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, MInsetFaceStyle insetStyle,
								bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges);
	void insetMarkedFacesToPoints(MVertexAdjustList &vertexAdjusts, int numSegments, bool markBoundaryEdges, bool nsharpBoundaryEdges);
	void expandMarkedVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges, bool normalSharpenBoundaryEdges);
	void bevelMarkedVertices(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
						bool useExistingTexturing, bool markBoundaryEdges, bool normalSharpenBoundaryEdges);
	void expandMarkedVerticesInMarkedFaces(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts, bool markBoundaryEdges);
	void expandMarkedEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &vertexAttribAdjusts,
						bool markBoundaryEdges, bool nsharpBoundaryEdges, bool unmarkOriginalEdges);

	void bevelMarkedEdges(MVertexAdjustList &vertexAdjusts, MVertexAttribAdjustList &attribAdjusts, bool useExistingTexturing,
						bool markBoundaryEdges, bool normalSharpedBoundaryEdges);

private:
	int discoverExtrudeableEdgeRuns(Array<MEdgeRun> &edgeRuns);

public:
	int extrudeMarkedEdges(MVertexVectorAdjustList &adjusts, MVertexAttribVectorAdjustList &attribAdjusts, int numSegments, bool useExistingTextures,
						bool bMarkExtrudedEdges);


	int fillMarkedEdges();


	int riftMarkedEdges(MVertexVectorAdjustList &adjusts, MEdgeRiftStyle style, int numSegments, bool fill, bool pinchAtMarkedVertices);



	void slideMarkedEdges(MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts, int slideVersion);


private:
	void slideMarkedEdgeRun(MEdgeRun &run, MVertexSlideAdjustList &adjusts, MVertexAttribSlideAdjustList &attribAdjusts, int slideVersion);


public:
	int collapseMarkedEdges(bool lockMarkedVertices);
	int collapseMarkedFaces();


private:
	MVertex * knifeComputeSnapVertex(const MKnifeTarget &target);
	void knifeSnap(const MKnifeTarget &target, MVertex *&vertex, Point3 &nearPoint, Point3 &farPoint, Point2 &screenPoint);
	void knifeComputePlanes(const Point3 &startNear, const Point3 &startFar, const Point3 &endNear, const Point3 &endFar, Plane &cutPlane, Plane boundingPlanes[2]);


public:
	void knife(const MKnifeTarget &startTarget, const MKnifeTarget &endTarget, bool faceMarkedOnlyFlag, const BackfaceCull *cull, bool markCutEdges);



private:
	void markEdgeRingTraverseThruFace(bool stopAtMarkedFaces, bool traverseFacesMultipleTimes, MEdge *e, MFace *f, std::list<MEdge*> &edgeStack);
	void markEdgeLoopTraverseThruVertex(bool stopAtMarkedVertices, bool traverseVerticesMultipleTimes, MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack);
	void markEdgeBoundaryTraverseThruVertex(MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack);
	void discoverEdgeRingTraverseThruFace(MEdge *e, MFace *f, std::list<MEdge*> &edgeStack, MEdgeList &ring);
	void discoverEdgeLoopTraverseThruVertex(MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack, MEdgeList &loop);
	void discoverEdgeBoundaryTraverseThruVertex(MEdge *e, MVertex *v, std::list<MEdge*> &edgeStack, MEdgeList &boundary);

	// Note that the edges in @ring will be secondary marked
	// Returns true if the ring is closed
	bool discoverMarkedEdgeRingTraverseThruFace(MEdge *e, MFace *f, BandsawRing &ring, MFaceList *ringFaces);
	// Returns true if the ring is closed
	bool discoverMarkedEdgeRing(MEdge *seed, BandsawRing &ring, MFaceList *ringFaces);
	class ConnectMarkedEdgesRingEntry
	{
	public:
		BandsawRing ring;
		MFaceList ringFaces;
		bool closedFlag;
		std::set<double> connectParameters;
		MVertex *startTerminal, *endTerminal;


		inline ConnectMarkedEdgesRingEntry() : closedFlag( false ), startTerminal( NULL ), endTerminal( NULL )
		{
		}
	};


public:
	void markEdgeRing(bool stopAtMarkedFaces, bool traverseFacesMultipleTimes);
	void markEdgeLoop(bool stopAtMarkedVertices, bool traverseVerticesMultipleTimes);
	void markEdgeBoundary();

	void discoverEdgeRing(MEdge *seed, MEdgeList &ring);
	void discoverEdgeLoop(MEdge *seed, MEdgeList &loop);
 	void discoverEdgeBoundary(MEdge *seed, MEdgeList &boundary);
 	void discoverMarkedEdgeRing(MEdge *seed, MEdgeList &ring);

 	bool discoverEdgeLoopPath(MVertex *from, MVertex *to, MEdgeRun &path);
 	bool discoverEdgeRingPath(MEdge *from, MEdge *to, MEdgeList &path);


	void connectMarkedVertices(bool markedFacesOnly, bool markCutEdges);

	void connectMarkedEdges(const MPickList &cutPickList, const MPickList &terminalVertexPickList, bool unmarkOriginalEdges);
	bool connectMarkedEdgesGetRingSegments(MEdge *seed, Array<Segment3> &ringSegments);
	void connectMarkedEdgesGetTerminalVertices(const MPickList &terminalVertexPickList, MVertexList &terminalVertices);
	void nConnectMarkedEdges(int numSegments);

	void splitEdges(const MPickList &pickList);
	void nSplitMarkedEdges(int numSegments);


	void cut(const MCutTargetList &cutTargets, MVertex *&lastSplitVertex, MFace *&currentFace);
	bool isValidCutTarget(const MCutTarget &target, MVertex *lastSplitVertex, MFace *currentFace);

private:
	bool cutChooseVertexTarget(const MPick &pick, MVertex *lastSplitVertex, MVertex *&vertex, MFace *&currentFace);
	bool cutChooseEdgeTarget(const MPick &pick, MVertex *lastSplitVertex, MEdge *&edge, MFace *&currentFace, Point3 &closestPoint);
	bool cutChooseFaceTarget(const MPick &pick, MVertex *lastSplitVertex, MFace *&currentFace);

public:
	void nCut(int numSegments, const MCutTargetList &cutTargets, MVertexList *lastSplitVertices = NULL);
	bool isValidNCutTarget(const MCutTarget &target, MVertexList &lastSplitVertices);

private:
	void nCutPerformCut(int numSegments, const MCutTarget &target, MVertexList &prevSplitVertices, MVertexList &splitVertices);
	bool nCutChooseVertexTarget(const MPick &pick, MVertexList &prevSplitVertices, MVertex *&vertex, MFace *&faceToSplit);
	bool nCutChooseEdgeTarget(const MPick &pick, MVertexList &prevSplitVertices, MEdge *&edge, MFace *&faceToSplit);


public:
	void bandsaw(const MBandsawTargetList &targetList);
	void nBandsaw(const MNBandsawTargetList &targetList);
	bool bandsawGetRingSegments(MEdge *seed, MFace *seedFace, Array<Segment3> &ringSegments, bool stopAtMarkedEdges);

private:
	// Returns true if the ring is closed
	bool discoverBandsawEdgeRing(MEdge *seed, MFace *seedFace, BandsawRing &ring, MFaceList *ringFaces, bool stopAtMarkedEdges, int &indexOfSeedInRing);
	// Note that the edges in @ring will be secondary marked
	// Returns true if the ring is closed
	bool discoverBandsawEdges(MEdge *e, MFace *f, BandsawRing &ring, MFaceList *ringFaces, bool stopAtMarkedEdges);


public:
	void roundMarkedFaces();



	int weldMarkedVertices(double thresholdDistance, bool weldLinkedVertices, bool markMergedEdges);


	void targetWeld(const MWeldTargetList &weldTargets);
	bool isValidWeldTargetVertex(MVertex *sourceVertex, MVertex *targetVertex);
	bool isValidWeldTargetEdge(MVertex *sourceVertex, MEdge *targetEdge, MWeldTarget::TargetType targetType);



	int dissolveMarkedVertices(bool twoEdgesOnly, bool twoFacesOnly, bool markCreatedFaces);
	int dissolveMarkedEdges(MEdgeDissolveStyle style, bool dissolveSimpleValenceTwoVertices);
	int dissolveMarkedFaces(bool bMarkMergedFaces);


	int spinMarkedEdges(int turns);


	int rewireEdges(const MRewireEdgeTargetList &rewireTargets);
	bool isValidRewireEdgeTarget(MEdge *edgeToRewire, MVertex *sourceVertex, MVertex *targetVertex);


private:
	int getEdgeRunsForJoining(Array<MEdgeRun> &runs);
	int getFaceBoundariesForJoining(Array<MEdgeRun> &boundaries);

public:
	int weldMarkedEdges(const PolyBlend *blend, MWeldStyle style);
	int weldMarkedFaces(const PolyBlend *blend, MWeldStyle style);



	int bridge(const PolyBlend *blend, int numSegments);

	int tunnel(const PolyBlend *blend, int numSegments);

private:
	int createTunnelFaces(const PolyBlend *blend, int numSegments, MEdgeRun &run0, MEdgeRun &run1);

	int weldEdgeRuns(const PolyBlend *blend, MWeldStyle style, Array<MEdgeRun> &runs);


	void flipFaceRegion(MFaceList &faceList);

public:
	void flip();


public:
	void drawQuads(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip, MDrawQuadsState &state);
	void drawQuads(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip);


	void drawFace(const MDrawFacePointList &points, bool bFlip, MDrawFaceState &state);
	void drawFace(const MDrawFacePointList &points, bool bFlip);


	void transform(const Matrix4 &m);



	//******** TRANSFORMATION OPERATIONS ********
public:
	void transformVerticesInit(MTransformationTarget &target, bool perGroup);
	void transformEdgesInit(MTransformationTarget &target, bool perGroup);
	void transformFacesInit(MTransformationTarget &target, bool perGroup);

	void transformVertices(const Matrix4 &m, bool markedOnly);
	void transformVertices(const Transformation *x, bool markedOnly);
	void saveVertexPositions(bool markedOnly);
	void restoreVertexPositions(bool markedOnly);

	void transformEdgeMarkedVertices(const Matrix4 &m);
	void saveEdgeMarkedVertexPositions();
	void restoreEdgeMarkedVertexPositions();

	void transformFaceMarkedVertices(const Matrix4 &m);
	void saveFaceMarkedVertexPositions();
	void restoreFaceMarkedVertexPositions();

	void transformMarkedComponents(const Matrix4 &m);
	void saveMarkedComponentPositions();
	void restoreMarkedComponentPositions();



	void vertexTransformFinalise();


private:
	void tweakAddVertex(std::set<MVertex*> &vertexSelection, std::set<MVertex*> &tweakSelection, MVertex *vertex);
	void tweakAddEdge(std::set<MEdge*> &edgeSelection, std::set<MVertex*> &tweakSelection, MEdge *edge);
	void tweakAddFace(std::set<MFace*> &faceSelection, std::set<MVertex*> &tweakSelection, MFace *face);

public:
	void tweakComponents(const MTweakComponentList &tweaks, bool bRestoreSelection);


private:
	MSurfaceTweakComponent::Target getSurfaceTweakComponentType(MSurfaceTweakComponent::Target target);

public:
	void surfaceTweakComponents(const MSurfaceTweakComponentList &tweaks, bool bRestoreSelection);


	void surfaceTweakAdjustMarkedVertices(MVertexSurfaceTweakAdjustList &adjusts, const Vector3 &viewVector);
	void surfaceTweakAdjustMarkedEdges(MVertexSurfaceTweakAdjustList &adjusts, const Vector3 &viewVector);
	void surfaceTweakAdjustMarkedFaces(MVertexSurfaceTweakAdjustList &adjusts, const Vector3 &viewVector);
	void surfaceTweakNormalAdjustMarkedVertices(MVertexAdjustList &adjusts);
	void surfaceTweakNormalAdjustMarkedEdges(MVertexAdjustList &adjusts);
	void surfaceTweakNormalAdjustMarkedFaces(MVertexAdjustList &adjusts);



	void repositionVertices(const MVertexRepositionList &targets);
	void setVertexPosition(MVertex *v, const Point3 &position);




	//******** PROPORTIONAL EDITING OPERATIONS ********
private:
	// @vertexCheckList is a temporary variable; you don't get any useful results back; its just there to save on allocations if MMesh::getBrushedVertices is called more than once
	void getBrushedVertices(const Point3 &brushPosition, const Brush &brush, Array< std::pair<MVertex*, double> > &vertexWeightPairs, Array<MVertex*> &vertexCheckList);

public:
	void proportionalAdjusterInit(const Point3 &brushPosition, const Brush &brush, MProportionalAdjuster *adjuster);

	void proportionalMove(const Point3 &brushPosition, const Brush &brush, const Vector3 &translation, MProportionalAdjuster *adjuster=NULL);
	void proportionalMoveNormal(const Point3 &brushPosition, const Brush &brush, double distance, MProportionalAdjuster *adjuster=NULL);
	void proportionalScale(const Point3 &brushPosition, const Brush &brush, double scaleFactor, MProportionalAdjuster *adjuster=NULL);
	void proportionalRotate(const Point3 &brushPosition, const Brush &brush, const Vector3 &rotationAxis, double rotation, MProportionalAdjuster *adjuster=NULL);
	void proportionalRelax(const Point3 &brushPosition, const Brush &brush, double relaxAmount, MProportionalAdjuster *adjuster=NULL);



	//******** FLATTEN OPERATIONS ********
	void flattenVertices(const Vector3 &axis, bool perRegion);
	void flattenVerticesNormal(bool perRegion);
	void flattenEdges(const Vector3 &axis, bool perRegion);
	void flattenEdgesNormal(bool perRegion);
	void flattenEdgeRuns();
	void straightenEdgeRuns();
	void flattenFaces(const Vector3 &axis, bool perRegion);
	void flattenFacesNormal(bool perRegion);



	//******** MOVE NORMAL OPERATIONS ********
	void moveVerticesNormal(MVertexAdjustList &adjusts);
	void moveEdgesNormal(MVertexAdjustList &adjusts);
	void moveFacesNormal(MVertexAdjustList &adjusts);



private:
	void discoverVertexTransformRegions(bool perRegion, Array<MVertexList> &regions);
	void discoverEdgeTransformRegions(bool perRegion, Array<MVertexList> &regionVertices);
	void discoverEdgeTransformRegions(bool perRegion, Array<MVertexList> &regionVertices, Array<MEdgeList> &regionEdges);
	void discoverFaceTransformRegions(bool perRegion, Array<MVertexList> &regionVertices);
	void discoverFaceTransformRegions(bool perRegion, Array<MVertexList> &regionVertices, Array<MFaceList> &regionFaces);

public:
	//******** SHARPNESS OPERATIONS ********

	void setMarkedEdgeSharpness(double sharpness, MSharpenFunction func);
	void setMarkedEdgeNormalSharpness(bool sharpness);
	void saveMarkedEdgeSharpness(Array<float> &sharpnessBackup);
	void restoreMarkedEdgeSharpness(Array<float> &sharpnessBackup);

	void setEdgesByVertexAsNormalSharp(MVertexList &verts);


	void setMarkedVertexSharpness(double sharpness, MSharpenFunction func);
	void setMarkedVertexNormalSharpness(bool sharpness);
	void saveMarkedVertexSharpness(Array<float> &sharpnessBackup);
	void restoreMarkedVertexSharpness(Array<float> &sharpnessBackup);


	//******** MATERIAL OPERATIONS ********
private:
	void refMaterial(int materialID);
	void unrefMaterial(int materialID);

public:
	inline void getUsedMaterialList(Array<int> &l) const
	{
		materialRefTable.getKeyList( l );
	}


	void setMaterial(int mat);
	void transformMaterial(const Matrix4 &xform, bool fixMarkedVertices);
	void transformMaterialSave(bool fixMarkedVertices, MMaterialTransformBackup &b);
	void transformMaterialRestore(bool fixMarkedVertices, const MMaterialTransformBackup &b);
public:
	void normaliseMaterialCoords(bool perGroup);

private:
	void normaliseFaceGroupMaterialCoords(MFaceList &faceList);


public:
	//UV MAPPING
	void uvMap(UVMapper *mapper);

	void uvmapMarkedEdges(bool syncAtMarkedVertices, bool invert, const Point2f &uvStart, const Point2f &uvEnd);

private:
	void uvmapInitialise();
	void uvmapShutdown();




	//******** MISC OPERATIONS ********

private:
	void vertexListToBlendingPolyline(MVertexList &v, BlendingPolyline &poly);
public:


	void markVertices_save();
	void markVertices_restore();
	void markEdges_save();
	void markEdges_restore();
	void markFaces_save();
	void markFaces_restore();

	void markVertices_all(MarkPredicate pred);
	void markEdges_all(MarkPredicate pred);
	void markFaces_all(MarkPredicate pred);

	void markVertices_edgeMarked(MarkPredicate pred);
	void markVertices_faceMarked(MarkPredicate pred);
	void markVertices_faceMarkBoundary(MarkPredicate pred);
	void markVertices_faceMarkInternal(MarkPredicate pred);
	void markVertices_boundary(MarkPredicate pred);
	void markVertices_internal(MarkPredicate pred);
	void markEdges_vertexMarked(MarkPredicate pred);
	void markEdges_whollyVertexMarked(MarkPredicate pred);
	void markEdges_faceMarked(MarkPredicate pred);
	void markEdges_faceMarkBoundary(MarkPredicate pred);
	void markEdges_faceMarkInternal(MarkPredicate pred);
	void markEdges_boundary(MarkPredicate pred);
	void markEdges_internal(MarkPredicate pred);
	void markFaces_vertexMarked(MarkPredicate pred);
	void markFaces_whollyVertexMarked(MarkPredicate pred);
	void markFaces_edgeMarked(MarkPredicate pred);
	void markFaces_whollyEdgeMarked(MarkPredicate pred);

	void markVertices_region(const Region3d *region, MarkPredicate pred);
	void markVertices_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred);
	void markEdges_region(const Region3d *region, MarkPredicate pred, bool containsAllOf);
	void markEdges_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf);
	void markFaces_region(const Region3d *region, MarkPredicate pred, bool containsAllOf);
	void markFaces_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf);

	void markVertices_pick(const MPickList &pick);
	void markEdges_pick(const MPickList &pick);
	void markEdgeRings_pick(const MPickList &pick);
	void markEdgeLoops_pick(const MPickList &pick);
	void markEdgeBoundaries_pick(const MPickList &pick);
	void markEdgeLoopPath_pick(const MPickList &pick);
	void markEdgeRingPath_pick(const MPickList &pick);
	void markFaces_pick(const MPickList &pick);


	void growMarkedVertices();
	void shrinkMarkedVertices();
	void growMarkedEdges();
	void shrinkMarkedEdges();
	void growMarkedFaces();
	void shrinkMarkedFaces();


	Side areVerticesInside(const Region3d *region) const;


	BBox3 getVerticesBBox(bool markedOnly) const;
	BBox3 getVerticesBBox(const Matrix4 &m, bool markedOnly) const;
	BBox3 getVerticesBBox(const Transformation *x, bool markedOnly) const;
	BBox3 getVerticesBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;
	BBox3 getVisibleVerticesBBox() const;

	BBox3 getMarkedEdgesBBox() const;
	BBox3 getMarkedEdgesBBox(const Matrix4 &m) const;
	BBox3 getMarkedEdgesBBox(const Transformation *x) const;
	BBox3 getMarkedEdgesBBox(const Matrix4 &m, const Transformation *x) const;

	BBox3 getMarkedFacesBBox() const;
	BBox3 getMarkedFacesBBox(const Matrix4 &m) const;
	BBox3 getMarkedFacesBBox(const Transformation *x) const;
	BBox3 getMarkedFacesBBox(const Matrix4 &m, const Transformation *x) const;

	BBox3 getMarkedFaceRegionBBox(const MFaceList &region) const;


	Vector3 computeMarkedFacesWeightedNormal() const;
	Vector3 computeWeightedNormal() const;


	void fix();

private:
	const MBBTree * getBBTree();
	const BBox3 & getBoundingBox();

public:
	MFace* raytraceFaceClipRay(Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint, int &tri);
	MFace* raytraceFaceClipRay(Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint);
	MFace* raytraceFace(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint, int &tri);
	MFace* raytraceFace(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersectionPoint);

private:
	MFace * pickFaceIncidentToVertexClosestToP(MVertex *vertex, const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag);
	MFace * pickFaceIncidentToEdgeClosestToP(MEdge *edge, const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag);

	MFace * pickFace(const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, double &t, Point3 &intersectionPoint);
	MEdge * pickEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, Point3 &closestPoint, MFace *&thruFace);
	MVertex * pickVertex(const Point2 &p, const Projection &projection, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, MFace *&thruFace);
	MVertex * pickVertexThruEdge(const Point2 &p, const Projection &projection, const Matrix4 &screenToWorldMatrix, const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, bool bSolidPick, bool bAllowPickOutsideBounds, MFace *&thruFace, MEdge *&thruEdge);

public:
	MFace * pickFace(const MPick &pick, Point3 &intersectionPoint);
	MFace * pickFace(const MPick &pick);
	MEdge * pickEdge(const MPick &pick, Point3 &closestPoint, MFace *&thruFace);
	MEdge * pickEdge(const MPick &pick, Point3 &closestPoint);
	MVertex * pickVertex(const MPick &pick, MFace *&thruFace);
	MVertex * pickVertex(const MPick &pick);
	MVertex * pickVertexThruEdge(const MPick &pick, MFace *&thruFace, MEdge *&thruEdge);
	MEdge * pickEdgeRing(MEdgeList &ring, const MPick &pick, Point3 &closestPoint, MFace *&thruFace);
	MEdge * pickEdgeRing(MEdgeList &ring, const MPick &pick, Point3 &closestPoint);
	MEdge * pickEdgeLoop(MEdgeList &loop, const MPick &pick, Point3 &closestPoint, MFace *&thruFace);
	MEdge * pickEdgeLoop(MEdgeList &loop, const MPick &pick, Point3 &closestPoint);
	MEdge * pickEdgeBoundary(MEdgeList &boundary, const MPick &pick, Point3 &closestPoint, MFace *&thruFace);
	MEdge * pickEdgeBoundary(MEdgeList &boundary, const MPick &pick, Point3 &closestPoint);



public:
	void assertFinalised() const;

public:
	void finalise();




	void checkIntegrity();
	void display();



	void buildRenderMesh(MRenderMesh &renderMesh);



private:
	enum ExpandRelativeTo
	{
		EXPANDRELATIVETO_EXPANSIONVECTOR,
		EXPANDRELATIVETO_EDGE
	};


	static Vector3 computeExpansionVector(const Array<Vector3> &normals);


	friend class GS_DllExport MVertex;
	friend class GS_DllExport MVertexAttrib;
	friend class GS_DllExport MEdge;
	friend class GS_DllExport MFace;
};





/****************************************************************************
 ****************************************************************************
						MDrawQuadsState
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MDrawQuadsState
{
private:
	int column, row, numColumns, numRows;
	bool bRowDirectionForward, bCloseRows, bCloseColumns, bFlip, bFlipFixed;
	Array<MVertex*> existingVertices;
	Array<MVertex*> quadMeshVertices;
	Array<Point3> vertexPositions;
	Array<bool> verticesValid;
	std::set<MVertex*> existingVerticesUsed;


public:
	MDrawQuadsState();


	int getColumn() const;
	int getRow() const;
	int getNumColumns() const;
	int getNumRows() const;
	bool isRowDirectionForward() const;
	bool areRowsClosed() const;
	bool areColumnsClosed() const;
	bool getQuadMeshVertexPosition(int column, int row, Point3 &pos) const;


	bool checkVertex(MVertex *v);


private:
	void drawQuads(MMesh *mesh, const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlipRequested);

	bool verifyVertex(MVertex *v, bool bAllowChangeDirection);
	int vtxIndex(int column, int row) const;
	bool verifyEdge(MVertex *v0, MVertex *v1, bool bOnBoundary, bool bAllowChangeDirection);

	friend class MMesh;
};





/****************************************************************************
 ****************************************************************************
						MDrawFaceState
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MDrawFaceState
{
private:
	int numVertices;
	bool bFlip, bFlipFixed;
	Array<MVertex*> existingVertices;
	Array<MVertex*> faceVertices;
	Array<Point3> vertexPositions;
	std::set<MVertex*> existingVerticesUsed;


public:
	MDrawFaceState();


	int getNumVertices() const;
	const Point3 & getFaceVertexPosition(int n) const;


	MDrawFaceVertexCheckResult checkVertex(MVertex *v);


private:
	void drawFace(MMesh *mesh, const MDrawFacePointList &points, bool bFlipRequested);

	MDrawFaceVertexCheckResult verifyVertex(int vertexIndex, MVertex *v, bool bAllowChangeDirection, bool bCheckNextVertex);
	bool verifyEdge(MVertex *v0, MVertex *v1, bool bAllowChangeDirection);

	friend class MMesh;
};






/****************************************************************************
 ****************************************************************************
				MProportionalAdjuster
 ****************************************************************************
 ****************************************************************************/

class GS_DllExport MProportionalAdjuster
{
private:
	Array<MVertex*> vertexCheckList;
	Array<Vector3> moveNormalTranslations;
	Array< std::pair<MVertex*, double> > vertexWeightPairs;
	Array< std::pair<Point3, double> > relaxPositionWeightPairs;


public:
	MProportionalAdjuster();
	~MProportionalAdjuster();



	void save();
	void restore();
	void move(const Vector3 &translation);
	void moveNormal(double distance);
	void scale(const Point3 &centre, double scaleFactor);
	void rotate(const Point3 &centre, const Vector3 &rotationAxis, double rotation);
	void relax(double relaxAmount);


	friend class MMesh;
};



#endif

