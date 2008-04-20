##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3, Matrix4, Quaternion

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import *

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh, MeshTransformation

from Britefury.MeshTools.MeshEdit.ToolMeshEditRotate import *




class ProcMeshRotateFaces (ProcMeshEditRotate):
	description = _( 'Rotate faces' )

	def procedureInvoke(self, inputObject, errorHandler):
		transformation = MeshTransformation()
		inputObject.initFaceTransformation( transformation, self.bPerGroup )
		transformation.centredTransform( Matrix4.rotate( self.rotation ) )
		return inputObject



class RotateFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshRotateFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.transformation = MeshTransformation()
		self.mesh.initFaceTransformation( self.transformation, self._proc.bPerGroup )

	def adjust(self):
		self.transformation.centredTransform( Matrix4.rotate( self._proc.rotation ) )

	def getResult(self):
		return self.mesh




class ToolMeshRotateFaces (ToolMeshEditRotate):
	sheet = SheetRefField( ProcMeshRotateFaces )

	functionClass = ProcMeshRotateFaces
	adjustableInvoker = RotateFacesAdjustableInvoker
	pageTitle = _( 'Rotate' )
	titleText = _( 'Rotate Faces' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return self.inputProduct.getMarkedFacesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	rotationManip = RotationFieldManipulator( sheet.rotation, focalPoint, snapping=FieldManipulatorSnappingAngle() )
	rotationManip.enable()
