//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VIEWPORT3DSETTINGS_CPP__
#define VIEWPORT3DSETTINGS_CPP__

#include <View/Viewport3dSettings.h>


Viewport3dSettings::Viewport3dSettings(ViewSettings *viewSettings, Camera *camera)
								: ViewportSettings( viewSettings ),
								camera( camera ),
								bForegroundVertices( true ), bForegroundWireframe( true ), bForegroundSolid( true ), bForegroundSmooth( false ),
								bForegroundMaterial( false ), bForegroundMarkedFaces( true ),
								bBackgroundWireframe( false ), bBackgroundSolid( true ), bBackgroundSmooth( false ), bBackgroundMaterial( false ),
								bBackgroundTransparent( false )
{
}


Camera * Viewport3dSettings::getCamera()
{
	return camera;
}



#endif
