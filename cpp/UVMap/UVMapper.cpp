//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef UVMAPPER_CPP__
#define UVMAPPER_CPP__

#include <UVMap/UVMapper.h>



/********************************************************************************
						UVMapper
 ********************************************************************************/

UVMapper::UVMapper(bool invertU, bool invertV)
					: invertU( invertU ), invertV( invertV )
{
}

UVMapper::~UVMapper()
{
}



void UVMapper::invert(Point2f &texCoord)
{
	if ( invertU )
	{
		texCoord.x = 1.0f - texCoord.x;
	}

	if ( invertV )
	{
		texCoord.y = 1.0f - texCoord.y;
	}
}




#endif
