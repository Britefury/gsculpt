//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VIEWPORT3DSETTINGS_H__
#define VIEWPORT3DSETTINGS_H__

#include <Graphics/Camera.h>

#include <View/ViewportSettings.h>


class GS_DllExport Viewport3dSettings : public ViewportSettings
{
private:
	Camera *camera;


public:
	bool bForegroundVertices, bForegroundWireframe, bForegroundSolid, bForegroundSmooth, bForegroundMaterial, bForegroundMarkedFaces;
	bool bBackgroundWireframe, bBackgroundSolid, bBackgroundSmooth, bBackgroundMaterial, bBackgroundTransparent;

public:
	Viewport3dSettings(ViewSettings *viewSettings, Camera *camera);


	Camera * getCamera();
};


#endif
