//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYGSPRODUCTMODEL_CPP__
#define PYGSPRODUCTMODEL_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Product/GSProduct.h>

#include <Model/GSProductModel.h>



void export_GSProductModel()
{
	class_<GSProductModel, bases<GSProduct> >( "GSProductModel", init<>() )
		.def( "_p_getVirtualMirrorPlane", &GSProductModel::getVirtualMirrorPlane, return_internal_reference<>() )
		.def( "_p_isVirtualMirrorEnabled", &GSProductModel::isVirtualMirrorEnabled )
		.def( "_p_setVirtualMirrorEnabled", &GSProductModel::setVirtualMirrorEnabled )
		.def( "preComputeFull", &GSProductModel::preComputeFull )
		.def( "transform", &GSProductModel::py_transformMatrix )
		.def( "transform", &GSProductModel::py_transformTransformation )
		.def( "transformUnlocked", &GSProductModel::py_transformUnlockedMatrix )
		.def( "transformUnlocked", &GSProductModel::py_transformUnlockedTransformation )
		.def( "transformRestore", &GSProductModel::transformRestore )
		.def( "getBBox", &GSProductModel::py_getBBox )
		.def( "getBBox", &GSProductModel::py_getBBoxMatrix )
		.def( "getBBox", &GSProductModel::py_getBBoxTransformation )
		.def( "getBBox", &GSProductModel::py_getBBoxMatrixAndTransformation )
		.def( "raytrace", &GSProductModel::py_raytrace )
		.def( "raytraceClipRay", &GSProductModel::py_raytraceClipRay )
		.def( "flip", &GSProductModel::flip )
		.def( "canMergeWith", &GSProductModel::canMergeWith )
		.def( "mergedWith", &GSProductModel::py_mergedWith, return_value_policy<manage_new_object>() );
}


#endif
