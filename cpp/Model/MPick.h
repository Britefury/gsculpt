//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MPICK_H__
#define MPICK_H__

#include <Math/Matrix4.h>
#include <Math/Point2.h>
#include <Math/Projection.h>



//**********************************
// MPick
//**********************************

class GS_DllExport MPick
{
public:
	Point2 screenPos;
	Projection projection;
	bool backfaceCullingFlag, bAllowPickOutsideBounds, bSolidPick;


	inline MPick() : backfaceCullingFlag( false ), bAllowPickOutsideBounds( false ), bSolidPick( false )
	{
	}

	inline MPick(const Point2 &screenPos, const Projection &projection, bool backfaceCullingFlag, bool bAllowPickOutsideBounds, bool bSolidPick)
				: screenPos( screenPos ), projection( projection ), backfaceCullingFlag( backfaceCullingFlag ), bAllowPickOutsideBounds( bAllowPickOutsideBounds ),
				bSolidPick( bSolidPick)
	{
	}



	inline MPick preTransformed(const Matrix4 &m, const Matrix4 &mInv) const
	{
		return MPick( screenPos, projection.preTransformed( m, mInv ), backfaceCullingFlag, bAllowPickOutsideBounds, bSolidPick );
	}
};



//**********************************
// MPickList
//**********************************

typedef Array<MPick> MPickList;




#endif
