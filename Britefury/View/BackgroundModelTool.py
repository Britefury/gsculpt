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
from Britefury.UI.ButtonPopupMenu import ButtonPopupMenu
from Britefury.UI.UIAction import UIAction

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.Background.BackgroundModelFileRefFieldEditor import BackgroundModelFileRefFieldEditChooser
from Britefury.Background.BackgroundModelFileChooser import backgroundModelFileChooserDialog
from Britefury.Background.BackgroundModelFileRef import BackgroundModelFileRef

from BackgroundModel import BackgroundModel, BackgroundModelList



class BackgroundModelToolProxy (Sheet):
	modelRef = SheetRefField( BackgroundModel )
	model = FieldProxy( modelRef.model )
	position = FieldProxy( modelRef.position )
	scale = FieldProxy( modelRef.scale )
	rotation = FieldProxy( modelRef.rotation )
	bOverlay = FieldProxy( modelRef.bOverlay )
	bVisible = FieldProxy( modelRef.bVisible )



class ModelOptionList (object):
	def __init__(self):
		super( ModelOptionList, self ).__init__()
		self._modelList = None
		self.optionList = OptionList( [ ( _( 'Model' ), str ) ] )


	def attachModelList(self, modelList):
		assert self._modelList is None, 'model list already attached'
		self._modelList = modelList
		self._modelList.modelAddedSignal.connect( self._onModelAdded )
		self._modelList.modelInsertedSignal.connect( self._onModelInserted )
		self._modelList.modelRemoveSignal.connect( self._onModelRemove )
		self._modelList.modelPathChangedSignal.connect( self._onModelPathChanged )
		for model in self._modelList:
			self.optionList.append( model, self._getModelColumnValue( model ) )

	def detachModelList(self):
		assert self._modelList is not None, 'no model list attached'
		self.optionList.clear()
		self._modelList.modelAddedSignal.disconnect( self._onModelAdded )
		self._modelList.modelInsertedSignal.disconnect( self._onModelInserted )
		self._modelList.modelRemoveSignal.disconnect( self._onModelRemove )
		self._modelList.modelPathChangedSignal.disconnect( self._onModelPathChanged )
		self._modelList = None


	def _getModelColumnValue(self, model):
		if model.model.path is not None:
			return os.path.basename( model.model.path )
		else:
			return _( '[NONE]' )


	def _onModelAdded(self, modelList, model):
		self.optionList.append( model, self._getModelColumnValue( model ) )

	def _onModelInserted(self, modelList, index, model):
		self.optionList.insert( index, model, self._getModelColumnValue( model ) )

	def _onModelRemove(self, modelList, model):
		self.optionList.remove( model )

	def _onModelPathChanged(self, modelList, model):
		def _refresh():
			self._refreshModelPath( model )
		queueEvent( _refresh )

	def _refreshModelPath(self, model):
		if model in self._modelList:
			self.optionList[model] = ( self._getModelColumnValue( model ), )



_positionAction = UIAction( 'Tools.Main.BackgroundModelMode.Position', ( _( 'Tools' ), _( 'Main' ), _( 'Background model mode' ), _( 'Position mode' ) ),
			_( 'Position' ), _( 'Position' ), 'w', _( 'Position mode' ) )
_scaleAction = UIAction( 'Tools.Main.BackgroundModelMode.Scale', ( _( 'Tools' ), _( 'Main' ), _( 'Background model mode' ), _( 'Scale mode' ) ),
			_( 'Scale' ), _( 'Scale' ), 'e', _( 'Scale mode' ) )
_rotationAction = UIAction( 'Tools.Main.BackgroundModelMode.Rotation', ( _( 'Tools' ), _( 'Main' ), _( 'Background model mode' ), _( 'Rotation mode' ) ),
			_( 'Rotation' ), _( 'Rotation' ), 'r', _( 'Rotation mode' ) )


class BackgroundModelTool (SheetEditorTool):
	sheet = SheetRefField( BackgroundModelToolProxy )

	pageTitle = _( 'Bk.Md.' )
	titleText = _( 'Background Models' )


	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	scaleManip = Scale3FieldManipulator3d( sheet.scale, positionField=sheet.position, spaceRotationField=sheet.rotation, snapping=FieldManipulatorSnappingFraction() )
	rotationManip = RotationFieldManipulator( sheet.rotation, sheet.position, snapping=FieldManipulatorSnappingAngle() )

	manipulatorSwitch = FieldManipulatorSwitch( [ positionManip, scaleManip, rotationManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	positionButton = manipulatorSelector.addChoice( positionManip, _positionAction )
	scaleButton = manipulatorSelector.addChoice( scaleManip, _scaleAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, _rotationAction )

	modelChooser = BackgroundModelFileRefFieldEditChooser( sheet.model, _( 'Choose model' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e18, 1.0e18, 0.1, 10.0, 4 )
	scaleEntry = Vector3FieldEditEntry( sheet.scale, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	rotationEntry = QuaternionFieldEditEntry( sheet.rotation )
	bOverlayCheck = BoolFieldEditCheck( sheet.bOverlay, _( 'Overlay' ) )
	bVisibleCheck = BoolFieldEditCheck( sheet.bVisible, _( 'Visible' ) )




	def __init__(self, view, editorSettings, parentWindow, commandHistory):
		self._modelOptionList = ModelOptionList()
		self._modelOptionList.optionList.getWidget().show()
		self._modelAddButton = gtk.Button( stock=gtk.STOCK_ADD )
		self._modelAddButton.show()
		self._modelRemoveButton = gtk.Button( stock=gtk.STOCK_REMOVE )
		self._modelRemoveButton.show()
		self._positionOriginButton = gtk.Button( _( 'Origin' ) )
		self._positionOriginButton.show()

		super( BackgroundModelTool, self ).__init__( view, editorSettings, parentWindow, commandHistory)
		self._proxy = BackgroundModelToolProxy()
		self.sheet = self._proxy

		self._modelList = None

		self._modelOptionList.optionList.choiceSignal.connect( self._onModelChoice )
		self._modelAddButton.connect( 'clicked', self._onModelAdd )
		self._modelRemoveButton.connect( 'clicked', self._onModelRemove )
		self._positionOriginButton.connect( 'clicked', self._onPositionOrigin )



	def shutdownTool(self):
		super( BackgroundModelTool, self ).shutdownTool()
		self._modelOptionList.optionList.choiceSignal.disconnect( self._onModelChoice )
		self._modelOptionList.detachModelList()
		self._modelOptionList = None
		self._modelAddButton = None
		self._modelRemoveButton = None
		self._positionOriginButton = None
		self.modelChooser.modelSignal.disconnect( self._onModelChoice )


	def attachModelList(self, modelList):
		self._modelList = modelList
		self._modelOptionList.attachModelList( self._modelList )

	def detachModelList(self):
		self._modelOptionList.detachModelList()
		self._modelList = None


	def layout(self):
		layout = FormLayout()
		layout.row()  <<  _( 'Mouse function:' )
		layout.row()  <<  self.positionButton
		layout.row()  <<  self.scaleButton
		layout.row()  <<  self.rotationButton
		layout.row()
		layout.block( 6 )  <<  self._modelOptionList.optionList
		layout.row()  <<  self._modelAddButton  <<  self._modelRemoveButton
		layout.row()
		layout.row()  <<  _( 'Model:' )
		layout.row()  <<  self.modelChooser.chooser
		layout.row()
		layout.row()  <<  _( 'Position:' )
		layout.row()  <<  self._positionOriginButton
		layout.row()  <<  'X:'  <<  self.positionEntry.x
		layout.row()  <<  'Y:'  <<  self.positionEntry.y
		layout.row()  <<  'Z:'  <<  self.positionEntry.z
		layout.row()
		layout.row()  <<  _( 'Scale:' )
		layout.row()  <<  'X:'  <<  self.scaleEntry.x
		layout.row()  <<  'Y:'  <<  self.scaleEntry.y
		layout.row()  <<  'Z:'  <<  self.scaleEntry.z
		layout.row()
		layout.row()  <<  _( 'Rotation:' )
		layout.row()  <<  'X:'  <<  self.rotationEntry.x
		layout.row()  <<  'Y:'  <<  self.rotationEntry.y
		layout.row()  <<  'Z:'  <<  self.rotationEntry.z
		layout.row()
		layout.row()  <<  self.bOverlayCheck.check
		layout.row()  <<  self.bVisibleCheck.check
		layout.row()
		layout.finalise()
		return layout.getWidget()


	def _onModelChoice(self, event, choice):
		self._proxy.modelRef = choice

	def _onModelAdd(self, widget):
		self._commandHistory.freeze()
		modelPath = backgroundModelFileChooserDialog( _( 'Choose model file' ), self._parentWindow )
		if modelPath is not None:
			model = self._modelList.newModel()
			self._modelOptionList.optionList.choice = model
			model.model = BackgroundModelFileRef( modelPath )
		self._commandHistory.thaw()


	def _onModelRemove(self, widget):
		if self._modelOptionList.optionList.choice is not None:
			self._modelList.removeModel( self._modelOptionList.optionList.choice )

	def _onPositionOrigin(self, widget):
		self._proxy.position = Point3()

	def _onMouseFunc(self, event, choice):
		self.manipulatorSwitch.switchIndex.value = choice





