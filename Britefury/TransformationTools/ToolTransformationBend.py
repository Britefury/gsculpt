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
from Britefury.Transformation.Transformation import BendTransformation, MatrixTransformation, Radius2Transformation




def makeTransformation(model, target, xformRotation, bend, direction, anchor, lower, upper):
	if abs( bend )  >=  1.0e-6:
		bendRadians = math.radians( bend )
		dirRadians = math.radians( direction )

		rotationMatrix = Matrix4.rotate( xformRotation )
		modelToXformFrameMatrix = rotationMatrix.rigidBodyInverse()
		xformToModelFrameMatrix = rotationMatrix

		bendAxesMatrix = modelToXformFrameMatrix * Matrix4.rotateZ( dirRadians )

		box = model.getBBox( bendAxesMatrix, target == TransformationTarget.POINTS )

		bendBoxLower = box.getLower()
		bendBoxCentre = box.getCentre()

		lower *= 0.01
		upper *= 0.01
		anchor *= 0.01

		bendLower = bendBoxLower.z  +  box.getDepth() * lower
		bendDepth = box.getDepth()  *  ( upper - lower )

		if abs( bendDepth )  >=  box.getMajorSize() * 1.0e-6:
			bendAnchorZ = bendLower  +  bendDepth * anchor
			bendAnchorToLower = -bendDepth  *  anchor
			bendAnchorToUpper = bendDepth  *  ( 1.0 - anchor )

			# Compute bend radius
			bendRadius = -bendDepth / bendRadians

			# Make sure origin is *outside* of bounding box
			if bendRadius > 0.0  and  bendRadius  <  box.getWidth() * 0.5:
				bendRadius = box.getWidth() * 0.5
			if bendRadius < 0.0  and  bendRadius  >  -box.getWidth() * 0.5:
				bendRadius = -box.getWidth() * 0.5


			# Compute bend origin
			bendOriginPoint = Point3( bendBoxCentre.x, bendBoxCentre.y, bendAnchorZ )  -  Vector3( bendRadius, 0.0, 0.0 )


			modelToBendMatrix = bendAxesMatrix  *  Matrix4.translate( -bendOriginPoint.toVector3() )
			bendToModelMatrix = modelToBendMatrix.rigidBodyInverse()


			x0 = MatrixTransformation( modelToBendMatrix )
			x1 = BendTransformation( -bendRadians / bendDepth, bendAnchorToLower, bendAnchorToUpper )
			x2 = MatrixTransformation( bendToModelMatrix )
			x1.chainTo( x2 )
			x0.chainTo( x1 )

			return x0
		else:
			return None
	else:
		return None




class ProcTransformationBend (ProcRotatedTransformation):
	description = _( 'Bend' )

	bend = Field( float, 0.0 )
	direction = Field( float, 0.0 )
	anchor = Field( float, 0.0 )
	lower = Field( float, 0.0 )
	upper = Field( float, 100.0 )


	def procedureInvoke(self, inputObject, errorHandler):
		x0 = makeTransformation( inputObject, self.target, self.xformRotation, self.bend, self.direction, self.anchor, self.lower, self.upper )
		if x0 is not None:
			ProcTransformationBend.transform( inputObject, x0, self.target )
		return inputObject




class TransformationBendAdjustableInvoker (RotatedTransformationAdjustableInvoker):
	functionClass = ProcTransformationBend

	def adjust(self):
		x0 = makeTransformation( self.model, self._proc.target, self._proc.xformRotation, self._proc.bend, self._proc.direction, self._proc.anchor, self._proc.lower, self._proc.upper )
		if x0 is not None:
			self.transform( x0, self._proc.target )




class ToolTransformationBend (ToolRotatedTransformation):
	sheet = SheetRefField( ProcTransformationBend )

	functionClass = ProcTransformationBend
	pageTitle = _( 'Bend' )
	titleText = _( 'Bend' )
	adjustableInvoker = TransformationBendAdjustableInvoker


	bendManip = Real1FieldManipulator( sheet.bend, -3.6e8, 3.6e8, RealManipulator.RealManipulator.FixedScale( 0.25 ), snapping=FieldManipulatorSnappingAngle() )

	rotationManip = RotationFieldManipulator( sheet.xformRotation, ToolRotatedTransformation._focalPoint, snapping=FieldManipulatorSnappingAngle() )

	manipulatorSwitch = FieldManipulatorSwitch( [ bendManip, rotationManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	bendButton = manipulatorSelector.addChoice( bendManip, TransformationToolActions.deformAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, TransformationToolActions.rotationAction )


	rotationLabel = FieldLabelWithFn( sheet.xformRotation, _( 'Deformation rotation:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.xformRotation )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.xformRotation, _( 'Set rotation' ) )

	bendLabel = FieldLabelWithFn( sheet.bend, _( 'Bend:' ) )
	bendEntry = FloatFieldEditEntry( sheet.bend, -3.6e8, 3.6e8, 0.01, 0.1, 4 )

	directionLabel = FieldLabelWithFn( sheet.direction, _( 'Direction:' ) )
	directionEntry = FloatFieldEditEntry( sheet.direction, 0.0, 360.0, 0.01, 1.0, 4 )
	directionSlider = FloatFieldEditSlider( sheet.direction, 0.0, 360.0, 0.01, 1.0 )

	anchorLabel = FieldLabelWithFn( sheet.anchor, _( 'Anchor (%%):' ) )
	anchorEntry = FloatFieldEditEntry( sheet.anchor, 0.0, 100.0, 0.01, 1.0, 4 )
	anchorSlider = FloatFieldEditSlider( sheet.anchor, 0.0, 100.0, 0.01, 1.0 )

	lowerLabel = FieldLabelWithFn( sheet.lower, _( 'Lower (%%):' ) )
	lowerEntry = FloatFieldEditEntry( sheet.lower, 0.0, 100.0, 0.01, 1.0, 4 )
	upperLabel = FieldLabelWithFn( sheet.upper, _( 'Upper (%%):' ) )
	upperEntry = FloatFieldEditEntry( sheet.upper, 0.0, 100.0, 0.01, 1.0, 4 )

	lowerSlider = FloatFieldEditSlider( sheet.lower, 0.0, 100.0, 0.01, 1.0 )
	upperSlider = FloatFieldEditSlider( sheet.upper, 0.0, 100.0, 0.01, 1.0 )

	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Mouse function:' )
	layout.row()  <<  bendButton
	layout.row()  <<  rotationButton
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  rotationPopup.popup
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  bendLabel.label  <<  bendEntry.entry
	layout.row()
	layout.row()  <<  directionLabel.label  <<  directionEntry.entry
	layout.row()  <<  directionSlider.slider
	layout.row()
	layout.row()  <<  anchorLabel.label  <<  anchorEntry.entry
	layout.row()  <<  anchorSlider.slider
	layout.row()
	layout.row()  <<  lowerLabel.label  <<  lowerEntry.entry
	layout.row()  <<  lowerSlider.slider
	layout.row()  <<  upperLabel.label  <<  upperEntry.entry
	layout.row()  <<  upperSlider.slider
	layout.row()
	layout.finalise()

