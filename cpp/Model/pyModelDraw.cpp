//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef PYMODELDRAW_CPP__
#define PYMODELDRAW_CPP__

#include <boost/python.hpp>
using namespace boost::python;


#include <Model/ModelDraw.h>



void export_ModelDraw()
{
	class_<ModelDrawColourPalette>( "ModelDrawColourPalette", init<>() )
		.def_readwrite( "unmarkedVertexColour", &ModelDrawColourPalette::unmarkedVertexColour )
		.def_readwrite( "unmarkedVertexPointSize", &ModelDrawColourPalette::unmarkedVertexPointSize )
		.def_readwrite( "markedVertexColour", &ModelDrawColourPalette::markedVertexColour )
		.def_readwrite( "markedVertexPointSize", &ModelDrawColourPalette::markedVertexPointSize )
		.def_readwrite( "reflectedVertexColour", &ModelDrawColourPalette::reflectedVertexColour )
		.def_readwrite( "reflectedVertexPointSize", &ModelDrawColourPalette::reflectedVertexPointSize )


		.def_readwrite( "polylineVertexSpecialHighlightPointSize", &ModelDrawColourPalette::polylineVertexSpecialHighlightPointSize )
		.def_readwrite( "polylineNormalSharpHighlightColour", &ModelDrawColourPalette::polylineNormalSharpHighlightColour )
		.def_readwrite( "polylineTextureBreakHighlightColour", &ModelDrawColourPalette::polylineTextureBreakHighlightColour )


		.def_readwrite( "unmarkedEdgeColour", &ModelDrawColourPalette::unmarkedEdgeColour )
		.def_readwrite( "unmarkedEdgeLineWidth", &ModelDrawColourPalette::unmarkedEdgeLineWidth )
		.def_readwrite( "markedEdgeColour", &ModelDrawColourPalette::markedEdgeColour )
		.def_readwrite( "markedEdgeLineWidth", &ModelDrawColourPalette::markedEdgeLineWidth )
		.def_readwrite( "backgroundEdgeColour", &ModelDrawColourPalette::backgroundEdgeColour )
		.def_readwrite( "backgroundEdgeLineWidth", &ModelDrawColourPalette::backgroundEdgeLineWidth )
		.def_readwrite( "transparentBackgroundEdgeColour", &ModelDrawColourPalette::transparentBackgroundEdgeColour )
		.def_readwrite( "transparentBackgroundEdgeLineWidth", &ModelDrawColourPalette::transparentBackgroundEdgeLineWidth )
		.def_readwrite( "reflectedEdgeColour", &ModelDrawColourPalette::reflectedEdgeColour )
		.def_readwrite( "reflectedEdgeLineWidth", &ModelDrawColourPalette::reflectedEdgeLineWidth )
		.def_readwrite( "liveSubdEdgeColour", &ModelDrawColourPalette::liveSubdEdgeColour )
		.def_readwrite( "liveSubdEdgeLineWidth", &ModelDrawColourPalette::liveSubdEdgeLineWidth )
		.def_readwrite( "uvEdgeColour", &ModelDrawColourPalette::uvEdgeColour )
		.def_readwrite( "uvEdgeAlpha", &ModelDrawColourPalette::uvEdgeAlpha )


		.def_readwrite( "splineEdgeColour", &ModelDrawColourPalette::splineEdgeColour )
		.def_readwrite( "splineEdgeLineWidth", &ModelDrawColourPalette::splineEdgeLineWidth )
		.def_readwrite( "backgroundSplineEdgeColour", &ModelDrawColourPalette::backgroundSplineEdgeColour )
		.def_readwrite( "backgroundSplineEdgeLineWidth", &ModelDrawColourPalette::backgroundSplineEdgeLineWidth )
		.def_readwrite( "reflectedSplineEdgeColour", &ModelDrawColourPalette::reflectedSplineEdgeColour )
		.def_readwrite( "reflectedSplineEdgeLineWidth", &ModelDrawColourPalette::reflectedSplineEdgeLineWidth )


		.def_readwrite( "surfaceDiffuseColour", &ModelDrawColourPalette::surfaceDiffuseColour )
		.def_readwrite( "surfaceSpecularColour", &ModelDrawColourPalette::surfaceSpecularColour )
		.def_readwrite( "transparentBackgroundSurfaceDiffuseColour", &ModelDrawColourPalette::transparentBackgroundSurfaceDiffuseColour )
		.def_readwrite( "transparentBackgroundSurfaceSpecularColour", &ModelDrawColourPalette::transparentBackgroundSurfaceSpecularColour )
		.def_readwrite( "transparentBackgroundSurfaceAlpha", &ModelDrawColourPalette::transparentBackgroundSurfaceAlpha )
		.def_readwrite( "reflectedSurfaceFilterColour", &ModelDrawColourPalette::reflectedSurfaceFilterColour )
		.def_readwrite( "liveSubdSurfaceDiffuseColour", &ModelDrawColourPalette::liveSubdSurfaceDiffuseColour )
		.def_readwrite( "liveSubdSurfaceSpecularColour", &ModelDrawColourPalette::liveSubdSurfaceSpecularColour )


		.def_readwrite( "markedFaceColour", &ModelDrawColourPalette::markedFaceColour )
		.def_readwrite( "markedFaceAlpha", &ModelDrawColourPalette::markedFaceAlpha )

		.def_readwrite( "overlayFactor", &ModelDrawColourPalette::overlayFactor )

		.def_readwrite( "mirrorPlaneColour", &ModelDrawColourPalette::mirrorPlaneColour )
		.def_readwrite( "mirrorPlaneAlpha", &ModelDrawColourPalette::mirrorPlaneAlpha );

	def( "setColourPalette", &setColourPalette );
}


#endif
