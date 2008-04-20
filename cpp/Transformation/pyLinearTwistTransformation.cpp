//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYLINEARTWISTTRANSFORMATION_CPP__
#define PYLINEARTWISTTRANSFORMATION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Transformation/LinearTwistTransformation.h>


void export_LinearTwistTransformation()
{
	class_<LinearTwistTransformation, bases<Transformation> >( "LinearTwistTransformation", init<>() )
		.def( init<double, double, double, double, double, double>() );
}


#endif
