//##************************
//##* This program is free software; you can use it, redistribute it and/or modify it
//##* under the terms of the GNU General Public License version 2 as published by the
//##* Free Software Foundation. The full text of the GNU General Public License
//##* version 2 can be found in the file named 'COPYING' that accompanies this
//##* program. This source code is (C)copyright Geoffrey French 1999-2007.
//##************************
#ifndef MODELDRAW_H__
#define MODELDRAW_H__

#include <Math/Colour3f.h>





class GS_DllExport ModelDrawColourPalette
{
public:
	Colour3f unmarkedVertexColour;
	double unmarkedVertexPointSize;
	Colour3f markedVertexColour;
	double markedVertexPointSize;
	Colour3f reflectedVertexColour;
	double reflectedVertexPointSize;


	double polylineVertexSpecialHighlightPointSize;
	Colour3f polylineNormalSharpHighlightColour;
	Colour3f polylineTextureBreakHighlightColour;


	Colour3f unmarkedEdgeColour;
	double unmarkedEdgeLineWidth;
	Colour3f markedEdgeColour;
	double markedEdgeLineWidth;
	Colour3f backgroundEdgeColour;
	double backgroundEdgeLineWidth;
	Colour3f transparentBackgroundEdgeColour;
	double transparentBackgroundEdgeLineWidth;
	Colour3f reflectedEdgeColour;
	double reflectedEdgeLineWidth;
	Colour3f liveSubdEdgeColour;
	double liveSubdEdgeLineWidth;
	Colour3f uvEdgeColour;
	float uvEdgeAlpha;


	Colour3f splineEdgeColour;
	double splineEdgeLineWidth;
	Colour3f backgroundSplineEdgeColour;
	double backgroundSplineEdgeLineWidth;
	Colour3f reflectedSplineEdgeColour;
	double reflectedSplineEdgeLineWidth;


	Colour3f surfaceDiffuseColour;
	Colour3f surfaceSpecularColour;
	Colour3f transparentBackgroundSurfaceDiffuseColour;
	Colour3f transparentBackgroundSurfaceSpecularColour;
	float transparentBackgroundSurfaceAlpha;
	Colour3f reflectedSurfaceFilterColour;
	Colour3f liveSubdSurfaceDiffuseColour;
	Colour3f liveSubdSurfaceSpecularColour;


	Colour3f markedFaceColour;
	float markedFaceAlpha;

	float overlayFactor;

	Colour3f mirrorPlaneColour;
	float mirrorPlaneAlpha;



	ModelDrawColourPalette();
};




GS_DllExport void setColourPalette(const ModelDrawColourPalette &palette);
GS_DllExport ModelDrawColourPalette & getColourPalette();



GS_DllExport const Colour3f & getUnmarkedVertexColour();
GS_DllExport double getUnmarkedVertexPointSize();
GS_DllExport const Colour3f & getMarkedVertexColour();
GS_DllExport double getMarkedVertexPointSize();
GS_DllExport const Colour3f & getReflectedVertexColour();
GS_DllExport double getReflectedVertexPointSize();
GS_DllExport double getPolylineVertexSpecialHighlightPointSize();
GS_DllExport const Colour3f & getPolylineNormalSharpHighlightColour();
GS_DllExport const Colour3f & getPolylineTextureBreakHighlightColour();

GS_DllExport const Colour3f & getUnmarkedEdgeColour();
GS_DllExport double getUnmarkedEdgeLineWidth();
GS_DllExport const Colour3f & getMarkedEdgeColour();
GS_DllExport double getMarkedEdgeLineWidth();
GS_DllExport const Colour3f & getBackgroundEdgeColour();
GS_DllExport double getBackgroundEdgeLineWidth();
GS_DllExport const Colour3f & getTransparentBackgroundEdgeColour();
GS_DllExport double getTransparentBackgroundEdgeLineWidth();
GS_DllExport const Colour3f & getReflectedEdgeColour();
GS_DllExport double getReflectedEdgeLineWidth();
GS_DllExport const Colour3f & getLiveSubdEdgeColour();
GS_DllExport double getLiveSubdEdgeLineWidth();
GS_DllExport const Colour3f & getUVEdgeColour();
GS_DllExport float getUVEdgeAlpha();

GS_DllExport const Colour3f & getSplineEdgeColour();
GS_DllExport double getSplineEdgeLineWidth();
GS_DllExport const Colour3f & getBackgroundSplineEdgeColour();
GS_DllExport double getBackgroundSplineEdgeLineWidth();
GS_DllExport const Colour3f & getReflectedSplineEdgeColour();
GS_DllExport double getReflectedSplineEdgeLineWidth();

GS_DllExport const Colour3f & getSurfaceDiffuseColour();
GS_DllExport const Colour3f & getSurfaceSpecularColour();
GS_DllExport const Colour3f & getTransparentBackgroundSurfaceDiffuseColour();
GS_DllExport const Colour3f & getTransparentBackgroundSurfaceSpecularColour();
GS_DllExport float getTransparentBackgroundSurfaceAlpha();
GS_DllExport const Colour3f & getReflectedSurfaceFilterColour();
GS_DllExport const Colour3f & getLiveSubdSurfaceDiffuseColour();
GS_DllExport const Colour3f & getLiveSubdSurfaceSpecularColour();


GS_DllExport const Colour3f & getMarkedFaceColour();
GS_DllExport float getMarkedFaceAlpha();

GS_DllExport float getOverlayFactor();

GS_DllExport const Colour3f & getMirrorPlaneColour();
GS_DllExport float getMirrorPlaneAlpha();



GS_DllExport inline void computeOverlayColour(Colour3f &col)
{
	col *= getOverlayFactor();
}




#endif
