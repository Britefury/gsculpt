//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef GSPRODUCTMODELBLANK_CPP__
#define GSPRODUCTMODELBLANK_CPP__

#include <FileIO/fileio.h>

#include <Model/GSProductModelBlank.h>




/***********************************************************************************
												GSProductModel
 ***********************************************************************************/

RT_IMPLEMENT_TYPE( GSProductModelBlank, GSProduct );


GSProductModelBlank::GSProductModelBlank() : GSProduct( new ObjectRep() )
{
}

GSProductModelBlank::GSProductModelBlank(ObjectRep *rep) : GSProduct( rep )
{
}

GSProductModelBlank::GSProductModelBlank(const GSProductModelBlank &m) : GSProduct( m )
{
}


GSProductModelBlank::~GSProductModelBlank()
{
}




std::string GSProductModelBlank::getDescription() const
{
	return "Blank Model";
}



int GSProductModelBlank::estimateCost() const
{
	int superClassCost = GSProduct::estimateCost();

	return sizeof(GSProductModelBlank) + superClassCost - sizeof(GSProduct);
}



void GSProductModelBlank::read(SENode &node, ObjectRepTable *repTable)
{
	GSProduct::read( node[0], repTable );
}

void GSProductModelBlank::write(SENode &node, ObjectRepTable *repTable)
{
	GSProduct::write( node.addList(), repTable );
}




#endif
