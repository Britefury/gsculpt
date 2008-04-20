//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYTRANSFORMATION_CPP__
#define PYTRANSFORMATION_CPP__

#include <boost/python.hpp>
using namespace boost::python;



#include <Transformation/Transformation.h>


void export_Transformation()
{
	class_<Transformation>( "Transformation", init<>() )
		.def( "transformPoint", &Transformation::transformPoint )
		.def( "transformVector", &Transformation::transformVector )
		.def( "chainTo", &Transformation::chainTo )
		.def( "clone", &Transformation::clone, return_value_policy<manage_new_object>() );
}


#endif
