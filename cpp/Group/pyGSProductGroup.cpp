//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGSPRODUCTGROUP_CPP__
#define PYGSPRODUCTGROUP_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Model/GSProductModel.h>

#include <Group/GSProductGroup.h>



void export_GSProductGroup()
{
	class_<GSProductGroup, bases<GSProductModel> >( "GSProductGroup", init<>() )
		.def( "__len__", &GSProductGroup::getNumModels )
		.def( "_getModelClone", &GSProductGroup::getModelClone, return_value_policy<manage_new_object>() )
		.def( "clear", &GSProductGroup::clear )
		.def( "append", &GSProductGroup::py_append )
		.def( "extend", &GSProductGroup::py_extend )
		.def( "merged", &GSProductGroup::merged, return_value_policy<manage_new_object>() );
}


#endif
