//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYQUADRATIC_CPP
#define PYQUADRATIC_CPP

#include <boost/python.hpp>
using namespace boost::python;


#include <Math/Quadratic.h>

void export_Quadratic()
{
	class_<QuadraticRootList>( "QuadraticRootList", init<>() )
		.def( init<double>() )
		.def( init<double, double>() )
		.def( init<const QuadraticRootList &>() )
		.def( "hasRoots", &QuadraticRootList::hasRoots )
		.def( "getMinimumRoot", &QuadraticRootList::getMinimumRoot )
		.def( "getMaximumRoot", &QuadraticRootList::getMaximumRoot );

	def( "evaluateQuadBezierPoint2", evaluateQuadBezierPoint2 );
	def( "solveQuadratic", solveQuadratic );
	def( "solveQuadraticBezier", solveQuadraticBezier );
}


#endif
