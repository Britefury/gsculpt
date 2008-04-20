//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYRADIUS2TRANSFORMATION_CPP__
#define PYRADIUS2TRANSFORMATION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Transformation/Radius2Transformation.h>


void export_Radius2Transformation()
{
	class_<Radius2Transformation, bases<Transformation> >( "Radius2Transformation", init<>() );
}


#endif
