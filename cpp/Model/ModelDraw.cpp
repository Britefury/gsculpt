//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MODELDRAW_CPP__
#define MODELDRAW_CPP__

#include <Model/ModelDraw.h>




ModelDrawColourPalette::ModelDrawColourPalette()
{
	unmarkedVertexColour = Colour3f( 0.5f, 0.3f, 1.0f );
	unmarkedVertexPointSize = 5.0;
	markedVertexColour = Colour3f( 1.0f, 0.95f, 0.5f );
	markedVertexPointSize = 5.0;
	reflectedVertexColour = Colour3f( 0.6f, 0.6f, 0.6f );
	reflectedVertexPointSize = 5.0;


	polylineVertexSpecialHighlightPointSize = 11.0;
	polylineNormalSharpHighlightColour = Colour3f( 0.45f, 0.0f, 0.0f );
	polylineTextureBreakHighlightColour = Colour3f( 0.0f, 0.0f, 0.45f );


	unmarkedEdgeColour = Colour3f( 1.0f, 0.15f, 0.0f );
	unmarkedEdgeLineWidth = 1.0;
	markedEdgeColour = Colour3f( 0.0f, 0.7f, 0.4f );
	markedEdgeLineWidth = 3.0;
	backgroundEdgeColour = Colour3f( 0.525f, 0.525f, 0.525f );
	backgroundEdgeLineWidth = 1.0;
	transparentBackgroundEdgeColour = Colour3f( 0.525f * 0.25f, 0.525f * 0.25f, 0.525f * 0.25f );
	transparentBackgroundEdgeLineWidth = 1.0;
	reflectedEdgeColour = Colour3f( 0.4f, 0.8f, 1.0f );
	reflectedEdgeLineWidth = 1.0;
	liveSubdEdgeColour = Colour3f( 0.43f, 0.52f, 0.47f );
	liveSubdEdgeLineWidth = 1.0;
	uvEdgeColour = Colour3f( 1.0, 1.0, 1.0 );
	uvEdgeAlpha = 1.0f;


	splineEdgeColour = Colour3f( 0.698f, 0.486f, 0.8f );
	splineEdgeLineWidth = 1.0;
	backgroundSplineEdgeColour = Colour3f( 0.612f, 0.502f, 0.667f );
	backgroundSplineEdgeLineWidth = 1.0;
	reflectedSplineEdgeColour = Colour3f( 0.667f, 0.557f, 1.0f );
	reflectedSplineEdgeLineWidth = 1.0;


	surfaceDiffuseColour = Colour3f( 1.0f, 1.0f, 1.0f );
	surfaceSpecularColour = Colour3f( 0.0f, 0.0f, 0.0f );
	transparentBackgroundSurfaceDiffuseColour = Colour3f( 0.835f * 0.3f, 0.114f * 0.3f, 0.29f * 0.3f );
	transparentBackgroundSurfaceSpecularColour = Colour3f( 0.5f * 0.3f, 0.5f * 0.3f, 0.5f * 0.3f );
	transparentBackgroundSurfaceAlpha = 0.6f;
	reflectedSurfaceFilterColour = Colour3f( 0.82f, 0.94f, 1.0f );
	liveSubdSurfaceDiffuseColour = Colour3f( 0.7f, 0.7f, 0.7f );
	liveSubdSurfaceSpecularColour = Colour3f( 0.5f, 0.5f, 0.5f );


	markedFaceColour = Colour3f( 0.5f, 0.3f, 0.075f );
	markedFaceAlpha = 0.5f;

	overlayFactor = 0.35f;

	mirrorPlaneColour = Colour3f( 0.02f, 0.067f, 0.10f );
	mirrorPlaneAlpha = 0.7f;
}




static ModelDrawColourPalette _palette;






void setColourPalette(const ModelDrawColourPalette &palette)
{
	_palette = palette;
}



const Colour3f & getUnmarkedVertexColour()
{
	return _palette.unmarkedVertexColour;
}

double getUnmarkedVertexPointSize()
{
	return _palette.unmarkedVertexPointSize;
}

const Colour3f & getMarkedVertexColour()
{
	return _palette.markedVertexColour;
}

double getMarkedVertexPointSize()
{
	return _palette.markedVertexPointSize;
}

const Colour3f & getReflectedVertexColour()
{
	return _palette.reflectedVertexColour;
}

double getReflectedVertexPointSize()
{
	return _palette.reflectedVertexPointSize;
}



double getPolylineVertexSpecialHighlightPointSize()
{
	return _palette.polylineVertexSpecialHighlightPointSize;
}

const Colour3f & getPolylineNormalSharpHighlightColour()
{
	return _palette.polylineNormalSharpHighlightColour;
}

const Colour3f & getPolylineTextureBreakHighlightColour()
{
	return _palette.polylineTextureBreakHighlightColour;
}



const Colour3f & getUnmarkedEdgeColour()
{
	return _palette.unmarkedEdgeColour;
}

double getUnmarkedEdgeLineWidth()
{
	return _palette.unmarkedEdgeLineWidth;
}

const Colour3f & getMarkedEdgeColour()
{
	return _palette.markedEdgeColour;
}

double getMarkedEdgeLineWidth()
{
	return _palette.markedEdgeLineWidth;
}

const Colour3f & getBackgroundEdgeColour()
{
	return _palette.backgroundEdgeColour;
}

double getBackgroundEdgeLineWidth()
{
	return _palette.backgroundEdgeLineWidth;
}

const Colour3f & getTransparentBackgroundEdgeColour()
{
	return _palette.transparentBackgroundEdgeColour;
}

double getTransparentBackgroundEdgeLineWidth()
{
	return _palette.transparentBackgroundEdgeLineWidth;
}

const Colour3f & getReflectedEdgeColour()
{
	return _palette.reflectedEdgeColour;
}

double getReflectedEdgeLineWidth()
{
	return _palette.reflectedEdgeLineWidth;
}

const Colour3f & getLiveSubdEdgeColour()
{
	return _palette.liveSubdEdgeColour;
}

double getLiveSubdEdgeLineWidth()
{
	return _palette.liveSubdEdgeLineWidth;
}

const Colour3f & getUVEdgeColour()
{
	return _palette.uvEdgeColour;
}

float getUVEdgeAlpha()
{
	return _palette.uvEdgeAlpha;
}



const Colour3f & getSplineEdgeColour()
{
	return _palette.splineEdgeColour;
}

double getSplineEdgeLineWidth()
{
	return _palette.splineEdgeLineWidth;
}

const Colour3f & getBackgroundSplineEdgeColour()
{
	return _palette.backgroundSplineEdgeColour;
}

double getBackgroundSplineEdgeLineWidth()
{
	return _palette.backgroundSplineEdgeLineWidth;
}

const Colour3f & getReflectedSplineEdgeColour()
{
	return _palette.reflectedSplineEdgeColour;
}

double getReflectedSplineEdgeLineWidth()
{
	return _palette.reflectedSplineEdgeLineWidth;
}




const Colour3f & getSurfaceDiffuseColour()
{
	return _palette.surfaceDiffuseColour;
}

const Colour3f & getSurfaceSpecularColour()
{
	return _palette.surfaceSpecularColour;
}

const Colour3f & getTransparentBackgroundSurfaceDiffuseColour()
{
	return _palette.transparentBackgroundSurfaceDiffuseColour;
}

const Colour3f & getTransparentBackgroundSurfaceSpecularColour()
{
	return _palette.transparentBackgroundSurfaceSpecularColour;
}

float getTransparentBackgroundSurfaceAlpha()
{
	return _palette.transparentBackgroundSurfaceAlpha;
}

const Colour3f & getReflectedSurfaceFilterColour()
{
	return _palette.reflectedSurfaceFilterColour;
}

const Colour3f & getLiveSubdSurfaceDiffuseColour()
{
	return _palette.liveSubdSurfaceDiffuseColour;
}

const Colour3f & getLiveSubdSurfaceSpecularColour()
{
	return _palette.liveSubdSurfaceSpecularColour;
}



const Colour3f & getMarkedFaceColour()
{
	return _palette.markedFaceColour;
}

float getMarkedFaceAlpha()
{
	return _palette.markedFaceAlpha;
}


float getOverlayFactor()
{
	return _palette.overlayFactor;
}



const Colour3f & getMirrorPlaneColour()
{
	return _palette.mirrorPlaneColour;
}

float getMirrorPlaneAlpha()
{
	return _palette.mirrorPlaneAlpha;
}



#endif
