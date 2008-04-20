//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef VIEWSETTINGS_CPP__
#define VIEWSETTINGS_CPP__

#include <View/ViewSettings.h>


ViewSettings::ViewSettings()
					: bBackfaceCull( false ), bMultilayer( false ),
					bMeshLiveSubdivisionEnabled( false ), meshLiveSubdivisionIterations( 1 ), meshLiveSubdivisionMaxFaces( 81920 ),
					meshLiveSubdivisionNormalSharpness( MPRESERVENORMALSHARPNESS_IFSHARP ), meshLiveSubdivisionWireframeMode( MESHLIVESUBDWIRE_CONTROL )
{
}


#endif
