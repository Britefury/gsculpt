##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Quaternion, Matrix4

from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.Kernel.Enum import Enum

from Britefury.Cell.Cell import *

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model import Model
from Britefury.Model.Model import GSProductModel




# TransformationTarget enumeration
class TransformationTarget (Enum):
	BODY = 0
	POINTS = 1




# XML IO for TransformationTarget

xformTargetIOXmlEnum = IOXmlEnum( TransformationTarget, 'TransformationTarget', TransformationTarget.BODY )
xformTargetIOXmlEnum.value( TransformationTarget.BODY, 'body' )
xformTargetIOXmlEnum.value( TransformationTarget.POINTS, 'points' )

ioXmlReadTransformationTargetProp = xformTargetIOXmlEnum.propReader
ioXmlReadTransformationTargetNode = xformTargetIOXmlEnum.nodeReader
ioXmlWriteTransformationTargetProp = xformTargetIOXmlEnum.propWriter
ioXmlWriteTransformationTargetNode = xformTargetIOXmlEnum.nodeWriter



# Cell for TransformationTarget

class TransformationTargetCell (Cell):
	valueClass = TransformationTarget
	bAllowNone = False

registerCellType( TransformationTarget, TransformationTargetCell )




class ProcTransformationBase (GSProcedure):
	description = _( 'Abstract transformation base' )

	target = Field( TransformationTarget, TransformationTarget.BODY )

	inputClass = GSProductModel


	@staticmethod
	def transform(inputObject, matrixOrTransformation, target):
		inputObject.transform( matrixOrTransformation, target == TransformationTarget.BODY, False, target == TransformationTarget.POINTS )

	@staticmethod
	def transformAboutCentre(inputObject, matrix, target):
		centre = inputObject.getBBox( target == TransformationTarget.POINTS ).getCentre()
		matrix = Matrix4.translate( -centre.toVector3() )  *  matrix  *  Matrix4.translate( centre.toVector3() )
		inputObject.transform( matrix, target == TransformationTarget.BODY, False, target == TransformationTarget.POINTS )




class TransformationBaseAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcTransformationBase

	def main(self):
		self.model = deepcopy( self._inputObject )


	def restore(self):
		self.model.transformRestore()


	def getResult(self):
		return self.model


	def transform(self, matrixOrTransformation, target):
		self.model.transform( matrixOrTransformation, target == TransformationTarget.BODY, True, target == TransformationTarget.POINTS )

	def transformAboutCentre(self, matrix, target):
		centre = self.model.getBBox( target == TransformationTarget.POINTS ).getCentre()
		matrix = Matrix4.translate( -centre.toVector3() )  *  matrix  *  Matrix4.translate( centre.toVector3() )
		self.model.transform( matrix, target == TransformationTarget.BODY, True, target == TransformationTarget.POINTS )




class ToolTransformationBase (GSProcedureTool):
	sheet = SheetRefField( ProcTransformationBase )

	functionClass = ProcTransformationBase
	pageTitle = _( 'Transformation' )
	titleText = _( 'Abstract Transformation' )
	adjustableInvoker = TransformationBaseAdjustableInvoker


	def _p_focalPoint(self):
		return self.inputProduct.getBBox( self.sheet.target ).getCentre()

	_focalPoint = FunctionField( _p_focalPoint )

