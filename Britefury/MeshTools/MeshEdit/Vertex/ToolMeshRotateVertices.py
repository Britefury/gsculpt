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




class ProcMeshRotateVertices (ProcMeshEditRotate):
	description = _( 'Rotate vertices' )

	def __init__(self, src=None):
		super( ProcMeshRotateVertices, self ).__init__( src )
		self.bPerGroup = False

	def procedureInvoke(self, inputObject, errorHandler):
		transformation = MeshTransformation()
		inputObject.initVertexTransformation( transformation, self.bPerGroup )
		transformation.centredTransform( Matrix4.rotate( self.rotation ) )
		return inputObject



class RotateVerticesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshRotateVertices

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.transformation = MeshTransformation()
		self.mesh.initVertexTransformation( self.transformation, self._proc.bPerGroup )

	def adjust(self):
		self.transformation.centredTransform( Matrix4.rotate( self._proc.rotation ) )

	def getResult(self):
		return self.mesh




class ToolMeshRotateVertices (ToolMeshEditRotate):
	sheet = SheetRefField( ProcMeshRotateVertices )

	functionClass = ProcMeshRotateVertices
	adjustableInvoker = RotateVerticesAdjustableInvoker
	pageTitle = _( 'Rotate' )
	titleText = _( 'Rotate Vertices' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return self.inputProduct.getMarkedVerticesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	rotationManip = RotationFieldManipulator( sheet.rotation, focalPoint, snapping=FieldManipulatorSnappingAngle() )
	rotationManip.enable()
