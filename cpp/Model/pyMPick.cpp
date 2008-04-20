//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMPICK_CPP__
#define PYMPICK_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Model/MPick.h>




inline void py_pyMPickList_to_MPickList(boost::python::list pickList, MPickList &result)
{
	boost::python::object lenObj = pickList.attr( "__len__" )();
	boost::python::extract<int> lenExtract( lenObj );
	if ( lenExtract.check() )
	{
		int numPicks = lenExtract;
		result.reserve( numPicks );
		for (int i = 0; i < numPicks; i++)
		{
			boost::python::object pickObj = pickList[i];
			boost::python::extract<MPick&> pickExtract( pickObj );
			if ( pickExtract.check() )
			{
				result.push_back( pickExtract );
			}
		}
	}
}


void export_MPick()
{
	class_<MPick>( "MPick", init<Point2, Projection, bool, bool, bool>() );
	class_<MPickList>( "MPickList", init<>() );
	def( "pyMPickList_to_MPickList", py_pyMPickList_to_MPickList );
}


#endif
