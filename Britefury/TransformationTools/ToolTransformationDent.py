##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

import math

from Britefury.Math.Math import Vector3, Matrix4

from Britefury.UI.FormLayout import FormLayoutFactory
from Britefury.UI.UIAction import UIAction

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *
from Britefury.Manipulator.FieldManipulators import *
from Britefury.Manipulator import RealManipulator

from Britefury.TransformationTools.ToolTransformation import *
from Britefury.TransformationTools.ToolTransformationBase import *
from Britefury.TransformationTools.ToolRotatedTransformation import *
from Britefury.TransformationTools import TransformationToolActions
from Britefury.Transformation.Transformation import DentTransformation, MatrixTransformation, Radius2Transformation




def makeTransformation(model, target, xformRotation, dent):
	centre = model.getBBox( target == TransformationTarget.POINTS ).getCentre()

	rotationMatrix = Matrix4.rotate( xformRotation )
	modelToXformFrameMatrix = Matrix4.translate( -centre.toVector3() )  *  rotationMatrix.rigidBodyInverse()
	xformToModelFrameMatrix = modelToXformFrameMatrix.rigidBodyInverse()

	getRadiusX0 = MatrixTransformation( modelToXformFrameMatrix )
	getRadiusX1 = Radius2Transformation()
	getRadiusX0.chainTo( getRadiusX1 )

	box = model.getBBox( getRadiusX0, target == TransformationTarget.POINTS )
	radius = box.getUpper().z

	x0 = MatrixTransformation( modelToXformFrameMatrix )
	x1 = DentTransformation( radius, dent )
	x2 = MatrixTransformation( xformToModelFrameMatrix )
	x1.chainTo( x2 )
	x0.chainTo( x1 )

	return x0




class ProcTransformationDent (ProcRotatedTransformation):
	description = _( 'Dent' )

	dent = Field( float, 1.0 )


	def procedureInvoke(self, inputObject, errorHandler):
		x0 = makeTransformation( inputObject, self.target, self.xformRotation, self.dent )
		ProcTransformationDent.transform( inputObject, x0, self.target )
		return inputObject




class TransformationDentAdjustableInvoker (RotatedTransformationAdjustableInvoker):
	functionClass = ProcTransformationDent

	def adjust(self):
		x0 = makeTransformation( self.model, self._proc.target, self._proc.xformRotation, self._proc.dent )
		self.transform( x0, self._proc.target )




class ToolTransformationDent (ToolRotatedTransformation):
	sheet = SheetRefField( ProcTransformationDent )

	functionClass = ProcTransformationDent
	pageTitle = _( 'Dent' )
	titleText = _( 'Dent' )
	adjustableInvoker = TransformationDentAdjustableInvoker


	dentManip = Real1FieldManipulator( sheet.dent, -1.0e8, 1.0e8, RealManipulator.RealManipulator.FixedScale( 0.005 ), snapping=FieldManipulatorSnappingFraction() )

	rotationManip = RotationFieldManipulator( sheet.xformRotation, ToolRotatedTransformation._focalPoint, snapping=FieldManipulatorSnappingAngle() )

	manipulatorSwitch = FieldManipulatorSwitch( [ dentManip, rotationManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	dentButton = manipulatorSelector.addChoice( dentManip, TransformationToolActions.deformAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, TransformationToolActions.rotationAction )


	rotationLabel = FieldLabelWithFn( sheet.xformRotation, _( 'Deformation rotation:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.xformRotation )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.xformRotation, _( 'Set rotation' ) )

	dentLabel = FieldLabelWithFn( sheet.dent, _( 'Dent:' ) )
	dentEntry = FloatFieldEditEntry( sheet.dent, -1.0e8, 1.0e8, 0.01, 0.1, 4 )


	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Mouse function:' )
	layout.row()  <<  dentButton
	layout.row()  <<  rotationButton
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  rotationPopup.popup
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  dentLabel.label  <<  dentEntry.entry
	layout.row()
	layout.finalise()

