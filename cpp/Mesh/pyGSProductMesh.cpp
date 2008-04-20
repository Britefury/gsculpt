//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGSPRODUCTMESH_CPP__
#define PYGSPRODUCTMESH_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Mesh/MeshLiveSubdWireframeMode.h>

#include <Mesh/MMesh/MImportMesh.h>

#include <Mesh/GSProductMesh.h>




void py_pyMCutTargetList_to_MCutTargetList(boost::python::list targetList, MCutTargetList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MCutTarget&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMWeldTargetList_to_MWeldTargetList(boost::python::list targetList, MWeldTargetList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MWeldTarget&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMRewireEdgeTargetList_to_MRewireEdgeTargetList(boost::python::list targetList, MRewireEdgeTargetList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MRewireEdgeTarget&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMBandsawTargetList_to_MBandsawTargetList(boost::python::list targetList, MBandsawTargetList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MBandsawTarget&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMNBandsawTargetList_to_MNBandsawTargetList(boost::python::list targetList, MNBandsawTargetList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MNBandsawTarget&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMTweakComponentList_to_MTweakComponentList(boost::python::list targetList, MTweakComponentList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MTweakComponent&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMSurfaceTweakComponentList_to_MSurfaceTweakComponentList(boost::python::list targetList, MSurfaceTweakComponentList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MSurfaceTweakComponent&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMVertexRepositionList_to_MVertexRepositionList(boost::python::list targetList, MVertexRepositionList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MVertexReposition&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMDrawQuadsPointList_to_MDrawQuadsPointList(boost::python::list targetList, MDrawQuadsPointList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MDrawQuadsPoint&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}

void py_pyMDrawFacePointList_to_MDrawFacePointList(boost::python::list targetList, MDrawFacePointList &result)
{
	boost::python::object lenObj = targetList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numTargets = lenExtract;
		result.reserve( numTargets );
		for (int i = 0; i < numTargets; i++)
		{
			boost::python::object targetObj = targetList[i];
			boost::python::extract<MDrawFacePoint&> targetExtract( targetObj );
			if ( targetExtract.check() )
			{
				result.push_back( targetExtract );
			}
		}
	}
}




void export_GSProductMesh()
{
	class_<GSProductMesh, bases<GSProductModel> >( "GSProductMesh", init<>() )
		.def( "pickVertex", &GSProductMesh::py_pickVertex )
		.def( "pickVertexThruEdge", &GSProductMesh::py_pickVertexThruEdge )
		.def( "pickEdge", &GSProductMesh::py_pickEdge )
		.def( "pickFace", &GSProductMesh::py_pickFace )
		.def( "pickEdgeRing", &GSProductMesh::py_pickEdgeRing )
		.def( "pickEdgeLoop", &GSProductMesh::py_pickEdgeLoop )
		.def( "pickEdgeBoundary", &GSProductMesh::py_pickEdgeBoundary )
		.def( "transformMarkedComponents", &GSProductMesh::transformMarkedComponents )
		.def( "saveMarkedComponentPositions", &GSProductMesh::saveMarkedComponentPositions )
		.def( "restoreMarkedComponentPositions", &GSProductMesh::restoreMarkedComponentPositions )
		.def( "tweakComponents", &GSProductMesh::tweakComponents )
		.def( "surfaceTweakComponents", &GSProductMesh::surfaceTweakComponents )
		.def( "surfaceTweakAdjustMarkedVertices", &GSProductMesh::surfaceTweakAdjustMarkedVertices )
		.def( "surfaceTweakAdjustMarkedEdges", &GSProductMesh::surfaceTweakAdjustMarkedEdges )
		.def( "surfaceTweakAdjustMarkedFaces", &GSProductMesh::surfaceTweakAdjustMarkedFaces )
		.def( "surfaceTweakNormalAdjustMarkedVertices", &GSProductMesh::surfaceTweakNormalAdjustMarkedVertices )
		.def( "surfaceTweakNormalAdjustMarkedEdges", &GSProductMesh::surfaceTweakNormalAdjustMarkedEdges )
		.def( "surfaceTweakNormalAdjustMarkedFaces", &GSProductMesh::surfaceTweakNormalAdjustMarkedFaces )
		.def( "repositionVertices", &GSProductMesh::repositionVertices )
		.def( "_p_setVertexPosition", &GSProductMesh::setVertexPosition )
		.def( "proportionalAdjusterInit", &GSProductMesh::proportionalAdjusterInit, with_custodian_and_ward<4,1>() )
		.def( "proportionalMove", &GSProductMesh::proportionalMove )
		.def( "proportionalMoveNormal", &GSProductMesh::proportionalMoveNormal )
		.def( "proportionalScale", &GSProductMesh::proportionalScale )
		.def( "proportionalRotate", &GSProductMesh::proportionalRotate )
		.def( "proportionalRelax", &GSProductMesh::proportionalRelax )
		.def( "getNumVertices", &GSProductMesh::getNumVertices )
		.def( "getNumEdges", &GSProductMesh::getNumEdges )
		.def( "getNumFaces", &GSProductMesh::getNumFaces )
		.def( "getNumMarkedVertices", &GSProductMesh::getNumMarkedVertices )
		.def( "getNumMarkedEdges", &GSProductMesh::getNumMarkedEdges )
		.def( "getNumMarkedFaces", &GSProductMesh::getNumMarkedFaces )
		.def( "_p_getVertexPosition", &GSProductMesh::getVertexPosition )
		.def( "_p_isVertexMarked", &GSProductMesh::isVertexMarked )
		.def( "_p_getVertexTexturePoint", &GSProductMesh::getVertexTexturePoint )
		.def( "_p_getNumVertexTextures", &GSProductMesh::getNumVertexTextures )
		.def( "_p_getVertexNormal", &GSProductMesh::getVertexNormal )
		.def( "_p_getNumVertexNormals", &GSProductMesh::getNumVertexNormals )
		.def( "_p_getEdgeSegment", &GSProductMesh::getEdgeSegment )
		.def( "_p_isEdgeMarked", &GSProductMesh::isEdgeMarked )
		.def( "_p_isEdgeBoundary", &GSProductMesh::isEdgeBoundary )
		.def( "_p_getEdgeVertexAIndex", &GSProductMesh::getEdgeVertexAIndex )
		.def( "_p_getEdgeVertexBIndex", &GSProductMesh::getEdgeVertexBIndex )
		.def( "_p_getEdgeOppositeVertexIndex", &GSProductMesh::getEdgeOppositeVertexIndex )
		.def( "_p_getEdgeFaceAIndex", &GSProductMesh::getEdgeFaceAIndex )
		.def( "_p_getEdgeFaceBIndex", &GSProductMesh::getEdgeFaceBIndex )
		.def( "_p_getEdgeOppositeFaceIndex", &GSProductMesh::getEdgeOppositeFaceIndex )
		.def( "_p_getFaceSize", &GSProductMesh::getFaceSize )
		.def( "_p_getFaceCentroid", &GSProductMesh::getFaceCentroid )
		.def( "_p_getFaceVertex", &GSProductMesh::py_getFaceVertex )
		.def( "_p_getFaceIndexTriangles", &GSProductMesh::py_getFaceIndexTriangles )
		.def( "_p_getFaceTriangles", &GSProductMesh::py_getFaceTriangles )
		.def( "_p_isFaceMarked", &GSProductMesh::isFaceMarked )
		.def( "markVertices_save", &GSProductMesh::markVertices_save )
		.def( "markVertices_restore", &GSProductMesh::markVertices_restore )
		.def( "markEdges_save", &GSProductMesh::markEdges_save )
		.def( "markEdges_restore", &GSProductMesh::markEdges_restore )
		.def( "markFaces_save", &GSProductMesh::markFaces_save )
		.def( "markFaces_restore", &GSProductMesh::markFaces_restore )
		.def( "markVertices_pick", &GSProductMesh::markVertices_pick )
		.def( "markEdges_pick", &GSProductMesh::markEdges_pick )
		.def( "markEdgeRings_pick", &GSProductMesh::markEdgeRings_pick )
		.def( "markEdgeLoops_pick", &GSProductMesh::markEdgeLoops_pick )
		.def( "markEdgeBoundaries_pick", &GSProductMesh::markEdgeBoundaries_pick )
		.def( "markEdgeLoopPath_pick", &GSProductMesh::markEdgeLoopPath_pick )
		.def( "markEdgeRingPath_pick", &GSProductMesh::markEdgeRingPath_pick )
		.def( "markFaces_pick", &GSProductMesh::markFaces_pick )
		.def( "markVertices_region", &GSProductMesh::markVertices_region )
		.def( "markEdges_region", &GSProductMesh::markEdges_region )
		.def( "markFaces_region", &GSProductMesh::markFaces_region )
		.def( "markVertices_all", &GSProductMesh::markVertices_all )
		.def( "markEdges_all", &GSProductMesh::markEdges_all )
		.def( "markFaces_all", &GSProductMesh::markFaces_all )
		.def( "markVertices_edgeMarked", &GSProductMesh::markVertices_edgeMarked )
		.def( "markVertices_faceMarked", &GSProductMesh::markVertices_faceMarked )
		.def( "markVertices_faceMarkBoundary", &GSProductMesh::markVertices_faceMarkBoundary )
		.def( "markVertices_faceMarkInternal", &GSProductMesh::markVertices_faceMarkInternal )
		.def( "markVertices_boundary", &GSProductMesh::markVertices_boundary )
		.def( "markVertices_internal", &GSProductMesh::markVertices_internal )
		.def( "markEdges_vertexMarked", &GSProductMesh::markEdges_vertexMarked )
		.def( "markEdges_whollyVertexMarked", &GSProductMesh::markEdges_whollyVertexMarked )
		.def( "markEdges_faceMarked", &GSProductMesh::markEdges_faceMarked )
		.def( "markEdges_faceMarkBoundary", &GSProductMesh::markEdges_faceMarkBoundary )
		.def( "markEdges_faceMarkInternal", &GSProductMesh::markEdges_faceMarkInternal )
		.def( "markEdges_boundary", &GSProductMesh::markEdges_boundary )
		.def( "markEdges_internal", &GSProductMesh::markEdges_internal )
		.def( "markFaces_vertexMarked", &GSProductMesh::markFaces_vertexMarked )
		.def( "markFaces_whollyVertexMarked", &GSProductMesh::markFaces_whollyVertexMarked )
		.def( "markFaces_edgeMarked", &GSProductMesh::markFaces_edgeMarked )
		.def( "markFaces_whollyEdgeMarked", &GSProductMesh::markFaces_whollyEdgeMarked )
		.def( "markEdgeRing", &GSProductMesh::markEdgeRing )
		.def( "markEdgeLoop", &GSProductMesh::markEdgeLoop )
		.def( "markEdgeBoundary", &GSProductMesh::markEdgeBoundary )
		.def( "growMarkedVertices", &GSProductMesh::growMarkedVertices )
		.def( "shrinkMarkedVertices", &GSProductMesh::shrinkMarkedVertices )
		.def( "growMarkedEdges", &GSProductMesh::growMarkedEdges )
		.def( "shrinkMarkedEdges", &GSProductMesh::shrinkMarkedEdges )
		.def( "growMarkedFaces", &GSProductMesh::growMarkedFaces )
		.def( "shrinkMarkedFaces", &GSProductMesh::shrinkMarkedFaces )
		// info retrieval
		.def( "_p_getMarkedVerticesBBox", &GSProductMesh::py_getMarkedVerticesBBox )
		.def( "_p_getMarkedVerticesCentre", &GSProductMesh::py_getMarkedVerticesCentre )
		.def( "_p_getMarkedEdgesBBox", &GSProductMesh::py_getMarkedEdgesBBox )
		.def( "_p_getMarkedEdgesCentre", &GSProductMesh::py_getMarkedEdgesCentre )
		.def( "_p_getMarkedFacesBBox", &GSProductMesh::py_getMarkedFacesBBox )
		.def( "_p_getMarkedFacesCentre", &GSProductMesh::py_getMarkedFacesCentre )
		// vertex ops
		.def( "transformMarkedVertices", &GSProductMesh::transformMarkedVertices )
		.def( "saveMarkedVertexPositions", &GSProductMesh::saveMarkedVertexPositions )
		.def( "restoreMarkedVertexPositions", &GSProductMesh::restoreMarkedVertexPositions )
		.def( "initVertexTransformation", &GSProductMesh::initVertexTransformation )
		.def( "moveVerticesNormal", &GSProductMesh::moveVerticesNormal )
		.def( "flattenVertices", &GSProductMesh::flattenVertices )
		.def( "flattenVerticesNormal", &GSProductMesh::flattenVerticesNormal )
		.def( "expandMarkedVertices", &GSProductMesh::expandMarkedVertices )
		.def( "expandMarkedVerticesInMarkedFaces", &GSProductMesh::expandMarkedVerticesInMarkedFaces )
		.def( "bevelMarkedVertices", &GSProductMesh::bevelMarkedVertices )
		.def( "connectMarkedVertices", &GSProductMesh::connectMarkedVertices )
		.def( "weldMarkedVertices", &GSProductMesh::weldMarkedVertices )
		.def( "targetWeld", &GSProductMesh::targetWeld )
		.def( "_p_isValidWeldTargetVertex", &GSProductMesh::isValidWeldTargetVertex )
		.def( "_p_isValidWeldTargetEdge", &GSProductMesh::isValidWeldTargetEdge )
		.def( "dissolveMarkedVertices", &GSProductMesh::dissolveMarkedVertices )
		.def( "setMarkedVertexSharpness", &GSProductMesh::setMarkedVertexSharpness )
		.def( "saveMarkedVertexSharpness", &GSProductMesh::saveMarkedVertexSharpness )
		.def( "restoreMarkedVertexSharpness", &GSProductMesh::restoreMarkedVertexSharpness )
		.def( "setMarkedVertexNormalSharpness", &GSProductMesh::setMarkedVertexNormalSharpness )
		// edge ops
		.def( "transformEdgeMarkedVertices", &GSProductMesh::transformEdgeMarkedVertices )
		.def( "saveEdgeMarkedVertexPositions", &GSProductMesh::saveEdgeMarkedVertexPositions )
		.def( "restoreEdgeMarkedVertexPositions", &GSProductMesh::restoreEdgeMarkedVertexPositions )
		.def( "discoverEdgeRing", &GSProductMesh::py_discoverEdgeRing )
		.def( "discoverEdgeLoop", &GSProductMesh::py_discoverEdgeLoop )
		.def( "discoverEdgeBoundary", &GSProductMesh::py_discoverEdgeBoundary )
		.def( "discoverEdgeLoopPath", &GSProductMesh::py_discoverEdgeLoopPath )
		.def( "discoverEdgeRingPath", &GSProductMesh::py_discoverEdgeRingPath )
		.def( "initEdgeTransformation", &GSProductMesh::initEdgeTransformation )
		.def( "moveEdgesNormal", &GSProductMesh::moveEdgesNormal )
		.def( "flattenEdges", &GSProductMesh::flattenEdges )
		.def( "flattenEdgesNormal", &GSProductMesh::flattenEdgesNormal )
		.def( "flattenEdgeRuns", &GSProductMesh::flattenEdgeRuns )
		.def( "straightenEdgeRuns", &GSProductMesh::straightenEdgeRuns )
		.def( "expandMarkedEdges", &GSProductMesh::expandMarkedEdges )
		.def( "bevelMarkedEdges", &GSProductMesh::bevelMarkedEdges )
		.def( "extrudeMarkedEdges", &GSProductMesh::extrudeMarkedEdges )
		.def( "fillMarkedEdges", &GSProductMesh::fillMarkedEdges )
		.def( "riftMarkedEdges", &GSProductMesh::riftMarkedEdges )
		.def( "slideMarkedEdges", &GSProductMesh::slideMarkedEdges )
		.def( "connectMarkedEdges", &GSProductMesh::connectMarkedEdges )
		.def( "_p_connectMarkedEdgesGetRingSegments", &GSProductMesh::py_connectMarkedEdgesGetRingSegments )
		.def( "connectMarkedEdgesGetTerminalVertices", &GSProductMesh::py_connectMarkedEdgesGetTerminalVertices )
		.def( "nConnectMarkedEdges", &GSProductMesh::nConnectMarkedEdges )
		.def( "splitEdges", &GSProductMesh::splitEdges )
		.def( "nSplitMarkedEdges", &GSProductMesh::nSplitMarkedEdges )
		.def( "bandsaw", &GSProductMesh::bandsaw )
		.def( "nBandsaw", &GSProductMesh::nBandsaw )
		.def( "_p_bandsawGetRingSegments", &GSProductMesh::py_bandsawGetRingSegments )
		.def( "bridge", &GSProductMesh::bridge )
		.def( "weldMarkedEdges", &GSProductMesh::weldMarkedEdges )
		.def( "collapseMarkedEdges", &GSProductMesh::collapseMarkedEdges )
		.def( "dissolveMarkedEdges", &GSProductMesh::dissolveMarkedEdges )
		.def( "spinMarkedEdges", &GSProductMesh::spinMarkedEdges )
		.def( "rewireEdges", &GSProductMesh::rewireEdges )
		.def( "_p_isValidRewireEdgeTarget", &GSProductMesh::isValidRewireEdgeTarget )
		.def( "_p_canEdgeBeRewired", &GSProductMesh::canEdgeBeRewired )
		.def( "setMarkedEdgeSharpness", &GSProductMesh::setMarkedEdgeSharpness )
		.def( "saveMarkedEdgeSharpness", &GSProductMesh::saveMarkedEdgeSharpness )
		.def( "restoreMarkedEdgeSharpness", &GSProductMesh::restoreMarkedEdgeSharpness )
		.def( "setMarkedEdgeNormalSharpness", &GSProductMesh::setMarkedEdgeNormalSharpness )
		// face ops
		.def( "transformFaceMarkedVertices", &GSProductMesh::transformFaceMarkedVertices )
		.def( "saveFaceMarkedVertexPositions", &GSProductMesh::saveFaceMarkedVertexPositions )
		.def( "restoreFaceMarkedVertexPositions", &GSProductMesh::restoreFaceMarkedVertexPositions )
		.def( "initFaceTransformation", &GSProductMesh::initFaceTransformation )
		.def( "moveFacesNormal", &GSProductMesh::moveFacesNormal )
		.def( "flattenFaces", &GSProductMesh::flattenFaces )
		.def( "flattenFacesNormal", &GSProductMesh::flattenFacesNormal )
		.def( "insetMarkedFaces", &GSProductMesh::insetMarkedFaces )
		.def( "bevelMarkedFaces", &GSProductMesh::bevelMarkedFaces )
		.def( "insetMarkedFacesSingular", &GSProductMesh::insetMarkedFacesSingular )
		.def( "bevelMarkedFacesSingular", &GSProductMesh::bevelMarkedFacesSingular )
		.def( "expandMarkedFaces", &GSProductMesh::expandMarkedFaces )
		.def( "extrudeMarkedFaces", &GSProductMesh::extrudeMarkedFaces )
		.def( "extrudeMarkedFacesSingular", &GSProductMesh::extrudeMarkedFacesSingular )
		.def( "vectorExtrudeMarkedFaces", &GSProductMesh::vectorExtrudeMarkedFaces )
		.def( "duplicateMarkedFaces", &GSProductMesh::duplicateMarkedFaces )
		.def( "tunnel", &GSProductMesh::tunnel )
		.def( "weldMarkedFaces", &GSProductMesh::weldMarkedFaces )
		.def( "collapseMarkedFaces", &GSProductMesh::collapseMarkedFaces )
		.def( "dissolveMarkedFaces", &GSProductMesh::dissolveMarkedFaces )
		.def( "removeMarkedFaces", &GSProductMesh::removeMarkedFaces )
		// body ops
		.def( "mirror", &GSProductMesh::mirror )
		.def( "subdivide", &GSProductMesh::subdivide )
		.def( "knife", &GSProductMesh::knife )
		// other ops
		.def( "cut", &GSProductMesh::cut )
		.def( "isValidCutTarget", &GSProductMesh::isValidCutTarget )
		.def( "nCut", &GSProductMesh::nCut )
		.def( "isValidNCutTarget", &GSProductMesh::isValidNCutTarget )
		.def( "clear", &GSProductMesh::clear )
		.def( "importMesh", &GSProductMesh::importMesh )
		// draw ops
		.def( "drawQuadsWithState", &GSProductMesh::drawQuadsWithState )
		.def( "drawQuads", &GSProductMesh::drawQuads )
		.def( "drawFaceWithState", &GSProductMesh::drawFaceWithState )
		.def( "drawFace", &GSProductMesh::drawFace );


	class_<MeshTransformation>( "MeshTransformation", init<>() )
		.def( "translate", &MeshTransformation::translate )
		.def( "centredTransform", &MeshTransformation::centredTransform );


	class_<MeshSharpnessBackup>( "MeshSharpnessBackup", init<>() );


	class_<MeshSurfaceTweakAdjuster>( "MeshSurfaceTweakAdjuster", init<>() )
		.def( "tweak", &MeshSurfaceTweakAdjuster::tweak )
		.def( "debugDraw", &MeshSurfaceTweakAdjuster::debugDraw );

	class_<MeshSurfaceTweakNormalAdjuster>( "MeshSurfaceTweakNormalAdjuster", init<>() )
		.def( "tweak", &MeshSurfaceTweakNormalAdjuster::tweak );



	class_<MeshProportionalAdjuster>( "MeshProportionalAdjuster", init<>() )
		.def( "save", &MeshProportionalAdjuster::save )
		.def( "restore", &MeshProportionalAdjuster::restore )
		.def( "move", &MeshProportionalAdjuster::move )
		.def( "moveNormal", &MeshProportionalAdjuster::moveNormal )
		.def( "scale", &MeshProportionalAdjuster::scale )
		.def( "rotate", &MeshProportionalAdjuster::rotate )
		.def( "relax", &MeshProportionalAdjuster::relax );



	class_<MeshMoveNormalAdjuster>( "MeshMoveNormalAdjuster", init<>() )
		.def( "setParameters", &MeshMoveNormalAdjuster::setParameters );

	class_<MeshExpandVerticesAdjuster>( "MeshExpandVerticesAdjuster", init<>() )
		.def( "setParameters", &MeshExpandVerticesAdjuster::setParameters );

	class_<MeshExpandVerticesInMarkedFacesAdjuster>( "MeshExpandVerticesInMarkedFacesAdjuster", init<>() )
		.def( "setParameters", &MeshExpandVerticesInMarkedFacesAdjuster::setParameters );

	class_<MeshBevelVerticesAdjuster>( "MeshBevelVerticesAdjuster", init<>() )
		.def( "setParameters", &MeshBevelVerticesAdjuster::setParameters );


	class_<MeshExpandEdgesAdjuster>( "MeshExpandEdgesAdjuster", init<>() )
		.def( "setParameters", &MeshExpandEdgesAdjuster::setParameters );

	class_<MeshBevelEdgesAdjuster>( "MeshBevelEdgesAdjuster", init<>() )
		.def( "setParameters", &MeshBevelEdgesAdjuster::setParameters );

	class_<MeshExtrudeEdgesAdjuster>( "MeshExtrudeEdgesAdjuster", init<>() )
		.def( "setParameters", &MeshExtrudeEdgesAdjuster::setParameters );

	class_<MeshRiftEdgesAdjuster>( "MeshRiftEdgesAdjuster", init<>() )
		.def( "setParameters", &MeshRiftEdgesAdjuster::setParameters );

	class_<MeshSlideEdgesAdjuster>( "MeshSlideEdgesAdjuster", init<>() )
		.def( "setParameters", &MeshSlideEdgesAdjuster::setParameters );


	class_<MeshInsetFacesAdjuster>( "MeshInsetFacesAdjuster", init<>() )
		.def( "setParameters", &MeshInsetFacesAdjuster::setParameters );

	class_<MeshBevelFacesAdjuster>( "MeshBevelFacesAdjuster", init<>() )
		.def( "setParameters", &MeshBevelFacesAdjuster::setParameters );

	class_<MeshExpandFacesAdjuster>( "MeshExpandFacesAdjuster", init<>() )
		.def( "setParameters", &MeshExpandFacesAdjuster::setParameters );

	class_<MeshExtrudeFacesAdjuster>( "MeshExtrudeFacesAdjuster", init<>() )
		.def( "setParameters", &MeshExtrudeFacesAdjuster::setParameters );

	class_<MeshVectorExtrudeFacesAdjuster>( "MeshVectorExtrudeFacesAdjuster", init<>() )
		.def( "setParameters", &MeshVectorExtrudeFacesAdjuster::setParameters );




	class_<MCutTarget>( "CppMeshCutTarget", init<const MPick &, MCutTarget::TargetType>() );
	class_<MCutTargetList>( "CppMeshCutTargetList", init<>() );
	def( "pyMeshCutTargetList_to_CppMeshCutTargetList", py_pyMCutTargetList_to_MCutTargetList );

	class_<MWeldTarget>( "CppMeshWeldTarget", init<const MPick &, const MPick &, MWeldTarget::TargetType>() );
	class_<MWeldTargetList>( "CppMeshWeldTargetList", init<>() );
	def( "pyMeshWeldTargetList_to_CppMeshWeldTargetList", py_pyMWeldTargetList_to_MWeldTargetList );

	class_<MRewireEdgeTarget>( "CppMeshRewireEdgeTarget", init<const MPick &, const MPick &>() );
	class_<MRewireEdgeTargetList>( "CppMeshRewireEdgeTargetList", init<>() );
	def( "pyMeshRewireEdgeTargetList_to_CppMeshRewireEdgeTargetList", py_pyMRewireEdgeTargetList_to_MRewireEdgeTargetList );

	class_<MBandsawTarget>( "CppMeshBandsawTarget", init<const MPick &, bool>() );
	class_<MBandsawTargetList>( "CppMeshBandsawTargetList", init<>() );
	def( "pyMeshBandsawTargetList_to_CppMeshBandsawTargetList", py_pyMBandsawTargetList_to_MBandsawTargetList );

	class_<MNBandsawTarget>( "CppMeshNBandsawTarget", init<const MPick &, bool, int>() );
	class_<MNBandsawTargetList>( "CppMeshNBandsawTargetList", init<>() );
	def( "pyMeshNBandsawTargetList_to_CppMeshNBandsawTargetList", py_pyMNBandsawTargetList_to_MNBandsawTargetList );

	class_<MTweakComponent>( "CppMeshTweakComponent", init<const MPick &, MTweakComponent::Target, bool, const Vector3 &>() );
	class_<MTweakComponentList>( "CppMeshTweakComponentList", init<>() );
	def( "pyMeshTweakComponentList_to_CppMeshTweakComponentList", py_pyMTweakComponentList_to_MTweakComponentList );

	class_<MSurfaceTweakComponent>( "CppMeshSurfaceTweakComponent", init<const MPick &, MSurfaceTweakComponent::Target, MSurfaceTweakComponent::Mode,
																bool, const Vector3 &, const Vector3 &, double>() );
	class_<MSurfaceTweakComponentList>( "CppMeshSurfaceTweakComponentList", init<>() );
	def( "pyMeshSurfaceTweakComponentList_to_CppMeshSurfaceTweakComponentList", py_pyMSurfaceTweakComponentList_to_MSurfaceTweakComponentList );

	class_<MVertexReposition>( "CppMeshVertexReposition", init<const MPick &, const Point3 &>() );
	class_<MVertexRepositionList>( "CppMeshVertexRepositionList", init<>() );
	def( "pyMeshVertexRepositionList_to_CppMeshVertexRepositionList", py_pyMVertexRepositionList_to_MVertexRepositionList );

	class_<MDrawQuadsPoint>( "CppMeshDrawQuadsPoint", init<const MPick &, const Point3 &, MDrawQuadsPoint::Target, MDrawQuadsPoint::Direction>() );
	class_<MDrawQuadsPointList>( "CppMeshDrawQuadsPointList", init<>() );
	def( "pyMeshDrawQuadsPointList_to_MeshDrawQuadsPointList", py_pyMDrawQuadsPointList_to_MDrawQuadsPointList );

	class_<MDrawFacePoint>( "CppMeshDrawFacePoint", init<const MPick &, const Point3 &, MDrawFacePoint::Target>() );
	class_<MDrawFacePointList>( "CppMeshDrawFacePointList", init<>() );
	def( "pyMeshDrawFacePointList_to_MeshDrawFacePointList", py_pyMDrawFacePointList_to_MDrawFacePointList );

	class_<MKnifeTarget>( "CppMeshKnifeTarget", init<const MPick &, MKnifeTarget::Constraint>() );



	class_<MeshCutState>( "MeshCutState", init<>() )
		.def_readonly( "lastSplitVertexIndex", &MeshCutState::lastSplitVertexIndex )
		.def_readonly( "lastSplitVertexPosition", &MeshCutState::lastSplitVertexPosition )
		.def_readonly( "currentFaceIndex", &MeshCutState::currentFaceIndex );

	class_<MeshNCutState>( "MeshNCutState", init<>() )
		.def( "__len__", &MeshNCutState::size )
		.def( "_p_getLastSplitVertexIndex", &MeshNCutState::getLastSplitVertexIndex )
		.def( "_p_getLastSplitVertexPosition", &MeshNCutState::getLastSplitVertexPosition, return_internal_reference<>() );

	class_<MeshDrawQuadsState>( "MeshDrawQuadsState", init<>() )
		.def( "getColumn", &MeshDrawQuadsState::getColumn )
		.def( "getRow", &MeshDrawQuadsState::getRow )
		.def( "getNumColumns", &MeshDrawQuadsState::getNumColumns )
		.def( "getNumRows", &MeshDrawQuadsState::getNumRows )
		.def( "isRowDirectionForward", &MeshDrawQuadsState::isRowDirectionForward )
		.def( "areRowsClosed", &MeshDrawQuadsState::areRowsClosed )
		.def( "areColumnsClosed", &MeshDrawQuadsState::areColumnsClosed )
		.def( "_p_getQuadMeshVertexPosition", &MeshDrawQuadsState::py_getQuadMeshVertexPosition )
		.def( "checkVertex", &MeshDrawQuadsState::checkVertex );

	class_<MeshDrawFaceState>( "MeshDrawFaceState", init<>() )
		.def( "getNumVertices", &MeshDrawFaceState::getNumVertices )
		.def( "getFaceVertexPosition", &MeshDrawFaceState::getFaceVertexPosition, return_internal_reference<>() )
		.def( "checkVertex", &MeshDrawFaceState::checkVertex );




	enum_<MSharpenFunction>( "MeshSharpenFunction" )
		.value( "SET", MSHARPENFUNC_SET )
		.value( "ACCUMULATE", MSHARPENFUNC_ACCUMULATE );

	enum_<MEdgeRiftStyle>( "MeshRiftEdgesStyle" )
		.value( "A", MEDGERIFTSTYLE_A )
		.value( "B", MEDGERIFTSTYLE_B )
		.value( "CENTRE", MEDGERIFTSTYLE_CENTRE );

	enum_<MEdgeDissolveStyle>( "MeshDissolveEdgesStyle" )
		.value( "A", MEDGEDISSOLVESTYLE_A )
		.value( "B", MEDGEDISSOLVESTYLE_B )
		.value( "MIDPOINT_MAT_A", MEDGEDISSOLVESTYLE_MIDPOINT_MAT_A )
		.value( "MIDPOINT_MAT_B", MEDGEDISSOLVESTYLE_MIDPOINT_MAT_B );

	enum_<MWeldTarget::TargetType>( "MeshTargetWeldVerticesTargetType" )
		.value( "VERTEX", MWeldTarget::VERTEX )
		.value( "EDGE_CLOSEST_POINT_TO_SOURCE", MWeldTarget::EDGE_CLOSEST_POINT_TO_SOURCE )
		.value( "EDGE", MWeldTarget::EDGE );

	enum_<MInsetFaceStyle>( "MeshInsetFacesStyle" )
		.value( "TOCENTRE", MINSETFACESTYLE_TOCENTRE )
		.value( "FROMEDGES", MINSETFACESTYLE_FROMEDGES );

	enum_<MExtrudeFaceDirection>( "MeshExtrudeFacesDirection" )
		.value( "EXPAND", MEXTRUDEFACEDIRECTION_EXPAND )
		.value( "PERGROUP", MEXTRUDEFACEDIRECTION_PERGROUP );

	enum_<MWeldStyle>( "MeshWeldStyle" )
		.value( "A", MWELDSTYLE_A )
		.value( "B", MWELDSTYLE_B )
		.value( "MIDPOINT", MWELDSTYLE_MIDPOINT );

	enum_<MPreserveNormalSharpness>( "MeshPreserveNormalSharpness" )
		.value( "PRESERVE", MPRESERVENORMALSHARPNESS_PRESERVE )
		.value( "IF_SHARP", MPRESERVENORMALSHARPNESS_IFSHARP )
		.value( "SMOOTH", MPRESERVENORMALSHARPNESS_SMOOTH );

	enum_<MCutTarget::TargetType>( "MeshCutTargetType" )
		.value( "VERTEX", MCutTarget::VERTEX )
		.value( "EDGE", MCutTarget::EDGE )
		.value( "FACE", MCutTarget::FACE );

	enum_<MKnifeTarget::Constraint>( "MeshKnifeTargetConstraint" )
		.value( "NOTHING", MKnifeTarget::NOTHING )
		.value( "VERTEX", MKnifeTarget::VERTEX )
		.value( "EDGE", MKnifeTarget::EDGE );

	enum_<MTweakComponent::Target>( "MeshTweakComponentTarget" )
		.value( "VERTICES", MTweakComponent::VERTICES )
		.value( "EDGES", MTweakComponent::EDGES )
		.value( "EDGERINGS", MTweakComponent::EDGERINGS )
		.value( "EDGELOOPS", MTweakComponent::EDGELOOPS )
		.value( "FACES", MTweakComponent::FACES );

	enum_<MSurfaceTweakComponent::Target>( "MeshSurfaceTweakComponentTarget" )
		.value( "VERTICES", MSurfaceTweakComponent::VERTICES )
		.value( "EDGES", MSurfaceTweakComponent::EDGES )
		.value( "EDGERINGS", MSurfaceTweakComponent::EDGERINGS )
		.value( "EDGELOOPS", MSurfaceTweakComponent::EDGELOOPS )
		.value( "FACES", MSurfaceTweakComponent::FACES );

	enum_<MSurfaceTweakComponent::Mode>( "MeshSurfaceTweakComponentMode" )
		.value( "SURFACE", MSurfaceTweakComponent::SURFACE )
		.value( "NORMAL", MSurfaceTweakComponent::NORMAL );

	enum_<MeshLiveSubdWireframeMode>( "MeshLiveSubdWireframeMode" )
		.value( "NONE", MESHLIVESUBDWIRE_NONE )
		.value( "CONTROL", MESHLIVESUBDWIRE_CONTROL )
		.value( "FULL", MESHLIVESUBDWIRE_FULL );

	enum_<MDrawQuadsPoint::Target>( "MeshDrawQuadsPointTarget" )
		.value( "EXISTINGVERTEX", MDrawQuadsPoint::EXISTINGVERTEX )
		.value( "NEWPOINT", MDrawQuadsPoint::NEWPOINT );

	enum_<MDrawQuadsPoint::Direction>( "MeshDrawQuadsDirection" )
		.value( "FORWARD", MDrawQuadsPoint::FORWARD )
		.value( "PINGPONG", MDrawQuadsPoint::PINGPONG );

	enum_<MDrawFacePoint::Target>( "MeshDrawFacePointTarget" )
		.value( "EXISTINGVERTEX", MDrawFacePoint::EXISTINGVERTEX )
		.value( "NEWPOINT", MDrawFacePoint::NEWPOINT );

	enum_<MDrawFaceVertexCheckResult>( "MeshDrawFaceStateVertexCheckResult" )
		.value( "GOOD", MDRAWFACEVERTEXCHECKRESULT_GOOD )
		.value( "CLOSING_EDGE_INVALID", MDRAWFACEVERTEXCHECKRESULT_CLOSING_EDGE_INVALID )
		.value( "INVALID", MDRAWFACEVERTEXCHECKRESULT_INVALID );
}


#endif
