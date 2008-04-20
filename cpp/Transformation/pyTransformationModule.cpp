//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYTRANSFORMATIONMODULE_CPP__
#define PYTRANSFORMATIONMODULE_CPP__

#include <boost/python.hpp>
using namespace boost::python;



void export_Transformation();
void export_MatrixTransformation();
void export_BendTransformation();
void export_DentTransformation();
void export_ShrinkTransformation();
void export_TaperTransformation();
void export_LinearTwistTransformation();
void export_RadialTwistTransformation();
void export_Radius2Transformation();



BOOST_PYTHON_MODULE(Transformation)
{
	export_Transformation();
	export_MatrixTransformation();
	export_BendTransformation();
	export_DentTransformation();
	export_ShrinkTransformation();
	export_TaperTransformation();
	export_LinearTwistTransformation();
	export_RadialTwistTransformation();
	export_Radius2Transformation();
}



#endif
