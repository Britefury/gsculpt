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





class ProcTransformationScale (ProcTransformation):
	__version__ = '1'
	description = _( 'Scale' )

	scale = Field( Vector3, Vector3( 1.0, 1.0, 1.0 ) )


	def procedureInvoke(self, inputObject, errorHandler):
		if self._f_isPivotCentre():
			self.transformAboutCentre( inputObject, Matrix4.scale( self.scale ), self.target )
		else:
			self.transform( inputObject, Matrix4.scale( self.scale ), self.target )
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




class TransformationScaleAdjustableInvoker (TransformationAdjustableInvoker):
	functionClass = ProcTransformationScale

	def adjust(self):
		if self._proc._f_isPivotCentre():
			self.transformAboutCentre( Matrix4.scale( self._proc.scale ), self._proc.target )
		else:
			self.transform( Matrix4.scale( self._proc.scale ), self._proc.target )




class ToolTransformationScale (ToolTransformation):
	sheet = SheetRefField( ProcTransformationScale )

	functionClass = ProcTransformationScale
	pageTitle = _( 'Scale' )
	titleText = _( 'Scale' )
	adjustableInvoker = TransformationScaleAdjustableInvoker


	scaleManip = Scale3FieldManipulator3d( sheet.scale, ToolTransformationBase._focalPoint, snapping=FieldManipulatorSnappingFraction() )
	scaleManip.enable()


	scaleLabel = FieldLabelWithFn( sheet.scale, _( 'Scale:' ) )
	scaleEntry = Vector3FieldEditEntry( sheet.scale, -1.0e8, 1.0e8, 0.01, 0.1, 4 )

	layout = FormLayoutFactory()
	layout.row()  <<  scaleLabel.label
	layout.row()  <<  'X:'  <<  scaleEntry.x
	layout.row()  <<  'Y:'  <<  scaleEntry.y
	layout.row()  <<  'Z:'  <<  scaleEntry.z
	layout.row()
	layout.finalise()

