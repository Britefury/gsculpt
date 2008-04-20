##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

import math

from Britefury.Math.Math import Vector3, Matrix4, Vector2, Point3

from Britefury.UI.FormLayout import FormLayoutFactory
from Britefury.UI.UIAction import UIAction

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *
from Britefury.Manipulator.FieldManipulators import *

from Britefury.TransformationTools.ToolTransformation import *
from Britefury.TransformationTools.ToolTransformationBase import *
from Britefury.TransformationTools.ToolRotatedTransformation import *
from Britefury.TransformationTools import TransformationToolActions
from Britefury.Transformation.Transformation import ShrinkTransformation, MatrixTransformation




def makeTransformation(model, target, xformRotation, shrink, anchor, lower, upper):
	centre = model.getBBox( target == TransformationTarget.POINTS ).getCentre()
	lower *= 0.01
	upper *= 0.01
	anchor *= 0.01

	rotationMatrix = Matrix4.rotate( xformRotation )
	modelToXformFrameMatrix = Matrix4.translate( -centre.toVector3() )  *  rotationMatrix.rigidBodyInverse()
	xformToModelFrameMatrix = modelToXformFrameMatrix.rigidBodyInverse()

	box = model.getBBox( modelToXformFrameMatrix, target == TransformationTarget.POINTS )
	lz = box.getLower().z
	uz = box.getUpper().z
	lowerZ = lz  +  ( uz - lz )  *  lower
	upperZ = lz  +  ( uz - lz )  *  upper
	anchorZ = lz  +  ( uz - lz )  *  anchor

	x0 = MatrixTransformation( modelToXformFrameMatrix )
	x1 = ShrinkTransformation( lowerZ, upperZ, anchorZ, shrink.x, shrink.y )
	x2 = MatrixTransformation( xformToModelFrameMatrix )
	x1.chainTo( x2 )
	x0.chainTo( x1 )

	return x0




class ProcTransformationShrink (ProcRotatedTransformation):
	description = _( 'Shrink' )

	shrink = Field( Vector2, Vector2( 1.0, 1.0 ) )
	anchor = Field( float, 50.0 )
	lower = Field( float, 0.0 )
	upper = Field( float, 100.0 )


	def procedureInvoke(self, inputObject, errorHandler):
		x0 = makeTransformation( inputObject, self.target, self.xformRotation, self.shrink, self.anchor, self.lower, self.upper )
		ProcTransformationShrink.transform( inputObject, x0, self.target )
		return inputObject




class TransformationShrinkAdjustableInvoker (RotatedTransformationAdjustableInvoker):
	functionClass = ProcTransformationShrink

	def adjust(self):
		x0 = makeTransformation( self.model, self._proc.target, self._proc.xformRotation, self._proc.shrink, self._proc.anchor, self._proc.lower, self._proc.upper )
		self.transform( x0, self._proc.target )




class ToolTransformationShrink (ToolRotatedTransformation):
	sheet = SheetRefField( ProcTransformationShrink )

	functionClass = ProcTransformationShrink
	pageTitle = _( 'Shrink' )
	titleText = _( 'Shrink' )
	adjustableInvoker = TransformationShrinkAdjustableInvoker


	def _p_shrinkFocalPoint(self):
		if self.sheet is not None:
			centre = self.inputProduct.getBBox( self.sheet.target == TransformationTarget.POINTS ).getCentre()

			rotationMatrix = Matrix4.rotate( self.sheet.xformRotation )
			modelToXformFrameMatrix = Matrix4.translate( -centre.toVector3() )  *  rotationMatrix.rigidBodyInverse()
			xformToModelFrameMatrix = modelToXformFrameMatrix.rigidBodyInverse()

			box = self.inputProduct.getBBox( modelToXformFrameMatrix, self.sheet.target == TransformationTarget.POINTS )
			lz = box.getLower().z
			uz = box.getUpper().z
			anchorZ = lz  +  ( uz - lz ) * self.sheet.anchor * 0.01
			return Point3( 0.0, 0.0, anchorZ )  *  xformToModelFrameMatrix
		else:
			return Point3()


	_shrinkFocalPoint = FunctionField( _p_shrinkFocalPoint )



	shrinkManip = Scale2FieldManipulator3d( sheet.shrink, _shrinkFocalPoint, sheet.xformRotation, snapping=FieldManipulatorSnappingFraction() )

	rotationManip = RotationFieldManipulator( sheet.xformRotation, ToolRotatedTransformation._focalPoint, snapping=FieldManipulatorSnappingAngle() )

	manipulatorSwitch = FieldManipulatorSwitch( [ shrinkManip, rotationManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	shrinkButton = manipulatorSelector.addChoice( shrinkManip, TransformationToolActions.deformAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, TransformationToolActions.rotationAction )


	rotationLabel = FieldLabelWithFn( sheet.xformRotation, _( 'Deformation rotation:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.xformRotation )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.xformRotation, _( 'Set rotation' ) )

	shrinkLabel = FieldLabelWithFn( sheet.shrink, _( 'Shrink:' ) )
	shrinkEntry = Vector2FieldEditEntry( sheet.shrink, -1.0e8, 1.0e8, 0.01, 0.1, 4 )

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
	layout.row()  <<  shrinkButton
	layout.row()  <<  rotationButton
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  rotationPopup.popup
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  shrinkLabel.label
	layout.row()  <<  'U:'  <<  shrinkEntry.x
	layout.row()  <<  'V:'  <<  shrinkEntry.y
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

