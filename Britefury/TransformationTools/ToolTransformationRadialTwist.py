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
from Britefury.Transformation.Transformation import RadialTwistTransformation, MatrixTransformation, Radius2Transformation




def makeTransformation(model, target, xformRotation, twist, lower, upper):
	centre = model.getBBox( target == TransformationTarget.POINTS ).getCentre()

	rotationMatrix = Matrix4.rotate( xformRotation )
	modelToXformFrameMatrix = Matrix4.translate( -centre.toVector3() )  *  rotationMatrix.rigidBodyInverse()
	xformToModelFrameMatrix = modelToXformFrameMatrix.rigidBodyInverse()

	getRadiusX0 = MatrixTransformation( modelToXformFrameMatrix )
	getRadiusX1 = Radius2Transformation()
	getRadiusX0.chainTo( getRadiusX1 )

	box = model.getBBox( getRadiusX0, target == TransformationTarget.POINTS )
	d1 = box.getUpper().z

	x0 = MatrixTransformation( modelToXformFrameMatrix )
	x1 = RadialTwistTransformation( 0.0, 0.0, d1, -math.radians( twist ), lower * 0.01, upper * 0.01 )
	x2 = MatrixTransformation( xformToModelFrameMatrix )
	x1.chainTo( x2 )
	x0.chainTo( x1 )

	return x0




class ProcTransformationRadialTwist (ProcRotatedTransformation):
	description = _( 'Radial Twist' )

	twist = Field( float, 0.0 )
	lower = Field( float, 0.0 )
	upper = Field( float, 100.0 )


	def procedureInvoke(self, inputObject, errorHandler):
		x0 = makeTransformation( inputObject, self.target, self.xformRotation, self.twist, self.lower, self.upper )
		ProcTransformationRadialTwist.transform( inputObject, x0, self.target )
		return inputObject




class TransformationRadialTwistAdjustableInvoker (RotatedTransformationAdjustableInvoker):
	functionClass = ProcTransformationRadialTwist

	def adjust(self):
		x0 = makeTransformation( self.model, self._proc.target, self._proc.xformRotation, self._proc.twist, self._proc.lower, self._proc.upper )
		self.transform( x0, self._proc.target )




class ToolTransformationRadialTwist (ToolRotatedTransformation):
	sheet = SheetRefField( ProcTransformationRadialTwist )

	functionClass = ProcTransformationRadialTwist
	pageTitle = _( 'Radial Twist' )
	titleText = _( 'Radial Twist' )
	adjustableInvoker = TransformationRadialTwistAdjustableInvoker


	twistManip = Real1FieldManipulator( sheet.twist, -3.6e18, 3.6e18, RealManipulator.RealManipulator.FixedScale( 0.5 ), snapping=FieldManipulatorSnappingAngle() )

	rotationManip = RotationFieldManipulator( sheet.xformRotation, ToolRotatedTransformation._focalPoint, snapping=FieldManipulatorSnappingAngle() )

	manipulatorSwitch = FieldManipulatorSwitch( [ twistManip, rotationManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	twistButton = manipulatorSelector.addChoice( twistManip, TransformationToolActions.deformAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, TransformationToolActions.rotationAction )


	rotationLabel = FieldLabelWithFn( sheet.xformRotation, _( 'Deformation rotation:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.xformRotation )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.xformRotation, _( 'Set rotation' ) )

	twistLabel = FieldLabelWithFn( sheet.twist, _( 'Twist:' ) )
	twistEntry = FloatFieldEditEntry( sheet.twist, -3.6e18, 3.6e18, 1.0, 10.0, 4 )

	lowerLabel = FieldLabelWithFn( sheet.lower, _( 'Lower (%%):' ) )
	lowerEntry = FloatFieldEditEntry( sheet.lower, 0.0, 100.0, 0.01, 1.0, 4 )
	upperLabel = FieldLabelWithFn( sheet.upper, _( 'Upper (%%):' ) )
	upperEntry = FloatFieldEditEntry( sheet.upper, 0.0, 100.0, 0.01, 1.0, 4 )

	lowerSlider = FloatFieldEditSlider( sheet.lower, 0.0, 100.0, 0.01, 1.0 )
	upperSlider = FloatFieldEditSlider( sheet.upper, 0.0, 100.0, 0.01, 1.0 )


	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Mouse function:' )
	layout.row()  <<  twistButton
	layout.row()  <<  rotationButton
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  rotationPopup.popup
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  twistLabel.label  <<  twistEntry.entry
	layout.row()
	layout.row()  <<  lowerLabel.label  <<  lowerEntry.entry
	layout.row()  <<  lowerSlider.slider
	layout.row()  <<  upperLabel.label  <<  upperEntry.entry
	layout.row()  <<  upperSlider.slider
	layout.row()
	layout.finalise()

