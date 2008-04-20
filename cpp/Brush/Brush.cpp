//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef BRUSH_CPP__
#define BRUSH_CPP__

#include <Brush/Brush.h>




RT_IMPLEMENT_ABSTRACT_TYPE( Brush, RTObject );


Brush::Brush()
{
}

Brush::Brush(const Brush &b)
{
}

Brush::~Brush()
{
}



#endif // BRUSH_CPP__
