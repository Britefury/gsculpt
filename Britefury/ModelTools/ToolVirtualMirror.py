##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Point3, Quaternion

from Britefury.UI.FormLayout import FormLayoutFactory
from Britefury.UI.UIAction import UIAction

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.Model import GSProductModel




class ProcVirtualMirror (GSProcedure):
	description = _( 'Set mirror plane' )

	position = Field( Point3, Point3() )
	rotation = Field( Quaternion, Quaternion() )
	size = Field( float, 100.0 )
	bVisible = Field( bool, True )
	bEnabled = Field( bool, True )

	inputClass = GSProductModel


	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.bVirtualMirrorEnabled = self.bEnabled
		inputObject.virtualMirrorPlane.position = self.position
		inputObject.virtualMirrorPlane.rotation = self.rotation
		inputObject.virtualMirrorPlane.size = self.size
		inputObject.virtualMirrorPlane.bVisible = self.bVisible
		return inputObject



_positionAction = UIAction( 'Tools.Model.VirtualMirrorMode.Position', ( _( 'Tools' ), _( 'Model' ), _( 'Virtual mirror mode' ), _( 'Position mode' ) ),
			_( 'Position' ), _( 'Position' ), 'w', _( 'Position mode' ) )
_rotationAction = UIAction( 'Tools.Model.VirtualMirrorMode.Rotation', ( _( 'Tools' ), _( 'Model' ), _( 'Virtual mirror mode' ), _( 'Rotation mode' ) ),
			_( 'Rotation' ), _( 'Rotation' ), 'r', _( 'Rotation mode' ) )
_sizeAction = UIAction( 'Tools.Model.VirtualMirrorMode.Size', ( _( 'Tools' ), _( 'Model' ), _( 'Virtual mirror mode' ), _( 'Size mode' ) ),
			_( 'Size' ), _( 'Size' ), 'e', _( 'Size mode' ) )



class ToolVirtualMirror (GSProcedureTool):
	sheet = SheetRefField( ProcVirtualMirror )

	functionClass = ProcVirtualMirror
	pageTitle = _( 'V.Mirr.' )
	titleText = _( 'Virtual Mirror' )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	rotationManip = RotationFieldManipulator( sheet.rotation, sheet.position, snapping=FieldManipulatorSnappingAngle() )
	sizeManip = Real1FieldManipulator( sheet.size, 0.0, 1.0e18, RealFieldManipulator.WorldSpaceScaleFactory( sheet.position ),
									snapping=FieldManipulatorSnappingDistance() )

	manipulatorSwitch = FieldManipulatorSwitch( [ positionManip, rotationManip, sizeManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	positionButton = manipulatorSelector.addChoice( positionManip, _positionAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, _rotationAction )
	sizeButton = manipulatorSelector.addChoice( sizeManip, _sizeAction )

	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e18, 1.0e18, 0.1, 10.0, 4 )
	rotationLabel = FieldLabelWithFn( sheet.rotation, _( 'Rotation:' ) )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.rotation, _( 'Set normal' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.rotation )
	sizeLabel = FieldLabelWithFn( sheet.size, _( 'Size:' ) )
	sizeEntry = FloatFieldEditEntry( sheet.size, 0.0, 1.0e18, 0.1, 10.0, 4 )
	bVisibleCheck = BoolFieldEditCheck( sheet.bVisible, _( 'Show plane' ) )
	bEnabledCheck = BoolFieldEditCheck( sheet.bEnabled, _( 'Enabled' ) )



	layout = FormLayoutFactory()
	layout.row()  <<  bEnabledCheck.check
	layout.row()  <<  bVisibleCheck.check
	layout.row()
	layout.row()  <<  _( 'Mouse function:' )
	layout.row()  <<  positionButton
	layout.row()  <<  rotationButton
	layout.row()  <<  sizeButton
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  None  <<  ( rotationPopup.popup, 2 )  <<  None
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  sizeLabel.label  <<  sizeEntry.entry
	layout.row()
	layout.finalise()


