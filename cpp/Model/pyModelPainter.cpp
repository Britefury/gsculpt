//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMODELPAINTER_CPP__
#define PYMODELPAINTER_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Model/ModelPainter.h>



void export_ModelPainter()
{
	class_<ModelPainter, bases<ObjectPainter> >( "ModelPainter", init<>() );
}


#endif
