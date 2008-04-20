##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3, Matrix4

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *
from Britefury.Manipulator.FieldManipulators import *

from Britefury.TransformationTools.ToolTransformation import *





class ProcTransformationMove (ProcTransformation):
	description = _( 'Move' )

	translation = Field( Vector3, Vector3() )


	def procedureInvoke(self, inputObject, errorHandler):
		ProcTransformationMove.transform( inputObject, Matrix4.translate( self.translation ), self.target )
		return inputObject




class TransformationMoveAdjustableInvoker (TransformationAdjustableInvoker):
	functionClass = ProcTransformationMove

	def adjust(self):
		self.transform( Matrix4.translate( self._proc.translation ), self._proc.target )




class ToolTransformationMove (ToolTransformation):
	sheet = SheetRefField( ProcTransformationMove )

	functionClass = ProcTransformationMove
	pageTitle = _( 'Move' )
	titleText = _( 'Move' )
	adjustableInvoker = TransformationMoveAdjustableInvoker


	translationManip = Translation3FieldManipulator3d( sheet.translation, ToolTransformationBase._focalPoint, snapping=FieldManipulatorSnappingDistance() )
	translationManip.enable()


	translationLabel = FieldLabelWithFn( sheet.translation, _( 'Move:' ) )
	translationEntry = Vector3FieldEditEntry( sheet.translation, -1.0e18, 1.0e18, 0.1, 10.0, 4 )

	layout = FormLayoutFactory()
	layout.row()  <<  translationLabel.label
	layout.row()  <<  'X:'  <<  translationEntry.x
	layout.row()  <<  'Y:'  <<  translationEntry.y
	layout.row()  <<  'Z:'  <<  translationEntry.z
	layout.row()
	layout.finalise()

