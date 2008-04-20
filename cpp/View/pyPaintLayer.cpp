//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYPAINTLAYER_CPP__
#define PYPAINTLAYER_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <View/PaintLayer.h>



void export_PaintLayer()
{
	enum_<PaintLayer>( "PaintLayer" )
		.value( "PAINTLAYER_FOREGROUND", PAINTLAYER_FOREGROUND )
		.value( "PAINTLAYER_TRANSPARENCY", PAINTLAYER_TRANSPARENCY )
		.value( "PAINTLAYER_WIREFRAME", PAINTLAYER_WIREFRAME )
		.value( "PAINTLAYER_OVERLAY_BLEND", PAINTLAYER_OVERLAY_BLEND )
		.value( "PAINTLAYER_OVERLAY_NODEPTH", PAINTLAYER_OVERLAY_NODEPTH )
		.value( "PAINTLAYER_OVERLAY_BLEND_NODEPTH", PAINTLAYER_OVERLAY_BLEND_NODEPTH );
}


#endif
