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
from Britefury.Transformation.Transformation import MatrixTransformation




def makeTransformation(model, target, xformRotation, shear):
	rotationMatrix = Matrix4.rotate( xformRotation )
	modelToXformFrameMatrix = rotationMatrix.rigidBodyInverse()
	xformToModelFrameMatrix = rotationMatrix

	box = model.getBBox( modelToXformFrameMatrix, target == TransformationTarget.POINTS )
	shear = shear  *  ( 1.0 / box.getDepth() )
	transformationMatrix = modelToXformFrameMatrix * Matrix4.translate( -box.getLower().toVector3() ) * Matrix4.shearZ( shear.x, shear.y ) *			\
					Matrix4.translate( box.getLower().toVector3() ) * xformToModelFrameMatrix

	return transformationMatrix




class ProcTransformationShear (ProcRotatedTransformation):
	description = _( 'Shear' )

	shear = Field( Vector2, Vector2( 0.0, 0.0 ) )


	def procedureInvoke(self, inputObject, errorHandler):
		x0 = makeTransformation( inputObject, self.target, self.xformRotation, self.shear )
		ProcTransformationShear.transform( inputObject, x0, self.target )
		return inputObject




class TransformationShearAdjustableInvoker (RotatedTransformationAdjustableInvoker):
	functionClass = ProcTransformationShear

	def adjust(self):
		x0 = makeTransformation( self.model, self._proc.target, self._proc.xformRotation, self._proc.shear )
		self.transform( x0, self._proc.target )




class ToolTransformationShear (ToolRotatedTransformation):
	sheet = SheetRefField( ProcTransformationShear )

	functionClass = ProcTransformationShear
	pageTitle = _( 'Shear' )
	titleText = _( 'Shear' )
	adjustableInvoker = TransformationShearAdjustableInvoker


	def _p_shearFocalPoint(self):
		if self.sheet is not None:
			centre = self.inputProduct.getBBox( self.sheet.target == TransformationTarget.POINTS ).getCentre()

			rotationMatrix = Matrix4.rotate( self.sheet.xformRotation )
			modelToXformFrameMatrix = Matrix4.translate( -centre.toVector3() )  *  rotationMatrix.rigidBodyInverse()
			xformToModelFrameMatrix = modelToXformFrameMatrix.rigidBodyInverse()

			box = self.inputProduct.getBBox( modelToXformFrameMatrix, self.sheet.target == TransformationTarget.POINTS )
			return Point3( 0.0, 0.0, box.getUpper().z )  *  xformToModelFrameMatrix
		else:
			return Point3()


	_shearFocalPoint = FunctionField( _p_shearFocalPoint )



	shearManip = Translation2FieldManipulator3d( sheet.shear, _shearFocalPoint, sheet.xformRotation, snapping=FieldManipulatorSnappingDistance() )

	rotationManip = RotationFieldManipulator( sheet.xformRotation, ToolRotatedTransformation._focalPoint, snapping=FieldManipulatorSnappingAngle() )

	manipulatorSwitch = FieldManipulatorSwitch( [ shearManip, rotationManip ] )
	manipulatorSwitch.enable()

	manipulatorSelector = FieldManipulatorSwitchRadioGroup( manipulatorSwitch )
	shearButton = manipulatorSelector.addChoice( shearManip, TransformationToolActions.deformAction )
	rotationButton = manipulatorSelector.addChoice( rotationManip, TransformationToolActions.rotationAction )


	rotationLabel = FieldLabelWithFn( sheet.xformRotation, _( 'Deformation rotation:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.xformRotation )
	rotationPopup = QuaternionFieldEditDirectionPopup( sheet.xformRotation, _( 'Set rotation' ) )

	shearLabel = FieldLabelWithFn( sheet.shear, _( 'Shear:' ) )
	shearEntry = Vector2FieldEditEntry( sheet.shear, -1.0e18, 1.0e18, 0.1, 10.0, 4 )




	layout = FormLayoutFactory()
	layout.row()  <<  _( 'Mouse function:' )
	layout.row()  <<  shearButton
	layout.row()  <<  rotationButton
	layout.row()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  rotationPopup.popup
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.row()  <<  shearLabel.label
	layout.row()  <<  'U:'  <<  shearEntry.x
	layout.row()  <<  'V:'  <<  shearEntry.y
	layout.row()
	layout.finalise()

