//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef POLYBLEND_H__
#define POLYBLEND_H__

#include <Util/Array.h>

#include <PolyBlend/BlendingPolyline.h>




class GS_DllExport PolyBlendPoint
{
public:
	int p0Index, p1Index;


	inline PolyBlendPoint() : p0Index( 0 ), p1Index( 0 )
	{
	}

	inline PolyBlendPoint(int x, int y) : p0Index( x ), p1Index( y )
	{
	}
};


typedef Array<PolyBlendPoint> PolyBlendPointList;



/****************************************************************************
				PolyBlend
 Polyline blending class
 ****************************************************************************/

class GS_DllExport PolyBlend
{
private:
	double collapsePenalty, twistPenalty, inversionPenalty, areaWeighting;


public:
	inline PolyBlend(double collapsePenalty = 1.0, double twistPenalty = 1.0, double inversionPenalty = 2.0, double areaWeighting = 0.5)
					: collapsePenalty( collapsePenalty ), twistPenalty( twistPenalty ), inversionPenalty( inversionPenalty ), areaWeighting( areaWeighting )
	{
	}


	inline double getCollapsePenalty() const
	{
		return collapsePenalty;
	}

	inline double getTwistPenalty() const
	{
		return twistPenalty;
	}

	inline double getInversionPenalty() const
	{
		return inversionPenalty;
	}

	inline double getAreaWeighting() const
	{
		return areaWeighting;
	}



	double computeBlendPath(const BlendingPolyline &poly0, const BlendingPolyline &poly1, PolyBlendPointList &blend) const;
	static void lerpPolylines(const BlendingPolyline &poly0, const BlendingPolyline &poly1, const PolyBlendPointList &blendPath, double t, BlendingPolyline &result);
	static void generateTextureCoords(const BlendingPolyline &p0, const BlendingPolyline &p1, const PolyBlendPointList &blendPath, Array<double> &t0, Array<double> &t1);
};



#endif
