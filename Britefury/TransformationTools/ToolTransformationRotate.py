##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Quaternion, Matrix4

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *
from Britefury.Manipulator.FieldManipulators import *

from Britefury.TransformationTools.ToolTransformation import *





class ProcTransformationRotate (ProcTransformation):
	__version__ = '1'
	description = _( 'Rotate' )

	rotation = Field( Quaternion, Quaternion() )


	def procedureInvoke(self, inputObject, errorHandler):
		if self._f_isPivotCentre():
			self.transformAboutCentre( inputObject, Matrix4.rotate( self.rotation ), self.target )
		else:
			self.transform( inputObject, Matrix4.rotate( self.rotation ), self.target )
		return inputObject


	# Old versions scaled about the origin, should be centre
	def _f_isPivotCentre(self):
		try:
			v = self.__version__
		except AttributeError:
			return False

		if v is None:
			return False
		elif v == '1':
			return True




class TransformationRotateAdjustableInvoker (TransformationAdjustableInvoker):
	functionClass = ProcTransformationRotate

	def adjust(self):
		if self._proc._f_isPivotCentre():
			self.transformAboutCentre( Matrix4.rotate( self._proc.rotation ), self._proc.target )
		else:
			self.transform( Matrix4.rotate( self._proc.rotation ), self._proc.target )




class ToolTransformationRotate (ToolTransformation):
	sheet = SheetRefField( ProcTransformationRotate )

	functionClass = ProcTransformationRotate
	pageTitle = _( 'Rotate' )
	titleText = _( 'Rotate' )
	adjustableInvoker = TransformationRotateAdjustableInvoker


	rotationManip = RotationFieldManipulator( sheet.rotation, ToolTransformationBase._focalPoint, snapping=FieldManipulatorSnappingAngle() )
	rotationManip.enable()


	rotationLabel = FieldLabelWithFn( sheet.rotation, _( 'Rotation:' ) )
	rotationEntry = QuaternionFieldEditEntry( sheet.rotation )

	layout = FormLayoutFactory()
	layout.row()  <<  rotationLabel.label
	layout.row()  <<  'X:'  <<  rotationEntry.x
	layout.row()  <<  'Y:'  <<  rotationEntry.y
	layout.row()  <<  'Z:'  <<  rotationEntry.z
	layout.row()
	layout.finalise()

