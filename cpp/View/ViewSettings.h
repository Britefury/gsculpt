//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VIEWSETTINGS_H__
#define VIEWSETTINGS_H__

#include <Mesh/MMesh/MPreserveNormalSharpness.h>
#include <Mesh/MeshLiveSubdWireframeMode.h>


class GS_DllExport ViewSettings
{
public:
	bool bBackfaceCull, bMultilayer, bBackgroundTransparent;
	bool bMeshLiveSubdivisionEnabled;
	int meshLiveSubdivisionIterations, meshLiveSubdivisionMaxFaces;
	MPreserveNormalSharpness meshLiveSubdivisionNormalSharpness;
	MeshLiveSubdWireframeMode meshLiveSubdivisionWireframeMode;

public:
	ViewSettings();
};


#endif
