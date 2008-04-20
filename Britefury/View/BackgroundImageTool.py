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

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEdit import *
from Britefury.SheetEdit.SheetEditorTool import *
from Britefury.SheetEdit.FieldEditors import *
from Britefury.Manipulator.FieldManipulators import *

from Britefury.UI.OptionList import OptionList
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.ToggleButton import ToggleButton
from Britefury.UI.ToggleGroup import ToggleGroup
from Britefury.UI.UIAction import UIAction

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.Texture.TextureRefFieldEditor import TextureRefFieldEditChooser
from Britefury.Texture.TextureChooser import textureChooserDialog
from Britefury.Texture.TextureRef import TextureRef

from BackgroundImage import BackgroundImage, BackgroundImageList



class BackgroundImageToolProxy (Sheet):
	imageRef = SheetRefField( BackgroundImage )
	texture = FieldProxy( imageRef.texture )
	position = FieldProxy( imageRef.position )
	rotation = FieldProxy( imageRef.rotation )
	size = FieldProxy( imageRef.size )
	brightness = FieldProxy( imageRef.brightness )
	opacity = FieldProxy( imageRef.opacity )
	bOverlay = FieldProxy( imageRef.bOverlay )
	bVisible = FieldProxy( imageRef.bVisible )
	bBackfaceCull = FieldProxy( imageRef.bBackfaceCull )



class ImageOptionList (object):
	def __init__(self):
		super( ImageOptionList, self ).__init__()
		self._imageList = None
		self.optionList = OptionList( [ ( _( 'Image' ), str ) ] )


	def attachImageList(self, imageList):
		assert self._imageList is None, 'image list already attached'
		self._imageList = imageList
		self._imageList.imageAddedSignal.connect( self._p_onImageAdded )
		self._imageList.imageInsertedSignal.connect( self._p_onImageInserted )
		self._imageList.imageRemoveSignal.connect( self._p_onImageRemove )
		self._imageList.imageTextureChangedSignal.connect( self._p_onImageTextureChanged )
		for image in self._imageList:
			self.optionList.append( image, self._p_getTextureColumnValue( image ) )

	def detachImageList(self):
		assert self._imageList is not None, 'no image list attached'
		self.optionList.clear()
		self._imageList.imageAddedSignal.disconnect( self._p_onImageAdded )
		self._imageList.imageInsertedSignal.disconnect( self._p_onImageInserted )
		self._imageList.imageRemoveSignal.disconnect( self._p_onImageRemove )
		self._imageList.imageTextureChangedSignal.disconnect( self._p_onImageTextureChanged )
		self._imageList = None


	def _p_getTextureColumnValue(self, image):
		if image.texture.texture is not None:
			return os.path.basename( image.texture.texture.path )
		else:
			return _( '[NONE]' )


	def _p_onImageAdded(self, imageList, image):
		self.optionList.append( image, self._p_getTextureColumnValue( image ) )

	def _p_onImageInserted(self, imageList, index, image):
		self.optionList.insert( index, image, self._p_getTextureColumnValue( image ) )

	def _p_onImageRemove(self, imageList, image):
		self.optionList.remove( image )

	def _p_onImageTextureChanged(self, imageList, image):
		def _refresh():
			self._p_refreshImageTexture( image )
		queueEvent( _refresh )

	def _p_refreshImageTexture(self, image):
		if image in self._imageList:
			self.optionList[image] = ( self._p_getTextureColumnValue( image ), )



_positionAction = UIAction( 'Tools.Main.BackgroundImageMode.Position', ( _( 'Tools' ), _( 'Main' ), _( 'Background image mode' ), _( 'Position mode' ) ),
			_( 'Position' ), _( 'Position' ), 'w', _( 'Position mode' ) )
_rotationAction = UIAction( 'Tools.Main.BackgroundImageMode.Rotation', ( _( 'Tools' ), _( 'Main' ), _( 'Background image mode' ), _( 'Rotation mode' ) ),
			_( 'Rotation' ), _( 'Rotation' ), 'r', _( 'Rotation mode' ) )
_sizeAction = UIAction( 'Tools.Main.BackgroundImageMode.Size', ( _( 'Tools' ), _( 'Main' ), _( 'Background image mode' ), _( 'Size mode' ) ),
			_( 'Size' ), _( 'Size' ), 'e', _( 'Size mode' ) )


class BackgroundImageTool (SheetEditorTool):
	sheet = SheetRefField( BackgroundImageToolProxy )

	pageTitle = _( 'Bkg.' )
	titleText = _( 'Background Images' )


	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	rotationManip = RotationFieldManipulator( sheet.rotation, sheet.position, snapping=FieldManipulatorSnappingAngle() )
	sizeManip = Scale2FieldManipulator3d( sheet.size, positionField=sheet.position, spaceRotationField=sheet.rotation, snapping=FieldManipulatorSnappingFraction() )

	manipulatorSwitch = FieldManipulatorSwitch( [ positionManip, rotationManip, sizeManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	positionButton = manipulatorSelector.addChoice( positionManip, _positionAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, _rotationAction )
	sizeButton = manipulatorSelector.addChoice( sizeManip, _sizeAction )

	textureChooser = TextureRefFieldEditChooser( sheet.texture, _( 'Choose texture' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e18, 1.0e18, 0.1, 10.0, 4 )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.rotation, _( 'Set normal' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.rotation )
	sizeEntry = Vector2FieldEditEntry( sheet.size, 0.0, 1.0e18, 0.1, 10.0, 4 )
	brightnessEntry = FloatFieldEditEntry( sheet.brightness, 0.0, 1.0, 0.01, 0.1, 2 )
	brightnessSlider = FloatFieldEditSlider( sheet.brightness, 0.0, 1.0, 0.01, 0.1 )
	opacityEntry = FloatFieldEditEntry( sheet.opacity, 0.0, 1.0, 0.01, 0.1, 2 )
	opacitySlider = FloatFieldEditSlider( sheet.opacity, 0.0, 1.0, 0.01, 0.1 )
	bOverlayCheck = BoolFieldEditCheck( sheet.bOverlay, _( 'Overlay' ) )
	bVisibleCheck = BoolFieldEditCheck( sheet.bVisible, _( 'Visible' ) )
	bBackfaceCullCheck = BoolFieldEditCheck( sheet.bBackfaceCull, _( 'Backface cull' ) )




	def __init__(self, view, editorSettings, parentWindow, commandHistory):
		self._imageOptionList = ImageOptionList()
		self._imageOptionList.optionList.getWidget().show()
		self._imageAddButton = gtk.Button( stock=gtk.STOCK_ADD )
		self._imageAddButton.show()
		self._imageRemoveButton = gtk.Button( stock=gtk.STOCK_REMOVE )
		self._imageRemoveButton.show()
		self._positionOriginButton = gtk.Button( _( 'Origin' ) )
		self._positionOriginButton.show()
		self._sizeFromImageButton = gtk.Button( _( 'From image' ) )
		self._sizeFromImageButton.show()

		super( BackgroundImageTool, self ).__init__( view, editorSettings, parentWindow, commandHistory)
		self._proxy = BackgroundImageToolProxy()
		self.sheet = self._proxy

		self._imageList = None

		self._imageOptionList.optionList.choiceSignal.connect( self._p_onImageChoice )
		self._imageAddButton.connect( 'clicked', self._p_onImageAdd )
		self._imageRemoveButton.connect( 'clicked', self._p_onImageRemove )
		self._positionOriginButton.connect( 'clicked', self._p_onPositionOrigin )
		self._sizeFromImageButton.connect( 'clicked', self._p_onSizeFromImage )



	def shutdownTool(self):
		super( BackgroundImageTool, self ).shutdownTool()
		self._imageOptionList.optionList.choiceSignal.disconnect( self._p_onImageChoice )
		self._imageOptionList.detachImageList()
		self._imageOptionList = None
		self._imageAddButton = None
		self._imageRemoveButton = None
		self._positionOriginButton = None
		self._sizeFromImageButton = None
		self.textureChooser.textureSignal.disconnect( self._p_onTextureChoice )


	def attachImageList(self, imageList):
		self._imageList = imageList
		self._imageOptionList.attachImageList( self._imageList )

	def detachImageList(self):
		self._imageOptionList.detachImageList()
		self._imageList = None


	def layout(self):
		layout = FormLayout()
		layout.row()  <<  _( 'Mouse function:' )
		layout.row()  <<  self.positionButton
		layout.row()  <<  self.rotationButton
		layout.row()  <<  self.sizeButton
		layout.row()
		layout.block( 6 )  <<  self._imageOptionList.optionList
		layout.row()  <<  self._imageAddButton  <<  self._imageRemoveButton
		layout.row()
		layout.row()  <<  _( 'Texture:' )
		layout.row()  <<  self.textureChooser.chooser
		layout.row()
		layout.row()  <<  _( 'Position:' )
		layout.row()  <<  self._positionOriginButton
		layout.row()  <<  'X:'  <<  self.positionEntry.x
		layout.row()  <<  'Y:'  <<  self.positionEntry.y
		layout.row()  <<  'Z:'  <<  self.positionEntry.z
		layout.row()
		layout.row()  <<  _( 'Rotation:' )
		layout.row()  <<  self.rotationPopup.popup
		layout.row()  <<  'X:'  <<  self.rotationEntry.x
		layout.row()  <<  'Y:'  <<  self.rotationEntry.y
		layout.row()  <<  'Z:'  <<  self.rotationEntry.z
		layout.row()
		layout.row()  <<  _( 'Size:' )
		layout.row()  <<  self._sizeFromImageButton
		layout.row()  <<  'X:'  <<  self.sizeEntry.x
		layout.row()  <<  'Y:'  <<  self.sizeEntry.y
		layout.row()
		layout.row()  <<  _( 'Brightness:' )  <<  self.brightnessEntry.entry
		layout.row()  <<  self.brightnessSlider.slider
		layout.row()  <<  _( 'Opacity:' )  <<  self.opacityEntry.entry
		layout.row()  <<  self.opacitySlider.slider
		layout.row()  <<  self.bOverlayCheck.check
		layout.row()  <<  self.bVisibleCheck.check
		layout.row()  <<  self.bBackfaceCullCheck.check
		layout.row()
		layout.finalise()
		return layout.getWidget()


	def _p_onImageChoice(self, event, choice):
		self._proxy.imageRef = choice

	def _p_onImageAdd(self, widget):
		self._commandHistory.freeze()
		tex = textureChooserDialog( _( 'Choose image file' ), self._parentWindow )
		if tex is not None:
			image = self._imageList.newImage()
			self._imageOptionList.optionList.choice = image
			image.texture = TextureRef( tex )
			image.setSizeToPreferredSize()
		self._commandHistory.thaw()


	def _p_onImageRemove(self, widget):
		if self._imageOptionList.optionList.choice is not None:
			self._imageList.removeImage( self._imageOptionList.optionList.choice )

	def _p_onPositionOrigin(self, widget):
		self._proxy.position = Point3()

	def _p_onSizeFromImage(self, widget):
		image = self._proxy.imageRef
		if image is not None:
			image.setSizeToPreferredSize()

	def _p_onMouseFunc(self, event, choice):
		self.manipulatorSwitch.switchIndex.value = choice





