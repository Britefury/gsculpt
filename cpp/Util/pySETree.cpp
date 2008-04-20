//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYSETREE_CPP__
#define PYSETREE_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Util/SETree.h>



void export_SETree()
{
	class_<SENode, boost::noncopyable>( "SENode", no_init );

	class_<SEList, bases<SENode> >( "SEList", init<>() );

	class_<SETree>( "SETree", init<>() )
		.def( "getNode", &SETree::getNode, return_internal_reference<>() )
		.def( "read", &SETree::read )
		.def( "write", &SETree::write );
}


#endif
