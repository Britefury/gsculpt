##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

import pygtk
pygtk.require( '2.0' )
import gtk

from copy import *

from Britefury.UI.FormLayout import FormLayoutFactory
from Britefury.UI.MessageDialog import messageDialog
from Britefury.UI.ConfirmOverwriteFileDialog import confirmOverwriteFileDialog

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Sheet.Sheet import *

from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.View.ViewDrawingSettings import ViewDrawingSettings, readViewDrawingSettings, writeViewDrawingSettings



class ViewDrawingSettingsEditor (SheetEditor):
	sheet = SheetRefField( ViewDrawingSettings )


	backgroundColourLabel = FieldLabelWithFn( sheet.backgroundColour, _( 'Background colour:' ) )
	backgroundColourButton = Colour3fFieldEditColourButton( sheet.backgroundColour )


	gridLevel1ColourLabel = FieldLabelWithFn( sheet.gridLevel1Colour, _( 'Grid level 1 colour:' ) )
	gridLevel1ColourButton = Colour3fFieldEditColourButton( sheet.gridLevel1Colour )
	gridLevel2ColourLabel = FieldLabelWithFn( sheet.gridLevel2Colour, _( 'Grid level 2 colour:' ) )
	gridLevel2ColourButton = Colour3fFieldEditColourButton( sheet.gridLevel2Colour )
	gridLevel3ColourLabel = FieldLabelWithFn( sheet.gridLevel3Colour, _( 'Grid level 3 colour:' ) )
	gridLevel3ColourButton = Colour3fFieldEditColourButton( sheet.gridLevel3Colour )


	unmarkedVertexColourLabel = FieldLabelWithFn( sheet.unmarkedVertexColour, _( 'Unmarked vertex colour:' ) )
	unmarkedVertexColourButton = Colour3fFieldEditColourButton( sheet.unmarkedVertexColour )
	unmarkedVertexPointSizeLabel = FieldLabelWithFn( sheet.unmarkedVertexPointSize, _( 'Unmarked vertex point size:' ) )
	unmarkedVertexPointSizeEntry = FloatFieldEditEntry( sheet.unmarkedVertexPointSize, 1.0, 1024.0, 1.0, 10.0, 2 )
	markedVertexColourLabel = FieldLabelWithFn( sheet.markedVertexColour, _( 'Marked vertex colour:' ) )
	markedVertexColourButton = Colour3fFieldEditColourButton( sheet.markedVertexColour )
	markedVertexPointSizeLabel = FieldLabelWithFn( sheet.markedVertexPointSize, _( 'Marked vertex point size:' ) )
	markedVertexPointSizeEntry = FloatFieldEditEntry( sheet.markedVertexPointSize, 1.0, 1024.0, 1.0, 10.0, 2 )
	reflectedVertexColourLabel = FieldLabelWithFn( sheet.reflectedVertexColour, _( 'Reflected vertex colour:' ) )
	reflectedVertexColourButton = Colour3fFieldEditColourButton( sheet.reflectedVertexColour )
	reflectedVertexPointSizeLabel = FieldLabelWithFn( sheet.reflectedVertexPointSize, _( 'Reflected vertex point size:' ) )
	reflectedVertexPointSizeEntry = FloatFieldEditEntry( sheet.reflectedVertexPointSize, 1.0, 1024.0, 1.0, 10.0, 2 )


	unmarkedEdgeColourLabel = FieldLabelWithFn( sheet.unmarkedEdgeColour, _( 'Unmarked edge colour:' ) )
	unmarkedEdgeColourButton = Colour3fFieldEditColourButton( sheet.unmarkedEdgeColour )
	unmarkedEdgeLineWidthLabel = FieldLabelWithFn( sheet.unmarkedEdgeLineWidth, _( 'Unmarked edge line width:' ) )
	unmarkedEdgeLineWidthEntry = FloatFieldEditEntry( sheet.unmarkedEdgeLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )
	markedEdgeColourLabel = FieldLabelWithFn( sheet.markedEdgeColour, _( 'Marked edge colour:' ) )
	markedEdgeColourButton = Colour3fFieldEditColourButton( sheet.markedEdgeColour )
	markedEdgeLineWidthLabel = FieldLabelWithFn( sheet.markedEdgeLineWidth, _( 'Marked edge line width:' ) )
	markedEdgeLineWidthEntry = FloatFieldEditEntry( sheet.markedEdgeLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )
	backgroundEdgeColourLabel = FieldLabelWithFn( sheet.backgroundEdgeColour, _( 'Background edge colour:' ) )
	backgroundEdgeColourButton = Colour3fFieldEditColourButton( sheet.backgroundEdgeColour )
	backgroundEdgeLineWidthLabel = FieldLabelWithFn( sheet.backgroundEdgeLineWidth, _( 'Background edge line width:' ) )
	backgroundEdgeLineWidthEntry = FloatFieldEditEntry( sheet.backgroundEdgeLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )
	transparentBackgroundEdgeColourLabel = FieldLabelWithFn( sheet.transparentBackgroundEdgeColour, _( 'Transparent background edge colour:' ) )
	transparentBackgroundEdgeColourButton = Colour3fFieldEditColourButton( sheet.transparentBackgroundEdgeColour )
	transparentBackgroundEdgeLineWidthLabel = FieldLabelWithFn( sheet.transparentBackgroundEdgeLineWidth, _( 'Transparent background edge line width:' ) )
	transparentBackgroundEdgeLineWidthEntry = FloatFieldEditEntry( sheet.transparentBackgroundEdgeLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )
	reflectedEdgeColourLabel = FieldLabelWithFn( sheet.reflectedEdgeColour, _( 'Reflected edge colour:' ) )
	reflectedEdgeColourButton = Colour3fFieldEditColourButton( sheet.reflectedEdgeColour )
	reflectedEdgeLineWidthLabel = FieldLabelWithFn( sheet.reflectedEdgeLineWidth, _( 'Reflected edge line width:' ) )
	reflectedEdgeLineWidthEntry = FloatFieldEditEntry( sheet.reflectedEdgeLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )
	liveSubdEdgeColourLabel = FieldLabelWithFn( sheet.liveSubdEdgeColour, _( 'Live subdivided edge colour:' ) )
	liveSubdEdgeColourButton = Colour3fFieldEditColourButton( sheet.liveSubdEdgeColour )
	liveSubdEdgeLineWidthLabel = FieldLabelWithFn( sheet.liveSubdEdgeLineWidth, _( 'Live subdivided edge line width:' ) )
	liveSubdEdgeLineWidthEntry = FloatFieldEditEntry( sheet.liveSubdEdgeLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )


	surfaceDiffuseColourLabel = FieldLabelWithFn( sheet.surfaceDiffuseColour, _( 'Surface diffuse colour:' ) )
	surfaceDiffuseColourButton = Colour3fFieldEditColourButton( sheet.surfaceDiffuseColour )
	surfaceSpecularColourLabel = FieldLabelWithFn( sheet.surfaceSpecularColour, _( 'Surface specular colour:' ) )
	surfaceSpecularColourButton = Colour3fFieldEditColourButton( sheet.surfaceSpecularColour )
	transparentBackgroundSurfaceDiffuseColourLabel = FieldLabelWithFn( sheet.transparentBackgroundSurfaceDiffuseColour, _( 'Transparent background diffuse colour:' ) )
	transparentBackgroundSurfaceDiffuseColourButton = Colour3fFieldEditColourButton( sheet.transparentBackgroundSurfaceDiffuseColour )
	transparentBackgroundSurfaceSpecularColourLabel = FieldLabelWithFn( sheet.transparentBackgroundSurfaceSpecularColour, _( 'Transparent background specular colour:' ) )
	transparentBackgroundSurfaceSpecularColourButton = Colour3fFieldEditColourButton( sheet.transparentBackgroundSurfaceSpecularColour )
	transparentBackgroundSurfaceAlphaLabel = FieldLabelWithFn( sheet.transparentBackgroundSurfaceAlpha, _( 'Transparent background alpha:' ) )
	transparentBackgroundSurfaceAlphaEntry = FloatFieldEditEntry( sheet.transparentBackgroundSurfaceAlpha, 0.0, 1.0, 0.01, 0.1, 2 )
	transparentBackgroundSurfaceAlphaSlider = FloatFieldEditSlider( sheet.transparentBackgroundSurfaceAlpha, 0.0, 1.0, 0.01, 0.1 )
	reflectedSurfaceFilterColourLabel = FieldLabelWithFn( sheet.reflectedSurfaceFilterColour, _( 'Reflected surface filter colour:' ) )
	reflectedSurfaceFilterColourButton = Colour3fFieldEditColourButton( sheet.reflectedSurfaceFilterColour )
	liveSubdSurfaceDiffuseColourLabel = FieldLabelWithFn( sheet.liveSubdSurfaceDiffuseColour, _( 'Live subdivided surface diffuse colour:' ) )
	liveSubdSurfaceDiffuseColourButton = Colour3fFieldEditColourButton( sheet.liveSubdSurfaceDiffuseColour )
	liveSubdSurfaceSpecularColourLabel = FieldLabelWithFn( sheet.liveSubdSurfaceSpecularColour, _( 'Live subdivided surface specular colour:' ) )
	liveSubdSurfaceSpecularColourButton = Colour3fFieldEditColourButton( sheet.liveSubdSurfaceSpecularColour )


	markedFaceColourLabel = FieldLabelWithFn( sheet.markedFaceColour, _( 'Marked face colour:' ) )
	markedFaceColourButton = Colour3fFieldEditColourButton( sheet.markedFaceColour )
	markedFaceAlphaLabel = FieldLabelWithFn( sheet.markedFaceAlpha, _( 'Marked face alpha:' ) )
	markedFaceAlphaEntry = FloatFieldEditEntry( sheet.markedFaceAlpha, 0.0, 1.0, 0.01, 0.1, 2 )
	markedFaceAlphaSlider = FloatFieldEditSlider( sheet.markedFaceAlpha, 0.0, 1.0, 0.01, 0.1 )


	overlayFactorLabel = FieldLabelWithFn( sheet.overlayFactor, _( 'Overlay factor:' ) )
	overlayFactorEntry = FloatFieldEditEntry( sheet.overlayFactor, 0.0, 1.0, 0.01, 0.1, 2 )
	overlayFactorSlider = FloatFieldEditSlider( sheet.overlayFactor, 0.0, 1.0, 0.01, 0.1 )


	mirrorPlaneColourLabel = FieldLabelWithFn( sheet.mirrorPlaneColour, _( 'Mirror plane colour:' ) )
	mirrorPlaneColourButton = Colour3fFieldEditColourButton( sheet.mirrorPlaneColour )
	mirrorPlaneAlphaLabel = FieldLabelWithFn( sheet.mirrorPlaneAlpha, _( 'Mirror plane alpha:' ) )
	mirrorPlaneAlphaEntry = FloatFieldEditEntry( sheet.mirrorPlaneAlpha, 0.0, 1.0, 0.01, 0.1, 2 )
	mirrorPlaneAlphaSlider = FloatFieldEditSlider( sheet.mirrorPlaneAlpha, 0.0, 1.0, 0.01, 0.1 )


	drawingPlaneColourLabel = FieldLabelWithFn( sheet.drawingPlaneColour, _( 'Drawing plane colour:' ) )
	drawingPlaneColourButton = Colour3fFieldEditColourButton( sheet.drawingPlaneColour )
	drawingPlaneAlphaLabel = FieldLabelWithFn( sheet.drawingPlaneAlpha, _( 'Drawing plane alpha:' ) )
	drawingPlaneAlphaEntry = FloatFieldEditEntry( sheet.drawingPlaneAlpha, 0.0, 1.0, 0.01, 0.1, 2 )
	drawingPlaneAlphaSlider = FloatFieldEditSlider( sheet.drawingPlaneAlpha, 0.0, 1.0, 0.01, 0.1 )



	vertexPickHighlightPointSizeLabel = FieldLabelWithFn( sheet.vertexPickHighlightPointSize, _( 'Vertex pick highlight point size:' ) )
	vertexPickHighlightPointSizeEntry = FloatFieldEditEntry( sheet.vertexPickHighlightPointSize, 1.0, 1024.0, 1.0, 10.0, 2 )
	edgePickHighlightLineWidthLabel = FieldLabelWithFn( sheet.edgePickHighlightLineWidth, _( 'Edge pick highlight line width:' ) )
	edgePickHighlightLineWidthEntry = FloatFieldEditEntry( sheet.edgePickHighlightLineWidth, 1.0, 1024.0, 1.0, 10.0, 2 )
	targetHighlightColourLabel = FieldLabelWithFn( sheet.targetHighlightColour, _( 'Target highlight colour:' ) )
	targetHighlightColourButton = Colour3fFieldEditColourButton( sheet.targetHighlightColour )
	markedTargetHighlightColourLabel = FieldLabelWithFn( sheet.markedTargetHighlightColour, _( 'Marked target highlight colour:' ) )
	markedTargetHighlightColourButton = Colour3fFieldEditColourButton( sheet.markedTargetHighlightColour )
	invalidTargetHighlightColourLabel = FieldLabelWithFn( sheet.invalidTargetHighlightColour, _( 'Invalid target highlight colour:' ) )
	invalidTargetHighlightColourButton = Colour3fFieldEditColourButton( sheet.invalidTargetHighlightColour )
	warningTargetHighlightColourLabel = FieldLabelWithFn( sheet.warningTargetHighlightColour, _( 'Warning target highlight colour:' ) )
	warningTargetHighlightColourButton = Colour3fFieldEditColourButton( sheet.warningTargetHighlightColour )
	pathHighlightColourLabel = FieldLabelWithFn( sheet.pathHighlightColour, _( 'Path highlight colour:' ) )
	pathHighlightColourButton = Colour3fFieldEditColourButton( sheet.pathHighlightColour )

	vertexReticleColourLabel = FieldLabelWithFn( sheet.vertexReticleColour, _( 'Vertex reticle colour:' ) )
	vertexReticleColourButton = Colour3fFieldEditColourButton( sheet.vertexReticleColour )
	modification1HighlightColourLabel = FieldLabelWithFn( sheet.modification1HighlightColour, _( 'Modification 1 highlight colour:' ) )
	modification1HighlightColourButton = Colour3fFieldEditColourButton( sheet.modification1HighlightColour )
	modification2HighlightColourLabel = FieldLabelWithFn( sheet.modification2HighlightColour, _( 'Modification 2 highlight colour:' ) )
	modification2HighlightColourButton = Colour3fFieldEditColourButton( sheet.modification2HighlightColour )
	modificationHighlightAlphaLabel = FieldLabelWithFn( sheet.modificationHighlightAlpha, _( 'Modification highlight alpha:' ) )
	modificationHighlightAlphaEntry = FloatFieldEditEntry( sheet.modificationHighlightAlpha, 0.0, 1.0, 0.01, 0.1, 2 )
	modificationHighlightAlphaSlider = FloatFieldEditSlider( sheet.modificationHighlightAlpha, 0.0, 1.0, 0.01, 0.1 )




	layout = FormLayoutFactory( bHomogeneous=False, xAttachOptions=gtk.FILL, yAttachOptions = gtk.SHRINK | gtk.FILL, xPadding=15 )
	layout.row()  <<  _( 'Background' )
	layout.row()  <<  backgroundColourLabel.label  <<  backgroundColourButton.colourButton  <<  None
	layout.row()  <<  ''
	layout.row()  <<  _( 'Grid' )
	layout.row()  <<  gridLevel1ColourLabel.label  <<  gridLevel1ColourButton.colourButton  <<  None
	layout.row()  <<  gridLevel2ColourLabel.label  <<  gridLevel2ColourButton.colourButton  <<  None
	layout.row()  <<  gridLevel3ColourLabel.label  <<  gridLevel3ColourButton.colourButton  <<  None
	layout.row()  <<  ''
	layout.row()  <<  _( 'Vertices' )
	layout.row()  <<  unmarkedVertexColourLabel.label  <<  unmarkedVertexColourButton.colourButton  <<  None
	layout.row()  <<  unmarkedVertexPointSizeLabel.label  <<  unmarkedVertexPointSizeEntry.entry  <<  None
	layout.row()  <<  markedVertexColourLabel.label  <<  markedVertexColourButton.colourButton  <<  None
	layout.row()  <<  markedVertexPointSizeLabel.label  <<  markedVertexPointSizeEntry.entry  <<  None
	layout.row()  <<  reflectedVertexColourLabel.label  <<  reflectedVertexColourButton.colourButton  <<  None
	layout.row()  <<  reflectedVertexPointSizeLabel.label  <<  reflectedVertexPointSizeEntry.entry  <<  None
	layout.row()  <<  ''
	layout.row()  <<  _( 'Edges' )
	layout.row()  <<  unmarkedEdgeColourLabel.label  <<  unmarkedEdgeColourButton.colourButton  <<  None
	layout.row()  <<  unmarkedEdgeLineWidthLabel.label  <<  unmarkedEdgeLineWidthEntry.entry  <<  None
	layout.row()  <<  markedEdgeColourLabel.label  <<  markedEdgeColourButton.colourButton  <<  None
	layout.row()  <<  markedEdgeLineWidthLabel.label  <<  markedEdgeLineWidthEntry.entry  <<  None
	layout.row()  <<  backgroundEdgeColourLabel.label  <<  backgroundEdgeColourButton.colourButton  <<  None
	layout.row()  <<  backgroundEdgeLineWidthLabel.label  <<  backgroundEdgeLineWidthEntry.entry  <<  None
	layout.row()  <<  transparentBackgroundEdgeColourLabel.label  <<  transparentBackgroundEdgeColourButton.colourButton  <<  None
	layout.row()  <<  transparentBackgroundEdgeLineWidthLabel.label  <<  transparentBackgroundEdgeLineWidthEntry.entry  <<  None
	layout.row()  <<  reflectedEdgeColourLabel.label  <<  reflectedEdgeColourButton.colourButton  <<  None
	layout.row()  <<  reflectedEdgeLineWidthLabel.label  <<  reflectedEdgeLineWidthEntry.entry  <<  None
	layout.row()  <<  liveSubdEdgeColourLabel.label  <<  liveSubdEdgeColourButton.colourButton  <<  None
	layout.row()  <<  liveSubdEdgeLineWidthLabel.label  <<  liveSubdEdgeLineWidthEntry.entry  <<  None
	layout.row()  <<  ''
	layout.row()  <<  _( 'Surface' )
	layout.row()  <<  surfaceDiffuseColourLabel.label  <<  surfaceDiffuseColourButton.colourButton  <<  None
	layout.row()  <<  surfaceSpecularColourLabel.label  <<  surfaceSpecularColourButton.colourButton  <<  None
	layout.row()  <<  transparentBackgroundSurfaceDiffuseColourLabel.label  <<  transparentBackgroundSurfaceDiffuseColourButton.colourButton  <<  None
	layout.row()  <<  transparentBackgroundSurfaceSpecularColourLabel.label  <<  transparentBackgroundSurfaceSpecularColourButton.colourButton  <<  None
	layout.row()  <<  transparentBackgroundSurfaceAlphaLabel.label  <<  transparentBackgroundSurfaceAlphaEntry.entry  <<  transparentBackgroundSurfaceAlphaSlider.slider
	layout.row()  <<  reflectedSurfaceFilterColourLabel.label  <<  reflectedSurfaceFilterColourButton.colourButton  <<  None
	layout.row()  <<  liveSubdSurfaceDiffuseColourLabel.label  <<  liveSubdSurfaceDiffuseColourButton.colourButton  <<  None
	layout.row()  <<  liveSubdSurfaceSpecularColourLabel.label  <<  liveSubdSurfaceSpecularColourButton.colourButton  <<  None
	layout.row()  <<  ''
	layout.row()  <<  _( 'Marked faces' )
	layout.row()  <<  markedFaceColourLabel.label  <<  markedFaceColourButton.colourButton  <<  None
	layout.row()  <<  markedFaceAlphaLabel.label  <<  markedFaceAlphaEntry.entry  <<  markedFaceAlphaSlider.slider
	layout.row()  <<  ''
	layout.row()  <<  _( 'Overlay' )
	layout.row()  <<  overlayFactorLabel.label  <<  overlayFactorEntry.entry  <<  overlayFactorSlider.slider
	layout.row()  <<  ''
	layout.row()  <<  _( 'Mirror plane' )
	layout.row()  <<  mirrorPlaneColourLabel.label  <<  mirrorPlaneColourButton.colourButton  <<  None
	layout.row()  <<  mirrorPlaneAlphaLabel.label  <<  mirrorPlaneAlphaEntry.entry  <<  mirrorPlaneAlphaSlider.slider
	layout.row()  <<  ''
	layout.row()  <<  _( 'Drawing plane' )
	layout.row()  <<  drawingPlaneColourLabel.label  <<  drawingPlaneColourButton.colourButton  <<  None
	layout.row()  <<  drawingPlaneAlphaLabel.label  <<  drawingPlaneAlphaEntry.entry  <<  drawingPlaneAlphaSlider.slider
	layout.row()  <<  ''
	layout.row()  <<  _( 'Highlights' )
	layout.row()  <<  vertexPickHighlightPointSizeLabel.label  <<  vertexPickHighlightPointSizeEntry.entry  <<  None
	layout.row()  <<  edgePickHighlightLineWidthLabel.label  <<  edgePickHighlightLineWidthEntry.entry  <<  None
	layout.row()  <<  targetHighlightColourLabel.label  <<  targetHighlightColourButton.colourButton  <<  None
	layout.row()  <<  markedTargetHighlightColourLabel.label  <<  markedTargetHighlightColourButton.colourButton  <<  None
	layout.row()  <<  invalidTargetHighlightColourLabel.label  <<  invalidTargetHighlightColourButton.colourButton  <<  None
	layout.row()  <<  warningTargetHighlightColourLabel.label  <<  warningTargetHighlightColourButton.colourButton  <<  None
	layout.row()  <<  pathHighlightColourLabel.label  <<  pathHighlightColourButton.colourButton  <<  None
	layout.row()  <<  ''
	layout.row()  <<  _( 'Modification highlights' )
	layout.row()  <<  vertexReticleColourLabel.label  <<  vertexReticleColourButton.colourButton  <<  None
	layout.row()  <<  modification1HighlightColourLabel.label  <<  modification1HighlightColourButton.colourButton  <<  None
	layout.row()  <<  modification2HighlightColourLabel.label  <<  modification2HighlightColourButton.colourButton  <<  None
	layout.row()  <<  modificationHighlightAlphaLabel.label  <<  modificationHighlightAlphaEntry.entry  <<  modificationHighlightAlphaSlider.slider
	layout.finalise()





class ViewDrawingSettingsEditorWindow (object):
	closeSignal = ClassSignal()
	applySignal = ClassSignal()


	def __init__(self, view, parentWindow):
		self._drawingSettings = None
		self._view = view
		self._parentWindow = parentWindow


		titleLabel = gtk.Label()
		titleLabel.set_markup( '<span foreground="#000080"><b>'  +  _( 'View drawing settings' )  +  '</b></span>' )
		titleLabel.show()


		scrolledWindow = gtk.ScrolledWindow()
		scrolledWindow.set_policy( gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC )



		self._editor = ViewDrawingSettingsEditor( self._view, None, self._parentWindow, None )

		self._editor.getSheetEditorWidget().show()
		scrolledWindow.add_with_viewport( self._editor.getSheetEditorWidget() )



		scrolledWindow.show()


		buttonBoxSep = gtk.HSeparator()
		buttonBoxSep.show()

		loadButton = gtk.Button( stock=gtk.STOCK_OPEN )
		loadButton.connect( 'clicked', self._p_onOpen )
		loadButton.show()

		saveButton = gtk.Button( stock=gtk.STOCK_SAVE )
		saveButton.connect( 'clicked', self._p_onSave )
		saveButton.show()

		resetButton = gtk.Button( _( 'Reset' ) )
		resetButton.connect( 'clicked', self._p_onReset )
		resetButton.show()

		cancelButton = gtk.Button( stock=gtk.STOCK_CANCEL )
		cancelButton.connect( 'clicked', self._p_onCancel )
		cancelButton.show()

		applyButton = gtk.Button( stock=gtk.STOCK_APPLY )
		applyButton.connect( 'clicked', self._p_onApply )
		applyButton.show()

		okButton = gtk.Button( stock=gtk.STOCK_OK )
		okButton.connect( 'clicked', self._p_onOk )
		okButton.show()


		buttonBox = gtk.HBox( spacing=10 )
		buttonBox.pack_start( loadButton, False, False )
		buttonBox.pack_start( saveButton, False, False )
		buttonBox.pack_end( okButton, False, False )
		buttonBox.pack_end( applyButton, False, False )
		buttonBox.pack_end( cancelButton, False, False )
		buttonBox.pack_end( resetButton, False, False )
		buttonBox.show()


		windowBox = gtk.VBox()
		windowBox.pack_start( titleLabel, False, False, 10 )
		windowBox.pack_start( scrolledWindow, True, True, 0 )
		windowBox.pack_start( buttonBoxSep, False, False, 10 )
		windowBox.pack_start( buttonBox, False, False, 10 )
		windowBox.show()


		self._window = gtk.Window( gtk.WINDOW_TOPLEVEL )
		self._window.set_title( _( 'View drawing settings' ) )
		self._window.set_transient_for( parentWindow )
		self._window.add( windowBox )
		self._window.set_border_width( 20 )
		self._window.set_size_request( -1, 500 )
		self._window.connect( 'delete-event', self._p_onWindowDelete )




	def start(self, drawingSettings):
		self._drawingSettings = copy( drawingSettings )
		self._editor.sheet = self._drawingSettings
		self._window.show()


	def stop(self):
		self._editor.sheet = None
		self._window.hide()
		self.closeSignal.emit()
		self._drawingSettings = None




	def _p_onOpen(self, button):
		gsviewFilter = gtk.FileFilter()
		gsviewFilter.set_name( _( 'gSculpt View drawing settings (*.gsview)' ) )
		gsviewFilter.add_pattern( '*.gsview' )

		openDialog = gtk.FileChooserDialog( _( 'Open' ), self._window, gtk.FILE_CHOOSER_ACTION_OPEN,
										( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
		openDialog.set_current_folder( os.getcwd() )
		openDialog.add_filter( gsviewFilter )
		openDialog.show()
		response = openDialog.run()
		filename = openDialog.get_filename()
		openDialog.destroy()
		if response == gtk.RESPONSE_OK:
			if filename is not None:
				try:
					readViewDrawingSettings( self._drawingSettings, filename )
				except IOError:
					messageDialog( _( 'Could not read file' ), self._window, _( 'Could not read \'%s\'' )  %  ( filename, ) )
				else:
					self._p_apply()


	def _p_onSave(self, button):
		gsviewFilter = gtk.FileFilter()
		gsviewFilter.set_name( _( 'gSculpt View drawing settings (*.gsview)' ) )
		gsviewFilter.add_pattern( '*.gsview' )

		saveDialog = gtk.FileChooserDialog( _( 'Save' ), self._window, gtk.FILE_CHOOSER_ACTION_SAVE,
									( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
		saveDialog.add_filter( gsviewFilter )
		saveDialog.set_current_folder( os.getcwd() )
		saveDialog.show()
		response = saveDialog.run()
		filenameFromDialog = saveDialog.get_filename()
		saveDialog.destroy()
		filename = None
		if response == gtk.RESPONSE_OK:
			if filenameFromDialog is not None:
				if os.path.exists( filenameFromDialog ):
					if confirmOverwriteFileDialog( filenameFromDialog, self._window ):
						filename = filenameFromDialog
				else:
					filename = filenameFromDialog
		if filename is not None:
			try:
				writeViewDrawingSettings( self._drawingSettings, filename )
			except IOError:
				messageDialog( _( 'Could not write file' ), self._window, _( 'Could not write \'%s\'' )  %  ( filename, ) )






	def _p_apply(self):
		self.applySignal.emit( self._drawingSettings )


	def _p_onReset(self, widget):
		dialog = gtk.Dialog( _( 'Confirm' ), self._window, gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT, ( gtk.STOCK_YES, gtk.RESPONSE_ACCEPT, gtk.STOCK_NO, gtk.RESPONSE_REJECT ) )
		dialog.vbox.pack_start( gtk.Label( _( 'Are you sure you wish to reset the view drawing settings?' ) ) )
		dialog.show_all()
		response = dialog.run()
		dialog.destroy()
		if response == gtk.RESPONSE_ACCEPT:
			self._drawingSettings = ViewDrawingSettings()
			self._editor.sheet = self._drawingSettings
			self._p_apply()



	def _p_onCancel(self, widget):
		self.stop()


	def _p_onApply(self, widget):
		self._p_apply()


	def _p_onOk(self, widget):
		self._p_apply()
		self.stop()

	def _p_onWindowDelete(self, widget, event):
		self.stop()
		# Don't allow the window to be deleted
		return True
