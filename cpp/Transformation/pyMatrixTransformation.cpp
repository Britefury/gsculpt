//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMATRIXTRANSFORMATION_CPP__
#define PYMATRIXTRANSFORMATION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Transformation/MatrixTransformation.h>


void export_MatrixTransformation()
{
	class_<MatrixTransformation, bases<Transformation> >( "MatrixTransformation", init<>() )
		.def( init<const Matrix4 &>() );
}


#endif
