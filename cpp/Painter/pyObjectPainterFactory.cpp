//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYOBJECTPAINTERFACTORY_CPP__
#define PYOBJECTPAINTERFACTORY_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Painter/ObjectPainter.h>
#include <Painter/ObjectPainterFactory.h>


void export_ObjectPainterFactory()
{
	class_<ObjectPainterFactory>( "ObjectPainterFactory", no_init )
		.def( "createPainterForObject", &ObjectPainterFactory::createPainterForObject, return_internal_reference<>() )
		.def( "getPainterFactory", &ObjectPainterFactory::getPainterFactory, return_value_policy<reference_existing_object>() ).staticmethod( "getPainterFactory" );
}


#endif
