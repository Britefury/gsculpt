//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCTMESH_H__
#define GSPRODUCTMESH_H__

#include <boost/python.hpp>


#include <Math/IndexTriangle.h>
#include <Math/Point2f.h>

#include <Util/Array.h>
#include <Util/UniqueID.h>

#include <Brush/Brush.h>

#include <Graphics/BackfaceCull.h>
#include <Graphics/Region.h>

#include <Polyline/PolylineTypes.h>
#include <Polyline/PolylinePath.h>

#include <PolyBlend/PolyBlend.h>

#include <Product/GSProduct.h>

#include <Model/GSProductModel.h>

#include <Mesh/MMesh/MeshTypes.h>

#include <UVMap/UVMapper.h>




class GS_DllExport MeshVertexAdjustment;
class GS_DllExport MeshVertexVectorAdjustment;
class GS_DllExport MeshVertexSlideAdjustment;
class GS_DllExport MeshVertexSurfaceTweakAdjustment;
class GS_DllExport MeshAttribAdjustment;
class GS_DllExport MeshAttribVectorAdjustment;
class GS_DllExport MeshAttribSlideAdjustment;
class GS_DllExport MeshTransformation;
class GS_DllExport MeshMaterialTransformBackup;

class GS_DllExport MeshSharpnessBackup;

class GS_DllExport MeshSurfaceTweakAdjuster;
class GS_DllExport MeshSurfaceTweakNormalAdjuster;

class GS_DllExport MeshProportionalAdjuster;

class GS_DllExport MeshMoveNormalAdjuster;
class GS_DllExport MeshExpandVerticesAdjuster;
class GS_DllExport MeshExpandVerticesInMarkedFacesAdjuster;
class GS_DllExport MeshBevelVerticesAdjuster;
class GS_DllExport MeshExpandEdgesAdjuster;
class GS_DllExport MeshBevelEdgesAdjuster;
class GS_DllExport MeshExtrudeEdgesAdjuster;
class GS_DllExport MeshRiftEdgesAdjuster;
class GS_DllExport MeshSlideEdgesAdjuster;
class GS_DllExport MeshInsetFacesAdjuster;
class GS_DllExport MeshBevelFacesAdjuster;
class GS_DllExport MeshExpandFacesAdjuster;
class GS_DllExport MeshExtrudeFacesAdjuster;
class GS_DllExport MeshVectorExtrudeFacesAdjuster;


class GS_DllExport MeshCutState;
class GS_DllExport MeshNCutState;
class GS_DllExport MeshDrawQuadsState;
class GS_DllExport MeshDrawFaceState;




/*********************************************************************************
												GSProductMesh
 *********************************************************************************/

class GS_DllExport GSProductMesh : public GSProductModel
{
RT_TYPE;
protected:
	class GS_DllExport MeshRep : public ModelRep
	{
	RT_TYPE;
	protected:
		MMesh *mesh;

	public:
		MeshRep();
		MeshRep(const MeshRep &rep);
		virtual ~MeshRep();


	protected:
		virtual void read(FILE *f, ObjectRepTable *repTable);
		virtual void write(FILE *f, ObjectRepTable *repTable);
		virtual void read(SENode &node, ObjectRepTable *repTable);
		virtual void write(SENode &node, ObjectRepTable *repTable);

		virtual int estimateCost() const;

	public:
		MMesh * getMesh();
		const MMesh * getMesh() const;

	protected:
		virtual void transform(const Matrix4 &xform);
		virtual void markAll(bool markState);

		virtual void i_paint3dEndNotify();

		friend class GSProductPolylineBase;
	};


public:
	GSProductMesh();
	GSProductMesh(const GSProductMesh &m);
protected:
	GSProductMesh(ObjectRep *rep);
public:
	virtual ~GSProductMesh();


	virtual std::string getDescription() const;

	int estimateCost() const;


	virtual void copyFrom(const GSProduct *o);


	virtual void read(SENode &node, ObjectRepTable *repTable);
	virtual void write(SENode &node, ObjectRepTable *repTable);


	virtual void preComputeFull();


protected:
	MMesh * getRepMesh();
	MMesh * getReadOnlyRepMesh();
	const MMesh * getReadOnlyRepMesh() const;


public:
	int getNumVertices() const;
	int getNumEdges() const;
	int getNumFaces() const;


	MMesh & getMMeshForDisplay();


	virtual void flip();



protected:
	virtual void i_transform(const Matrix4 &m, bool markedOnly);
	virtual void i_transform(const Transformation *x, bool markedOnly);

	virtual void i_transformPointsSave(bool markedOnly);
	virtual void i_transformPointsRestore(bool markedOnly);


	virtual void i_markPoints(MarkPredicate predicate, const Region3d *region);
	virtual void i_markAllPoints(MarkPredicate predicate);
	virtual void i_markPointsSave();
	virtual void i_markPointsRestore();
	virtual MarkStatus i_getMarkStatus() const;
	virtual Side i_arePointsInside(const Region3d *region) const;


	virtual BBox3 i_getBBox(bool markedOnly) const;
	virtual BBox3 i_getBBox(const Matrix4 &m, bool markedOnly) const;
	virtual BBox3 i_getBBox(const Transformation *x, bool markedOnly) const;
	virtual BBox3 i_getBBox(const Matrix4 &m, const Transformation *x, bool markedOnly) const;


	virtual bool i_raytraceClipRay(Segment3 &ray, bool backfaceCullingFlag, double &t, Point3 &intersection);

public:
	virtual bool canMergeWith(GSProductModel *withObject) const;

protected:
	virtual GSProductModel * i_mergedWith(const Array<GSProductModel*> &models);



public:
	// ********************** MESH SPECIFIC METHODS **********************

	void clear();

	void importMesh(const MImportMesh &importMesh);

	int addVertex(const Point3 &location, bool marked = false);
	int addFace(const Array<int> &vertexIndices, const Array<Point2f> &textureCoords);
	int addFace(int va, const Point2f &ta, int vb, const Point2f &tb, int vc, const Point2f &tc);
	int addFace(int va, const Point2f &ta, int vb, const Point2f &tb, int vc, const Point2f &tc, int vd, const Point2f &td);

	int addFace(const Array<Point2f> &textureCoordinates, const Array<int> &vertexIndices, int startAt, bool reverseVertexOrder);


	void setEdgesAsNormalSharp(const Array<int> &vertexIndices, int offset, bool closed);


	void reserveVertices(int s);
	void reserveFaces(int s);



	int getNumMarkedVertices() const;
	int getNumMarkedEdges() const;
	int getNumMarkedFaces() const;



	void addLoftFaces(const Polyline &poly, int loftSegments, const Array<int> &vertexIndices);
	//poly0 : v
	//poly1 : u
	void addLoftFaces(const Polyline &poly1, const Polyline &poly0, const Array<int> &vertexIndices);

	//capping faces
	void addCapFace(const Polyline &poly, const Array<int> &vertexIndices, Axis planeNormal, int capOffset, bool flipCap, bool sharpenBoundaries);
	void addCapFaces(const Polyline &poly, const Array<int> &vertexIndices, Axis planeNormal, int startCapOffset, int endCapOffset,
					bool flipStartCap, bool flipEndCap, bool sharpenBoundaries);

	//tesselated polyline
	void addTesselatedPolyline(const Polyline &poly, bool flip, Axis planeNormal);




	// ********************** MESH RAYTRACE METHODS **********************
	int raytraceFace(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersection);
	int raytraceFaceAttribPoint(const Segment3 &ray, bool backfaceCullingFlag, bool faceMarkedOnlyFlag, double &t, Point3 &intersection, Point2 &attribPoint);

	// ********************** MESH PICK METHODS **********************
	int pickFace(const MPick &pick, Point3 &intersectionPoint);
	int pickFace(const MPick &pick);
	int pickEdge(const MPick &pick, Point3 &closestPoint, int &thruFaceIndex);
	int pickEdge(const MPick &pick, Point3 &closestPoint);
	int pickVertex(const MPick &pick, int &thruFaceIndex);
	int pickVertex(const MPick &pick);
	int pickVertexThruEdge(const MPick &pick, int &thruFaceIndex, int &thruEdgeIndex);
	int pickEdgeRing(Array<int> &ringEdgeIndices, const MPick &pick, Point3 &closestPoint, int &thruFaceIndex);
	int pickEdgeRing(Array<int> &ringEdgeIndices, const MPick &pick, Point3 &closestPoint);
	int pickEdgeLoop(Array<int> &loopEdgeIndices, const MPick &pick, Point3 &closestPoint, int &thruFaceIndex);
	int pickEdgeLoop(Array<int> &loopEdgeIndices, const MPick &pick, Point3 &closestPoint);
	int pickEdgeBoundary(Array<int> &boundaryEdgeIndices, const MPick &pick, Point3 &closestPoint, int &thruFaceIndex);
	int pickEdgeBoundary(Array<int> &boundaryEdgeIndices, const MPick &pick, Point3 &closestPoint);

	void discoverEdgeRing(int seedIndex, Array<int> &ringEdgeIndices);
	void discoverEdgeLoop(int seedIndex, Array<int> &loopEdgeIndices);
	void discoverEdgeBoundary(int seedIndex, Array<int> &boundaryEdgeIndices);

 	bool discoverEdgeLoopPath(int fromVertexIndex, int toVertexIndex, Array<int> &pathEdgeIndices);
 	bool discoverEdgeRingPath(int fromEdgeIndex, int toEdgeIndex, Array<int> &pathEdgeIndices);



private:
	void py_indexListToPyList(boost::python::list pyList, const Array<int> &indexList);

public:
	boost::python::tuple py_pickVertex(const MPick &pick);				// -> vertexIndex, thruFaceIndex
	boost::python::tuple py_pickVertexThruEdge(const MPick &pick);		// -> vertexIndex, thruFaceIndex, thruEdgeIndex
	boost::python::tuple py_pickEdge(const MPick &pick);				// -> edgeIndex, closestPoint, thruFaceIndex
	boost::python::tuple py_pickFace(const MPick &pick);				// -> faceIndex, intersectionPoint
	boost::python::tuple py_pickEdgeRing(const MPick &pick);				// -> [ edgeIndices ], seedEdgeIndex, closestPoint, thruFaceIndex
	boost::python::tuple py_pickEdgeLoop(const MPick &pick);			// -> [ edgeIndices ], seedEdgeIndex, closestPoint, thruFaceIndex
	boost::python::tuple py_pickEdgeBoundary(const MPick &pick);			// -> [ edgeIndices ], seedEdgeIndex, closestPoint, thruFaceIndex

	boost::python::list py_discoverEdgeRing(int seedIndex);
	boost::python::list py_discoverEdgeLoop(int seedIndex);
	boost::python::list py_discoverEdgeBoundary(int seedIndex);
	boost::python::tuple py_discoverEdgeLoopPath(int fromVertexIndex, int toVertexIndex);		// -> bSuccess, [ edgeIndices ]
	boost::python::tuple py_discoverEdgeRingPath(int fromEdgeIndex, int toEdgeIndex);			// -> bSuccess, [ edgeIndices ]



	// ********************** FINALISE **********************
	void finalise();



	//mesh ops
	void mirror(const Plane &mirrorPlane);

	void subdivide(bool markedFacesOnly, bool catmullClark, MPreserveNormalSharpness preserveNormalSharpness);

	void knife(const MKnifeTarget &startTarget, const MKnifeTarget &endTarget, bool faceMarkedOnly, const BackfaceCull *cull, bool markCutEdges);



	//vertex ops
	void expandMarkedVertices(MeshExpandVerticesAdjuster &adjuster, bool markBoundaryEdges, bool nsharpBoundaryEdges);
	void expandMarkedVerticesInMarkedFaces(MeshExpandVerticesInMarkedFacesAdjuster &adjuster, bool markBoundaryEdges);

	void bevelMarkedVertices(MeshBevelVerticesAdjuster &adjuster, bool useExistingTexturing, bool markBoundaryEdges, bool nsharpBoundaryEdges);

	void connectMarkedVertices(bool markedFacesOnly, bool markCutEdges);

	void weldMarkedVertices(double threshold, bool weldLinkedVertices, bool markMergedEdges);

	void targetWeld(const MWeldTargetList &weldTargets);
	bool isValidWeldTargetVertex(int sourceVertexIndex, int targetVertexIndex);
	bool isValidWeldTargetEdge(int sourceVertexIndex, int targetEdgeIndex, MWeldTarget::TargetType targetType);


	int dissolveMarkedVertices(bool twoEdgesOnly, bool twoFacesOnly, bool markCreatedFaces);

	void setMarkedVertexSharpness(double sharpness, MSharpenFunction sharpenFunction);
	void saveMarkedVertexSharpness(MeshSharpnessBackup &sharpnessBackup);
	void restoreMarkedVertexSharpness(MeshSharpnessBackup &sharpnessBackup);

	void setMarkedVertexNormalSharpness(bool sharpness);



	//edge ops
	void expandMarkedEdges(MeshExpandEdgesAdjuster &adjuster, bool markBoundaryEdges, bool nsharpBoundaryEdges, bool unmarkExtrudedEdges);

	void bevelMarkedEdges(MeshBevelEdgesAdjuster &adjuster, bool useExistingTexturing, bool markBoundaryEdges, bool nsharpBoundaryEdges);

	int extrudeMarkedEdges(MeshExtrudeEdgesAdjuster &adjuster, int numSegments, bool useExistingTextures, bool bMarkExtrudedEdges);

	int fillMarkedEdges();

	int riftMarkedEdges(MeshRiftEdgesAdjuster &adjuster, MEdgeRiftStyle style, int numSegments, bool fill, bool pinch);


	void slideMarkedEdges(MeshSlideEdgesAdjuster &adjuster, int slideVersion);


	void connectMarkedEdges(const MPickList &cutPickList, const MPickList &terminalVertexPickList, bool unmarkOriginalEdges);
	bool connectMarkedEdgesGetRingSegments(int seedEdgeIndex, Array<Segment3> &ringSegments);
	void connectMarkedEdgesGetTerminalVertices(const MPickList &terminalVertexPickList, Array<int> &terminalVertexIndices);

	boost::python::tuple py_connectMarkedEdgesGetRingSegments(int seedEdgeIndex);
	boost::python::list py_connectMarkedEdgesGetTerminalVertices(const MPickList &terminalVertexPickList);

	void nConnectMarkedEdges(int numSegments);


	void splitEdges(const MPickList &pickList);
	void nSplitMarkedEdges(int numSegments);

	void cut(const MCutTargetList &cutTargets, MeshCutState *cutState);
	bool isValidCutTarget(const MCutTarget &target, const MeshCutState &cutState);

	void nCut(int numSegments, const MCutTargetList &cutTargets, MeshNCutState *cutState);
	bool isValidNCutTarget(const MCutTarget &target, const MeshNCutState &cutState);

	void bandsaw(const MBandsawTargetList &targetList);
	void nBandsaw(const MNBandsawTargetList &targetList);
	bool bandsawGetRingSegments(int seedEdgeIndex, int seedFaceIndex, Array<Segment3> &ringSegments, bool stopAtMarkedEdges);

	boost::python::tuple py_bandsawGetRingSegments(int seedEdgeIndex, int seedFaceIndex, bool stopAtMarkedEdges);


	int bridge(const PolyBlend *blend, int segments);

	int weldMarkedEdges(const PolyBlend *blend, MWeldStyle style);

	int collapseMarkedEdges(bool lockMarkedVertices);

	int dissolveMarkedEdges(MEdgeDissolveStyle dissolveStyle, bool dissolveSimpleValenceTwoVertices);

	int spinMarkedEdges(int turns);

	int rewireEdges(const MRewireEdgeTargetList &rewireTargets);
	bool isValidRewireEdgeTarget(int edgeToRewireIndex, int sourceVertexIndex, int targetVertexIndex);
	bool canEdgeBeRewired(int edgeToRewireIndex);

	void extractMarkedEdgesAsPolylines(Array<Polyline> &polylines);

	void setMarkedEdgeSharpness(double sharpness, MSharpenFunction sharpenFunction);
	void saveMarkedEdgeSharpness(MeshSharpnessBackup &sharpnessBackup);
	void restoreMarkedEdgeSharpness(MeshSharpnessBackup &sharpnessBackup);

	void setMarkedEdgeNormalSharpness(bool sharpness);



	//face ops
	void insetMarkedFaces(MeshInsetFacesAdjuster &adjuster, MInsetFaceStyle style, bool flowThruMarkedEdges,
						bool markBoundaryEdges, bool markInsetEdges);
	void bevelMarkedFaces(MeshBevelFacesAdjuster &adjuster, MInsetFaceStyle style, bool flowThruMarkedEdges,
						bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges);

	void insetMarkedFacesSingular(MeshInsetFacesAdjuster &adjuster, MInsetFaceStyle style,
						bool markBoundaryEdges, bool markInsetEdges);
	void bevelMarkedFacesSingular(MeshBevelFacesAdjuster &adjuster, MInsetFaceStyle style,
						bool markBoundaryEdges, bool markInsetEdges, bool nsharpBoundaryEdges, bool nsharpInsetEdges);

	void expandMarkedFaces(MeshExpandFacesAdjuster &adjuster, bool markBoundaryEdges, bool markExpansionEdges, bool nsharpBoundaryEdges, bool nsharpExpansionEdges);

	void insetMarkedFacesToPoints(MeshVertexAdjustment &vertexAdjusts, int segments, bool markBoundaryEdges, bool nsharpBoundaryEdges);

	void extrudeMarkedFaces(MeshExtrudeFacesAdjuster &adjuster, MExtrudeFaceDirection extrudeDirection, int numSegments,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);
	void extrudeMarkedFacesSingular(MeshExtrudeFacesAdjuster &adjuster, int numSegments,
								bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);

	int pathExtrudeMarkedFaces(const Polyline &extrusionPath, MPathExtrudeFaceMode mode, bool correctiveScaling, PolylinePathOrientation orientation, double pathRotation,
							bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);

	void vectorExtrudeMarkedFaces(MeshVectorExtrudeFacesAdjuster &adjuster, int numSegments,
								bool markBoundaryEdges, bool markExtrudedEdges, bool nsharpBoundaryEdges, bool nsharpExtrudedEdges);

	void duplicateMarkedFaces(MeshVectorExtrudeFacesAdjuster &adjuster,bool markOriginalFaces, bool markDuplicateFaces);

	void roundMarkedFaces();

	int tunnel(const PolyBlend *blend, int segments);

	int weldMarkedFaces(const PolyBlend *blend, MWeldStyle style);

	int collapseMarkedFaces();

	int dissolveMarkedFaces(bool bMarkMergedFaces);

	void removeMarkedFaces();



	void drawQuadsWithState(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip, MeshDrawQuadsState *state);
	void drawQuads(const MDrawQuadsPointList &points, bool bCloseRows, bool bCloseColumns, int numColumns, bool bFlip);

	void drawFaceWithState(const MDrawFacePointList &points, bool bFlip, MeshDrawFaceState *state);
	void drawFace(const MDrawFacePointList &points, bool bFlip);



	//mark ops
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

	void markVertices_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred);
	void markEdges_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf);
	void markFaces_region(const Region3d *region, const BackfaceCull *faceCull, MarkPredicate pred, bool containsAllOf);

	void markVertices_pick(const MPickList &pick);
	void markEdges_pick(const MPickList &pick);
	void markEdgeRings_pick(const MPickList &pick);
	void markEdgeLoops_pick(const MPickList &pick);
	void markEdgeBoundaries_pick(const MPickList &pick);
	void markEdgeLoopPath_pick(const MPickList &pick);
	void markEdgeRingPath_pick(const MPickList &pick);
	void markFaces_pick(const MPickList &pick);

	void markEdgeRing(bool stopAtMarkedFaces, bool traverseFacesMultipleTimes);
	void markEdgeLoop(bool stopAtMarkedVertices, bool traverseVerticesMultipleTimes);
	void markEdgeBoundary();

	void growMarkedVertices();
	void shrinkMarkedVertices();
	void growMarkedEdges();
	void shrinkMarkedEdges();
	void growMarkedFaces();
	void shrinkMarkedFaces();


	//transformation ops
	void initVertexTransformation(MeshTransformation &meshXform, bool perGroup);
	void initEdgeTransformation(MeshTransformation &meshXform, bool perGroup);
	void initFaceTransformation(MeshTransformation &meshXform, bool perGroup);


	void transformVertices(const Array<int> &vertexIndices, const Matrix4 &xform);
	void transformMarkedVertices(const Matrix4 &xform);
	void saveVertexPositions(const Array<int> &vertexIndices);
	void restoreVertexPositions(const Array<int> &vertexIndices);
	void saveMarkedVertexPositions();
	void restoreMarkedVertexPositions();

	void transformEdgeMarkedVertices(const Matrix4 &xform);
	void saveEdgeMarkedVertexPositions();
	void restoreEdgeMarkedVertexPositions();

	void transformFaceMarkedVertices(const Matrix4 &xform);
	void saveFaceMarkedVertexPositions();
	void restoreFaceMarkedVertexPositions();

	void transformMarkedComponents(const Matrix4 &xform);
	void saveMarkedComponentPositions();
	void restoreMarkedComponentPositions();


	void tweakComponents(const MTweakComponentList &tweaks, bool bRestoreSelection);

	void surfaceTweakComponents(const MSurfaceTweakComponentList &tweaks, bool bRestoreSelection);


	void surfaceTweakAdjustMarkedVertices(MeshSurfaceTweakAdjuster &adjuster, const Vector3 &viewVector);
	void surfaceTweakAdjustMarkedEdges(MeshSurfaceTweakAdjuster &adjuster, const Vector3 &viewVector);
	void surfaceTweakAdjustMarkedFaces(MeshSurfaceTweakAdjuster &adjuster, const Vector3 &viewVector);
	void surfaceTweakNormalAdjustMarkedVertices(MeshSurfaceTweakNormalAdjuster &adjuster);
	void surfaceTweakNormalAdjustMarkedEdges(MeshSurfaceTweakNormalAdjuster &adjuster);
	void surfaceTweakNormalAdjustMarkedFaces(MeshSurfaceTweakNormalAdjuster &adjuster);


	void repositionVertices(const MVertexRepositionList &targets);
	void setVertexPosition(int vertexIndex, const Point3 &position);


	//proportional ops
	void proportionalAdjusterInit(const Point3 &brushPosition, const Brush &brush, MeshProportionalAdjuster &adjuster);

	void proportionalMove(const Point3 &brushPosition, const Brush &brush, const Vector3 &translation, MeshProportionalAdjuster *adjuster=NULL);
	void proportionalMoveNormal(const Point3 &brushPosition, const Brush &brush, double distance, MeshProportionalAdjuster *adjuster=NULL);
	void proportionalScale(const Point3 &brushPosition, const Brush &brush, double scaleFactor, MeshProportionalAdjuster *adjuster=NULL);
	void proportionalRotate(const Point3 &brushPosition, const Brush &brush, const Vector3 &rotationAxis, double rotation, MeshProportionalAdjuster *adjuster=NULL);
	void proportionalRelax(const Point3 &brushPosition, const Brush &brush,double relaxAmount, MeshProportionalAdjuster *adjuster=NULL);



	//move-normal ops
	void moveVerticesNormal(MeshMoveNormalAdjuster &adjuster);
	void moveEdgesNormal(MeshMoveNormalAdjuster &adjuster);
	void moveFacesNormal(MeshMoveNormalAdjuster &adjuster);


	//flatten ops
	void flattenVertices(const Vector3 &axis, bool perRegion);
	void flattenVerticesNormal(bool perRegion);
	void flattenEdges(const Vector3 &axis, bool perRegion);
	void flattenEdgesNormal(bool perRegion);
	void flattenEdgeRuns();
	void straightenEdgeRuns();
	void flattenFaces(const Vector3 &axis, bool perRegion);
	void flattenFacesNormal(bool perRegion);


	//material by marked region
	void setMaterial(int mat);
	void transformMaterial(const Matrix4 &xform, bool fixMarkedVertices);
	void transformMaterialSave(bool fixMarkedVertices, MeshMaterialTransformBackup &b);
	void normaliseMaterialCoords(bool perGroup);
	void transformMaterialRestore(const MeshMaterialTransformBackup &b);


	//material uv mapping
	void uvMap(UVMapper *mapper);

	void uvmapMarkedEdges(bool syncAtMarkedVertices, bool invert, const Point2f &uvStart, const Point2f &uvEnd);



	void getPoints(Array<Point3> &points) const;

	Point3 getVertexPosition(int index) const;
	bool isVertexMarked(int index) const;

	Point2f getVertexTexturePoint(int vertexIndex, int textureIndex) const;
	int getNumVertexTextures(int vertexIndex) const;

	Vector3f getVertexNormal(int vertexIndex, int normalIndex) const;
	int getNumVertexNormals(int vertexIndex) const;

	Segment3 getEdgeSegment(int index) const;
	bool isEdgeMarked(int index) const;
	bool isEdgeBoundary(int index) const;
	int getEdgeVertexAIndex(int edgeIndex) const;
	int getEdgeVertexBIndex(int edgeIndex) const;
	int getEdgeOppositeVertexIndex(int edgeIndex, int vertexIndex) const;
	int getEdgeFaceAIndex(int edgeIndex) const;
	int getEdgeFaceBIndex(int edgeIndex) const;
	int getEdgeOppositeFaceIndex(int edgeIndex, int faceIndex) const;

	int getFaceSize(int faceIndex) const;
	void getFaceVertex(int faceIndex, int faceVertexIndex, int &vertexIndex, int &textureIndex, int &normalIndex) const;
	int getNumTrianglesInFace(int index);
	IndexTriangle getFaceIndexTriangle(int faceIndex, int triIndex);
	Triangle3 getFaceTriangle(int faceIndex, int triIndex);
	bool isFaceMarked(int index) const;
	int getFaceMaterialID(int index) const;
	Point3 getFaceCentroid(int index) const;

	boost::python::tuple py_getFaceVertex(int faceIndex, int faceVertexIndex);
	boost::python::list py_getFaceIndexTriangles(int index);
	boost::python::list py_getFaceTriangles(int index);




	BBox3 getMarkedVerticesBBox() const;
	BBox3 getMarkedVerticesBBox(const Matrix4 &matrix) const;
	BBox3 getMarkedVerticesBBox(const Transformation *x) const;
	Point3 getMarkedVerticesCentre() const;

	BBox3 py_getMarkedVerticesBBox() const;
	Point3 py_getMarkedVerticesCentre() const;


	BBox3 getMarkedEdgesBBox() const;
	BBox3 getMarkedEdgesBBox(const Matrix4 &matrix) const;
	BBox3 getMarkedEdgesBBox(const Transformation *x) const;
	Point3 getMarkedEdgesCentre() const;

	BBox3 py_getMarkedEdgesBBox() const;
	Point3 py_getMarkedEdgesCentre() const;


	BBox3 getMarkedFacesBBox() const;
	BBox3 getMarkedFacesBBox(const Matrix4 &matrix) const;
	BBox3 getMarkedFacesBBox(const Transformation *x) const;
	Point3 getMarkedFacesCentre() const;

	BBox3 py_getMarkedFacesBBox() const;
	Point3 py_getMarkedFacesCentre() const;


	Vector3 computeMarkedFacesWeightedNormal() const;
	Vector3 computeWeightedNormal() const;


private:
	//misc ops
	void convertEdgeRunToPolyline(MEdgeRun &run, Polyline &poly) const;



	friend class MeshDrawQuadsState;
	friend class MeshDrawFaceState;
};




/*********************************************************************************
												MeshModifier
 *********************************************************************************/

class GS_DllExport MeshModifier
{
private:
	GSProductMesh *mesh;

public:
	MeshModifier();

	void finalise();

protected:
	void lock();
	void unlock();

private:
	void setMesh(GSProductMesh *m);

	friend class GSProductMesh;
};



/*********************************************************************************
											MeshVertexAdjustment
 *********************************************************************************/

class GS_DllExport MeshVertexAdjustment : public MeshModifier
{
private:
	MVertexAdjustList *adjust;

public:
	MeshVertexAdjustment();
	~MeshVertexAdjustment();

	void setParameters(double alpha, double beta);

	friend class GSProductMesh;
};



/*********************************************************************************
										MeshVertexVectorAdjustment
 *********************************************************************************/

class GS_DllExport MeshVertexVectorAdjustment : public MeshModifier
{
private:
	MVertexVectorAdjustList *adjust;

public:
	MeshVertexVectorAdjustment();
	~MeshVertexVectorAdjustment();

	void setParameters(const Vector3 &v);

	friend class GSProductMesh;
};



/*********************************************************************************
										MeshVertexSlideAdjustment
 *********************************************************************************/

class GS_DllExport MeshVertexSlideAdjustment : public MeshModifier
{
private:
	MVertexSlideAdjustList *adjust;

public:
	MeshVertexSlideAdjustment();
	~MeshVertexSlideAdjustment();

	void setSlideFactor(double slide);

	friend class GSProductMesh;
};



/*********************************************************************************
										MeshVertexSurfaceTweakAdjustment
 *********************************************************************************/

class GS_DllExport MeshVertexSurfaceTweakAdjustment : public MeshModifier
{
private:
	MVertexSurfaceTweakAdjustList *adjust;

public:
	MeshVertexSurfaceTweakAdjustment();
	~MeshVertexSurfaceTweakAdjustment();

	void tweak(const Vector3 &displacement);

	void debugDraw();

	friend class GSProductMesh;
};



/*********************************************************************************
											MeshAttribAdjustment
 *********************************************************************************/

class GS_DllExport MeshAttribAdjustment : public MeshModifier
{
private:
	MVertexAttribAdjustList *adjust;

public:
	MeshAttribAdjustment();
	~MeshAttribAdjustment();

	void setParameters(float alpha);

	friend class GSProductMesh;
};



/*********************************************************************************
										MeshAttribVectorAdjustment
 *********************************************************************************/

class GS_DllExport MeshAttribVectorAdjustment : public MeshModifier
{
private:
	MVertexAttribVectorAdjustList *adjust;

public:
	MeshAttribVectorAdjustment();
	~MeshAttribVectorAdjustment();

	void setParameters(const Vector2f &v);

	friend class GSProductMesh;
};



/*********************************************************************************
										MeshAttribSlideAdjustment
 *********************************************************************************/

class GS_DllExport MeshAttribSlideAdjustment : public MeshModifier
{
private:
	MVertexAttribSlideAdjustList *adjust;

public:
	MeshAttribSlideAdjustment();
	~MeshAttribSlideAdjustment();

	void setSlideFactor(double slide);

	friend class GSProductMesh;
};



/*********************************************************************************
											MeshTransformation
 *********************************************************************************/

class GS_DllExport MeshTransformation : public MeshModifier
{
private:
	MTransformationTarget *target;

public:
	MeshTransformation();
	~MeshTransformation();

	const Point3 &getCentre() const;

	void translate(const Vector3 &translation);
	void centredTransform(const Matrix4 &xform);

	friend class GSProductMesh;
};



/*********************************************************************************
									MeshMaterialTransformBackup
 *********************************************************************************/

class GS_DllExport MeshMaterialTransformBackup
{
private:
	bool fixMarkedVertices;
	MMaterialTransformBackup *backup;

public:
	MeshMaterialTransformBackup();
	~MeshMaterialTransformBackup();

	friend class GSProductMesh;
};





/*********************************************************************************
									MeshSharpnessBackup
 *********************************************************************************/

class GS_DllExport MeshSharpnessBackup
{
private:
	Array<float> sharpnessBackup;

public:
	MeshSharpnessBackup();
	~MeshSharpnessBackup();

	friend class GSProductMesh;
};





/*********************************************************************************
					MeshSurfaceTweakAdjuster
 *********************************************************************************/

class GS_DllExport MeshSurfaceTweakAdjuster
{
private:
	MeshVertexSurfaceTweakAdjustment vertexAdjust;


public:
	MeshSurfaceTweakAdjuster();


	void tweak(const Vector3 &displacement);


	void debugDraw();


	friend class GSProductMesh;
};




/*********************************************************************************
					MeshSurfaceTweakNormalAdjuster
 *********************************************************************************/

class GS_DllExport MeshSurfaceTweakNormalAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;


public:
	MeshSurfaceTweakNormalAdjuster();


	void tweak(double displacement);


	friend class GSProductMesh;
};




/*********************************************************************************
					MeshProportionalAdjuster
 *********************************************************************************/

class GS_DllExport MeshProportionalAdjuster : public MeshModifier
{
private:
	MProportionalAdjuster *proportionalAdjuster;


public:
	MeshProportionalAdjuster();
	~MeshProportionalAdjuster();


	void save();
	void restore();
	void move(const Vector3 &translation);
	void moveNormal(double distance);
	void scale(const Point3 &centre, double scaleFactor);
	void rotate(const Point3 &centre, const Vector3 &rotationAxis, double rotation);
	void relax(double relaxAmount);


	friend class GSProductMesh;
};




/*********************************************************************************
					MeshMoveNormalAdjuster
 *********************************************************************************/

class GS_DllExport MeshMoveNormalAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;


public:
	MeshMoveNormalAdjuster();


	void setParameters(double distance);


	friend class GSProductMesh;
};




/*********************************************************************************
					MeshExpandVerticesAdjuster
 *********************************************************************************/

class GS_DllExport MeshExpandVerticesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshExpandVerticesAdjuster();


	void setParameters(double expansion, double extrusion);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshExpandVerticesInMarkedFacesAdjuster
 *********************************************************************************/

class GS_DllExport MeshExpandVerticesInMarkedFacesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshExpandVerticesInMarkedFacesAdjuster();


	void setParameters(double expansion);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshBevelVerticesAdjuster
 *********************************************************************************/

class GS_DllExport MeshBevelVerticesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshBevelVerticesAdjuster();


	void setParameters(double bevel);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshExpandEdgesAdjuster
 *********************************************************************************/

class GS_DllExport MeshExpandEdgesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshExpandEdgesAdjuster();


	void setParameters(double expansion, double extrusion);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshBevelEdgesAdjuster
 *********************************************************************************/

class GS_DllExport MeshBevelEdgesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshBevelEdgesAdjuster();


	void setParameters(double bevel);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshExtrudeEdgesAdjuster
 *********************************************************************************/

class GS_DllExport MeshExtrudeEdgesAdjuster
{
private:
	MeshVertexVectorAdjustment vertexAdjust;
	MeshAttribVectorAdjustment attribAdjust;


public:
	MeshExtrudeEdgesAdjuster();


	void setParameters(const Vector3 &extrude, const Vector2 &texExtrude);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshRiftEdgesAdjuster
 *********************************************************************************/

class GS_DllExport MeshRiftEdgesAdjuster
{
private:
	MeshVertexVectorAdjustment vertexAdjust;


public:
	MeshRiftEdgesAdjuster();


	void setParameters(const Vector3 &rift);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshSlideEdgesAdjuster
 *********************************************************************************/

class GS_DllExport MeshSlideEdgesAdjuster
{
private:
	MeshVertexSlideAdjustment vertexAdjust;
	MeshAttribSlideAdjustment attribAdjust;


public:
	MeshSlideEdgesAdjuster();


	void setParameters(double slideFactor);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshInsetFacesAdjuster
 *********************************************************************************/

class GS_DllExport MeshInsetFacesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshInsetFacesAdjuster();


	void setParameters(double inset);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshBevelFacesAdjuster
 *********************************************************************************/

class GS_DllExport MeshBevelFacesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshBevelFacesAdjuster();


	void setParameters(double inset, double extrusion);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshExpandFacesAdjuster
 *********************************************************************************/

class GS_DllExport MeshExpandFacesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;
	MeshAttribAdjustment attribAdjust;


public:
	MeshExpandFacesAdjuster();


	void setParameters(double expansion, double extrusion);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshExtrudeFacesAdjuster
 *********************************************************************************/

class GS_DllExport MeshExtrudeFacesAdjuster
{
private:
	MeshVertexAdjustment vertexAdjust;


public:
	MeshExtrudeFacesAdjuster();


	void setParameters(double extrusion);


	friend class GSProductMesh;
};



/*********************************************************************************
					MeshVectorExtrudeFacesAdjuster
 *********************************************************************************/

class GS_DllExport MeshVectorExtrudeFacesAdjuster
{
private:
	MeshVertexVectorAdjustment vertexAdjust;


public:
	MeshVectorExtrudeFacesAdjuster();


	void setParameters(const Vector3 &extrusion);


	friend class GSProductMesh;
};




/*********************************************************************************
						MeshCutState
 *********************************************************************************/

struct MeshCutState
{
	int lastSplitVertexIndex;
	Point3 lastSplitVertexPosition;
	int currentFaceIndex;
};





/*********************************************************************************
						MeshNCutState
 *********************************************************************************/

class GS_DllExport MeshNCutState
{
private:
	Array<int> lastSplitVertexIndices;
	Array<Point3> lastSplitVertexPositions;


public:
	MeshNCutState();


	int size();
	int getLastSplitVertexIndex(int index);
	const Point3 & getLastSplitVertexPosition(int index);


	friend class GSProductMesh;
};




/*********************************************************************************
						MeshDrawQuadsState
 *********************************************************************************/

class GS_DllExport MeshDrawQuadsState
{
private:
	MDrawQuadsState *state;
	GSProductMesh *mesh;


public:
	MeshDrawQuadsState();
	~MeshDrawQuadsState();


	int getColumn() const;
	int getRow() const;
	int getNumColumns() const;
	int getNumRows() const;
	bool isRowDirectionForward() const;
	bool areRowsClosed() const;
	bool areColumnsClosed() const;
	bool getQuadMeshVertexPosition(int column, int row, Point3 &pos) const;


	boost::python::tuple py_getQuadMeshVertexPosition(int column, int row) const;


	bool checkVertex(int vertexIndex);


private:
	void init(GSProductMesh *mesh);


	friend class GSProductMesh;
};





/*********************************************************************************
						MeshDrawFaceState
 *********************************************************************************/

class GS_DllExport MeshDrawFaceState
{
private:
	MDrawFaceState *state;
	GSProductMesh *mesh;


public:
	MeshDrawFaceState();
	~MeshDrawFaceState();


	int getNumVertices() const;
	const Point3 & getFaceVertexPosition(int n) const;


	MDrawFaceVertexCheckResult checkVertex(int vertexIndex);


private:
	void init(GSProductMesh *mesh);


	friend class GSProductMesh;
};



#endif
