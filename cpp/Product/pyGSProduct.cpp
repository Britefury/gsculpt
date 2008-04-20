//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGSPRODUCT_CPP__
#define PYGSPRODUCT_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Product/GSProduct.h>



void export_GSProduct()
{
	class_<GSProduct, bases<RTObject> >( "GSProduct", init<>() )
		.def( "estimateCost", &GSProduct::estimateCost )
		.def( "readSingleGSProduct", &GSProduct::readSingleGSProduct, return_value_policy<manage_new_object>() ).staticmethod( "readSingleGSProduct" )
		.def( "writeSingleGSProduct", &GSProduct::writeSingleGSProduct ).staticmethod( "writeSingleGSProduct" )
		.def( "readSimple", &GSProduct::readSimple )
		.def( "writeSimple", &GSProduct::writeSimple );
}


#endif
