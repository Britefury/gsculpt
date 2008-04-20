//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MESHTYPES_H__
#define MESHTYPES_H__

#include <Util/Array.h>
#include <Util/ArraySet.h>

#include <Math/Point2.h>
#include <Math/Projection.h>

#include <Model/MPick.h>

#include <Mesh/MMesh/MPreserveNormalSharpness.h>


#define MMESH_NORMAL_TOLERENCE			0.707
#define MMESH_BEVEL_PERPENDICULAR		1.0e-6

class GS_DllExport MEdge;
class GS_DllExport MEdgeRun;
class GS_DllExport MVertex;
class GS_DllExport MVertexList;
class GS_DllExport MFace;
class GS_DllExport MMesh;
class GS_DllExport MVertexAttrib;
class GS_DllExport MVertexAdjust;
class GS_DllExport MVertexAdjustList;
class GS_DllExport MVertexVectorAdjust;
class GS_DllExport MVertexVectorAdjustList;
class GS_DllExport MVertexSlideAdjust;
class GS_DllExport MVertexSlideAdjustList;
class GS_DllExport MVertexSurfaceTweakAdjust;
class GS_DllExport MVertexSurfaceTweakAdjustList;
class GS_DllExport MVertexAttribAdjust;
class GS_DllExport MVertexAttribAdjustList;
class GS_DllExport MVertexAttribVectorAdjust;
class GS_DllExport MVertexAttribVectorAdjustList;
class GS_DllExport MVertexAttribSlideAdjust;
class GS_DllExport MVertexAttribSlideAdjustList;
class GS_DllExport MVertexNormal;
class GS_DllExport MTransformationTarget;
class GS_DllExport MRenderMesh;
class GS_DllExport MImportMesh;
class GS_DllExport MBBTree;
class GS_DllExport MDrawQuadsState;
class GS_DllExport MDrawFaceState;
class GS_DllExport MProportionalAdjuster;


class GS_DllExport MMeshException {};


typedef Array<MFace*> MFaceList;
typedef Array<MEdge*> MEdgeList;
typedef Array<MVertexAttrib*> MVertexAttribList;
typedef Array<MVertexAttrib> MMaterialTransformBackup;

typedef ArraySet<MVertex*> MVertexSet;
typedef ArraySet<MEdge*> MEdgeSet;
typedef ArraySet<MFace*> MFaceSet;



//**********************************
// MFindPolicy
//**********************************

enum MFindPolicy
{
	MFINDPOLICY_RETURNNULL,
	MFINDPOLICY_CREATE,
	MFINDPOLICY_FORCEDCREATE
};


//**********************************
// MExtrudeFaceDirection
//**********************************

enum MExtrudeFaceDirection
{
	MEXTRUDEFACEDIRECTION_EXPAND,
	MEXTRUDEFACEDIRECTION_PERGROUP
};




//**********************************
// MPathExtrudeFaceMode
//**********************************

enum MPathExtrudeFaceMode
{
	MPATHEXTRUDEFACEMODE_NORMAL,
	MPATHEXTRUDEFACEMODE_ALIGNPATHTOFACES
};




//**********************************
// MInsetFaceStyle
//**********************************

enum MInsetFaceStyle
{
	MINSETFACESTYLE_TOCENTRE,
	MINSETFACESTYLE_FROMEDGES
};




//**********************************
// MInsetExpandOperation
//**********************************

enum MInsetExpandOperation
{
	MINSETEXPANDOP_INSET,
	MINSETEXPANDOP_EXPAND
};




//**********************************
// MSharpenFunction
//**********************************

enum MSharpenFunction
{
	MSHARPENFUNC_SET,
	MSHARPENFUNC_ACCUMULATE
};




//**********************************
// MCollapseEdgeTo
//**********************************

enum MCollapseEdgeTo
{
	MCOLLAPSEEDGETO_A,
	MCOLLAPSEEDGETO_B,
	MCOLLAPSEEDGETO_MIDPOINT
};




//**********************************
// MWeldStyle
//**********************************

enum MWeldStyle
{
	MWELDSTYLE_A,
	MWELDSTYLE_B,
	MWELDSTYLE_MIDPOINT
};




//**********************************
// MEdgeDissolveStyle
//**********************************

enum MEdgeDissolveStyle
{
	MEDGEDISSOLVESTYLE_A,
	MEDGEDISSOLVESTYLE_B,
	MEDGEDISSOLVESTYLE_MIDPOINT_MAT_A,
	MEDGEDISSOLVESTYLE_MIDPOINT_MAT_B
};




//**********************************
// MEdgeRiftStyle
//**********************************

enum MEdgeRiftStyle
{
	MEDGERIFTSTYLE_A,
	MEDGERIFTSTYLE_B,
	MEDGERIFTSTYLE_CENTRE
};




//**********************************
// MCutTarget
//**********************************

class GS_DllExport MCutTarget
{
public:
	enum TargetType
	{
		VERTEX,
		EDGE,
		FACE
	};

	MPick pick;
	TargetType targetType;


	inline MCutTarget()
	{
	}

	inline MCutTarget(const MPick &pick, TargetType targetType)
					: pick( pick ), targetType( targetType )
	{
	}
};




//**********************************
// MCutTargetList
//**********************************

typedef Array<MCutTarget> MCutTargetList;




//**********************************
// MBandsawTarget
//**********************************

class GS_DllExport MBandsawTarget
{
public:
	MPick pick;
	bool stopAtMarkedEdges;


	MBandsawTarget()
	{
	}

	MBandsawTarget(const MPick &pick, bool stopAtMarkedEdges)
				: pick( pick ), stopAtMarkedEdges( stopAtMarkedEdges )
	{
	}
};




//**********************************
// MBandsawTargetList
//**********************************

typedef Array<MBandsawTarget> MBandsawTargetList;




//**********************************
// MNBandsawTarget
//**********************************

class GS_DllExport MNBandsawTarget
{
public:
	MPick pick;
	bool stopAtMarkedEdges;
	int numSegments;


	MNBandsawTarget()
	{
	}

	MNBandsawTarget(const MPick &pick, bool stopAtMarkedEdges, int numSegments)
				: pick( pick ), stopAtMarkedEdges( stopAtMarkedEdges ), numSegments( numSegments )
	{
	}
};




//**********************************
// MNBandsawTargetList
//**********************************

typedef Array<MNBandsawTarget> MNBandsawTargetList;




//**********************************
// MKnifeTarget
//**********************************

class GS_DllExport MKnifeTarget
{
public:
	enum Constraint
	{
		NOTHING,
		VERTEX,
		EDGE
	};

	MPick pick;
	Constraint constraint;


	inline MKnifeTarget() : constraint( NOTHING )
	{
	}

	inline MKnifeTarget(const MPick &pick, Constraint constraint)
					: pick( pick ), constraint( constraint )
	{
	}
};




//**********************************
// MWeldTarget
//**********************************

class GS_DllExport MWeldTarget
{
public:
	enum TargetType
	{
		VERTEX,
		EDGE_CLOSEST_POINT_TO_SOURCE,
		EDGE
	};

	MPick sourcePick, targetPick;
	TargetType targetType;


	inline MWeldTarget() : targetType( VERTEX )
	{
	}

	inline MWeldTarget(const MPick &sourcePick, const MPick &targetPick, TargetType targetType)
					: sourcePick( sourcePick ), targetPick( targetPick), targetType( targetType )
	{
	}
};




//**********************************
// MWeldTargetList
//**********************************

typedef Array<MWeldTarget> MWeldTargetList;





//**********************************
// MRewireEdgeTarget
//**********************************

class GS_DllExport MRewireEdgeTarget
{
public:
	MPick sourcePick, targetPick;


	inline MRewireEdgeTarget()
	{
	}

	inline MRewireEdgeTarget(const MPick &sourcePick, const MPick &targetPick)
						: sourcePick( sourcePick ), targetPick( targetPick)
	{
	}
};




//**********************************
// MRewireEdgeTargetList
//**********************************

typedef Array<MRewireEdgeTarget> MRewireEdgeTargetList;






//**********************************
// MTweakComponent
//**********************************

class GS_DllExport MTweakComponent
{
public:
	enum Target
	{
		VERTICES,
		EDGES,
		EDGERINGS,
		EDGELOOPS,
		FACES
	};

	MPick pick;
	Target target;
	bool clearSelectionFlag;
	Vector3 translation;


	inline MTweakComponent() : clearSelectionFlag( true )
	{
	}

	inline MTweakComponent(const MPick &pick, Target target, bool clearSelectionFlag, const Vector3 &translation)
				: pick( pick ), target( target ), clearSelectionFlag( clearSelectionFlag ), translation( translation )
	{
	}
};



//**********************************
// MTweakComponentList
//**********************************

typedef Array<MTweakComponent> MTweakComponentList;




//**********************************
// MSurfaceTweakComponent
//**********************************

class GS_DllExport MSurfaceTweakComponent
{
public:
	enum Target
	{
		VERTICES,
		EDGES,
		EDGERINGS,
		EDGELOOPS,
		FACES
	};

	enum Mode
	{
		SURFACE,
		NORMAL
	};

	MPick pick;
	Target target;
	Mode mode;
	bool clearSelectionFlag;
	Vector3 viewVector;
	Vector3 displacementAlongSurface;
	double displacementAlongNormal;


	inline MSurfaceTweakComponent() : clearSelectionFlag( true )
	{
	}

	inline MSurfaceTweakComponent(const MPick &pick, Target target, Mode mode, bool clearSelectionFlag, const Vector3 &viewVector,
								const Vector3 &displacementAlongSurface, double displacementAlongNormal)
							: pick( pick ), target( target ), mode( mode ), clearSelectionFlag( clearSelectionFlag ), viewVector( viewVector ),
								displacementAlongSurface( displacementAlongSurface ), displacementAlongNormal( displacementAlongNormal )
	{
	}
};



//**********************************
// MSurfaceTweakComponentList
//**********************************

typedef Array<MSurfaceTweakComponent> MSurfaceTweakComponentList;




//**********************************
// MVertexReposition
//**********************************

class GS_DllExport MVertexReposition
{
public:
	MPick pick;
	Point3 position;


	inline MVertexReposition()
	{
	}

	inline MVertexReposition(const MPick &pick, const Point3 &position)
						: pick( pick ), position( position )
	{
	}
};



//**********************************
// MVertexRepositionList
//**********************************

typedef Array<MVertexReposition> MVertexRepositionList;




//**********************************
// MDrawQuadsPoint
//**********************************

class GS_DllExport MDrawQuadsPoint
{
public:
	enum Target
	{
		NEWPOINT,
		EXISTINGVERTEX
	};

	enum Direction
	{
		FORWARD,
		PINGPONG
	};

	MPick pick;
	Point3 position;
	Target target;
	Direction direction;


	inline MDrawQuadsPoint()
	{
	}

	inline MDrawQuadsPoint(const MPick &pick, const Point3 &position, Target target, Direction direction)
					: pick( pick ), position( position ), target( target ), direction( direction )
	{
	}
};




//**********************************
// MDrawQuadsPointList
//**********************************

typedef Array<MDrawQuadsPoint> MDrawQuadsPointList;






//**********************************
// MDrawFaceVertexCheckResult
//**********************************

enum MDrawFaceVertexCheckResult
{
	MDRAWFACEVERTEXCHECKRESULT_GOOD,
	MDRAWFACEVERTEXCHECKRESULT_CLOSING_EDGE_INVALID,
	MDRAWFACEVERTEXCHECKRESULT_INVALID
};



//**********************************
// MDrawFacePoint
//**********************************

class GS_DllExport MDrawFacePoint
{
public:
	enum Target
	{
		NEWPOINT,
		EXISTINGVERTEX
	};

	MPick pick;
	Point3 position;
	Target target;


	inline MDrawFacePoint()
	{
	}

	inline MDrawFacePoint(const MPick &pick, const Point3 &position, Target target)
					: pick( pick ), position( position ), target( target )
	{
	}
};




//**********************************
// MDrawFacePointList
//**********************************

typedef Array<MDrawFacePoint> MDrawFacePointList;





#endif
