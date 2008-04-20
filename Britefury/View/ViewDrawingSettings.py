##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

from Britefury.Math.Math import Colour3f

from Britefury.Sheet.Sheet import *

from Britefury.FileIO.IOXml import *

from Britefury.Config import GSConfig

from Britefury.Model.Model import ModelDrawColourPalette, setColourPalette

from Britefury.Model import ModelDraw

from Britefury.WorkArea import Viewport



class ViewDrawingSettings (Sheet):
	backgroundColour = Field( Colour3f, Colour3f( 0.0, 0.0, 0.0 ), bWriteIfDefault=False )


	gridLevel1Colour = Field( Colour3f, Colour3f( 0.0, 0.8, 0.0 ), bWriteIfDefault=False )
	gridLevel2Colour = Field( Colour3f, Colour3f( 0.0, 0.4, 0.0 ), bWriteIfDefault=False )
	gridLevel3Colour = Field( Colour3f, Colour3f( 0.0, 0.2, 0.0 ), bWriteIfDefault=False )


	unmarkedVertexColour = Field( Colour3f, Colour3f( 0.5, 0.3, 1.0 ), bWriteIfDefault=False )
	unmarkedVertexPointSize = Field( float, 5.0, bWriteIfDefault=False )
	markedVertexColour = Field( Colour3f, Colour3f( 1.0, 0.95, 0.5 ), bWriteIfDefault=False )
	markedVertexPointSize = Field( float, 5.0, bWriteIfDefault=False )
	reflectedVertexColour = Field( Colour3f, Colour3f( 0.6, 0.6, 0.6 ), bWriteIfDefault=False )
	reflectedVertexPointSize = Field( float, 5.0, bWriteIfDefault=False )


	polylineVertexSpecialHighlightPointSize = Field( float, 11.0, bWriteIfDefault=False )
	polylineNormalSharpHighlightColour = Field( Colour3f, Colour3f( 0.45, 0.0, 0.0 ), bWriteIfDefault=False )
	polylineTextureBreakHighlightColour = Field( Colour3f, Colour3f( 0.0, 0.0, 0.45 ), bWriteIfDefault=False )


	unmarkedEdgeColour = Field( Colour3f, Colour3f( 1.0, 0.15, 0.0 ), bWriteIfDefault=False )
	unmarkedEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	markedEdgeColour = Field( Colour3f, Colour3f( 0.0, 0.7, 0.4 ), bWriteIfDefault=False )
	markedEdgeLineWidth = Field( float, 3.0, bWriteIfDefault=False )
	backgroundEdgeColour = Field( Colour3f, Colour3f( 0.525, 0.525, 0.525 ), bWriteIfDefault=False )
	backgroundEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	transparentBackgroundEdgeColour = Field( Colour3f, Colour3f( 0.525 * 0.25, 0.525 * 0.25, 0.525 * 0.25 ), bWriteIfDefault=False )
	transparentBackgroundEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	reflectedEdgeColour = Field( Colour3f, Colour3f( 0.4, 0.8, 1.0 ), bWriteIfDefault=False )
	reflectedEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	liveSubdEdgeColour = Field( Colour3f, Colour3f( 0.43, 0.52, 0.47 ), bWriteIfDefault=False )
	liveSubdEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	uvEdgeColour = Field( Colour3f, Colour3f( 1.0, 1.0, 1.0 ), bWriteIfDefault=False )
	uvEdgeAlpha = Field( float, 1.0, bWriteIfDefault=False )


	splineEdgeColour = Field( Colour3f, Colour3f( 0.698, 0.486, 0.8 ), bWriteIfDefault=False )
	splineEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	backgroundSplineEdgeColour = Field( Colour3f, Colour3f( 0.612, 0.502, 0.667 ), bWriteIfDefault=False )
	backgroundSplineEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )
	reflectedSplineEdgeColour = Field( Colour3f, Colour3f( 0.667, 0.557, 1.0 ), bWriteIfDefault=False )
	reflectedSplineEdgeLineWidth = Field( float, 1.0, bWriteIfDefault=False )


	surfaceDiffuseColour = Field( Colour3f, Colour3f( 1.0, 1.0, 1.0 ), bWriteIfDefault=False )
	surfaceSpecularColour = Field( Colour3f, Colour3f( 0.0, 0.0, 0.0 ), bWriteIfDefault=False )
	transparentBackgroundSurfaceDiffuseColour = Field( Colour3f, Colour3f( 0.835 * 0.3, 0.114 * 0.3, 0.29 * 0.3 ), bWriteIfDefault=False )
	transparentBackgroundSurfaceSpecularColour = Field( Colour3f, Colour3f( 0.5 * 0.3, 0.5 * 0.3, 0.5 * 0.3 ), bWriteIfDefault=False )
	transparentBackgroundSurfaceAlpha = Field( float, 0.6, bWriteIfDefault=False )
	reflectedSurfaceFilterColour = Field( Colour3f, Colour3f( 0.82, 0.94, 1.0 ), bWriteIfDefault=False )
	liveSubdSurfaceDiffuseColour = Field( Colour3f, Colour3f( 0.7, 0.7, 0.7 ), bWriteIfDefault=False )
	liveSubdSurfaceSpecularColour = Field( Colour3f, Colour3f( 0.5, 0.5, 0.5 ), bWriteIfDefault=False )


	markedFaceColour = Field( Colour3f, Colour3f( 0.5, 0.3, 0.075 ), bWriteIfDefault=False )
	markedFaceAlpha = Field( float, 0.5, bWriteIfDefault=False )

	overlayFactor = Field( float, 0.35, bWriteIfDefault=False )

	mirrorPlaneColour = Field( Colour3f, Colour3f( 0.02, 0.067, 0.10 ), bWriteIfDefault=False )
	mirrorPlaneAlpha = Field( float, 0.7, bWriteIfDefault=False )

	drawingPlaneColour = Field( Colour3f, Colour3f( 0.15, 0.0, 0.15 ), bWriteIfDefault=False )
	drawingPlaneAlpha = Field( float, 0.7, bWriteIfDefault=False )




	vertexPickHighlightPointSize = Field( float, 9.0, bWriteIfDefault=False )
	edgePickHighlightLineWidth = Field( float, 6.0, bWriteIfDefault=False )
	targetHighlightColour = Field( Colour3f, Colour3f( 0.0, 0.7, 0.0 ), bWriteIfDefault=False )
	markedTargetHighlightColour = Field( Colour3f, Colour3f( 0.7, 0.7, 0.0 ), bWriteIfDefault=False )
	invalidTargetHighlightColour = Field( Colour3f, Colour3f( 0.65, 0.0, 0.0 ), bWriteIfDefault=False )
	warningTargetHighlightColour = Field( Colour3f, Colour3f( 0.7, 0.35, 0.0 ), bWriteIfDefault=False )
	pathHighlightColour = Field( Colour3f, Colour3f( 0.25, 0.1, 1.0 ), bWriteIfDefault=False )

	vertexReticleColour = Field( Colour3f, Colour3f( 1.0, 0.0, 1.0 ), bWriteIfDefault=False )
	modification1HighlightColour = Field( Colour3f, Colour3f( 0.5, 0.0, 1.0 ), bWriteIfDefault=False )
	modification2HighlightColour = Field( Colour3f, Colour3f( 0.0, 0.4, 0.8 ), bWriteIfDefault=False )
	modificationHighlightAlpha = Field( float, 0.55, bWriteIfDefault=False )

	tweakConstraintWidgetAlpha = Field( float, 0.6, bWriteIfDefault=False )
	tweakConstraintWidgetSize = Field( float, 0.0375, bWriteIfDefault=False )
	tweakConstraintWidgetBrightness = Field( float, 0.5, bWriteIfDefault=False )






	def applySettings(self, view):
		palette = ModelDrawColourPalette()

		palette.unmarkedVertexColour = self.unmarkedVertexColour
		palette.unmarkedVertexPointSize = max( self.unmarkedVertexPointSize, 1.0 )
		palette.markedVertexColour = self.markedVertexColour
		palette.markedVertexPointSize = max( self.markedVertexPointSize, 1.0 )
		palette.reflectedVertexColour = self.reflectedVertexColour
		palette.reflectedVertexPointSize = max( self.reflectedVertexPointSize, 1.0 )


		palette.polylineVertexSpecialHighlightPointSize = self.polylineVertexSpecialHighlightPointSize
		palette.polylineNormalSharpHighlightColour = self.polylineNormalSharpHighlightColour
		palette.polylineTextureBreakHighlightColour = self.polylineTextureBreakHighlightColour


		palette.unmarkedEdgeColour = self.unmarkedEdgeColour
		palette.unmarkedEdgeLineWidth = max( self.unmarkedEdgeLineWidth, 1.0 )
		palette.markedEdgeColour = self.markedEdgeColour
		palette.markedEdgeLineWidth = max( self.markedEdgeLineWidth, 1.0 )
		palette.backgroundEdgeColour = self.backgroundEdgeColour
		palette.backgroundEdgeLineWidth = max( self.backgroundEdgeLineWidth, 1.0 )
		palette.transparentBackgroundEdgeColour = self.transparentBackgroundEdgeColour
		palette.transparentBackgroundEdgeLineWidth = max( self.transparentBackgroundEdgeLineWidth, 1.0 )
		palette.reflectedEdgeColour = self.reflectedEdgeColour
		palette.reflectedEdgeLineWidth = max( self.reflectedEdgeLineWidth, 1.0 )
		palette.liveSubdEdgeColour = self.liveSubdEdgeColour
		palette.liveSubdEdgeLineWidth = max( self.liveSubdEdgeLineWidth, 1.0 )
		palette.uvEdgeColour = self.uvEdgeColour
		palette.uvEdgeAlpha = min( max( self.uvEdgeAlpha, 0.0 ), 1.0 )


		palette.splineEdgeColour = self.splineEdgeColour
		palette.splineEdgeLineWidth = max( self.splineEdgeLineWidth, 1.0 )
		palette.backgroundSplineEdgeColour = self.backgroundSplineEdgeColour
		palette.backgroundSplineEdgeLineWidth = max( self.backgroundSplineEdgeLineWidth, 1.0 )
		palette.reflectedSplineEdgeColour = self.reflectedSplineEdgeColour
		palette.reflectedSplineEdgeLineWidth = max( self.reflectedSplineEdgeLineWidth, 1.0 )


		palette.surfaceDiffuseColour = self.surfaceDiffuseColour
		palette.surfaceSpecularColour = self.surfaceSpecularColour
		palette.transparentBackgroundSurfaceDiffuseColour = self.transparentBackgroundSurfaceDiffuseColour
		palette.transparentBackgroundSurfaceSpecularColour = self.transparentBackgroundSurfaceSpecularColour
		palette.transparentBackgroundSurfaceAlpha = min( max( self.transparentBackgroundSurfaceAlpha, 0.0 ), 1.0 )
		palette.reflectedSurfaceFilterColour = self.reflectedSurfaceFilterColour
		palette.liveSubdSurfaceDiffuseColour = self.liveSubdSurfaceDiffuseColour
		palette.liveSubdSurfaceSpecularColour = self.liveSubdSurfaceSpecularColour


		palette.markedFaceColour = self.markedFaceColour
		palette.markedFaceAlpha = min( max( self.markedFaceAlpha, 0.0 ), 1.0 )

		palette.overlayFactor = self.overlayFactor

		palette.mirrorPlaneColour = self.mirrorPlaneColour
		palette.mirrorPlaneAlpha = min( max( self.mirrorPlaneAlpha, 0.0 ), 1.0 )

		setColourPalette( palette )



		ModelDraw.VERTEX_PICKHIGHLIGHT_POINT_SIZE = max( self.vertexPickHighlightPointSize, 1.0 )
		ModelDraw.EDGE_PICKHIGHLIGHT_LINE_WIDTH = max( self.edgePickHighlightLineWidth, 1.0 )

		ModelDraw.OVERLAY_FACTOR = self.overlayFactor

		ModelDraw.VERTEX_RETICLE_COLOUR = self.vertexReticleColour

		ModelDraw.TARGET_HIGHLIGHT_COLOUR = self.targetHighlightColour
		ModelDraw.MARKED_TARGET_HIGHLIGHT_COLOUR = self.markedTargetHighlightColour
		ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR = self.invalidTargetHighlightColour
		ModelDraw.WARNING_TARGET_HIGHLIGHT_COLOUR = self.warningTargetHighlightColour
		ModelDraw.PATH_HIGHLIGHT_COLOUR = self.pathHighlightColour

		ModelDraw.TWEAK_CONSTRAINT_WIDGET_ALPHA = min( max( self.tweakConstraintWidgetAlpha, 0.0 ), 1.0 )
		ModelDraw.TWEAK_CONSTRAINT_WIDGET_SIZE = max( self.tweakConstraintWidgetSize, 0.00001 )
		ModelDraw.TWEAK_CONSTRAINT_WIDGET_BRIGHTNESS = self.tweakConstraintWidgetBrightness

		ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR = self.modification1HighlightColour
		ModelDraw.MODIFICATION2_HIGHLIGHT_COLOUR = self.modification2HighlightColour
		ModelDraw.MODIFICATION_HIGHLIGHT_ALPHA = min( max( self.modificationHighlightAlpha, 0.0 ), 1.0 )

		ModelDraw.DRAWING_PLANE_COLOUR = self.drawingPlaneColour
		ModelDraw.DRAWING_PLANE_ALPHA = min( max( self.drawingPlaneAlpha, 0.0 ), 1.0 )


		Viewport.CLEAR_COLOUR = self.backgroundColour


		view.gridColours = [ self.gridLevel3Colour, self.gridLevel2Colour, self.gridLevel1Colour ]




def readViewDrawingSettings(viewDrawingSettings, filename):
	if os.path.exists( filename ):
		f = open( filename, 'r' )
		if f is not None:
			doc = InputXmlDocument()
			doc.parseFile( f )
			contentNode = doc.getContentNode()
			if contentNode.isValid():
				contentNode  >>  viewDrawingSettings
	else:
		raise IOError, 'View drawing settings file \'%s\' does not exist'  %  ( filename, )



def writeViewDrawingSettings(viewDrawingSettings, filename):
	doc = OutputXmlDocument()
	doc.getContentNode()  <<  viewDrawingSettings
	f = open( filename, 'w' )
	if f is not None:
		doc.writeFile( f )
		f.close()




def readSystemViewDrawingSettings(viewDrawingSettings):
	try:
		readViewDrawingSettings( viewDrawingSettings, GSConfig.viewDrawingSettingsConfigPath )
	except IOError:
		print 'Could not read system view drawing settings file'



def writeSystemViewDrawingSettings(viewDrawingSettings):
	try:
		writeViewDrawingSettings( viewDrawingSettings, GSConfig.viewDrawingSettingsConfigPath )
	except IOError:
		print 'Could not write system view drawing settings file'

