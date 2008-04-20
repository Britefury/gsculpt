//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYOBJECTPAINTER_CPP__
#define PYOBJECTPAINTER_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Painter/ObjectPainter.h>


void export_ObjectPainter()
{
	class_<ObjectPainter, bases<RTObject> >( "ObjectPainter", init<>() )
		.def( "setProduct", &ObjectPainter::setProduct, with_custodian_and_ward<1, 2>() )
		.def( "getProduct", &ObjectPainter::getProduct, return_internal_reference<>() )
		.def( "paintObject3d", &ObjectPainter::paintObject3d )
		.def( "paintObjectUV", &ObjectPainter::paintObjectUV );
}


#endif
