##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3, Matrix4

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import *

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh, MeshTransformation

from Britefury.MeshTools.MeshEdit.ToolMeshEditScale import *




class ProcMeshScaleEdges (ProcMeshEditScale):
	description = _( 'Scale edges' )

	def procedureInvoke(self, inputObject, errorHandler):
		transformation = MeshTransformation()
		inputObject.initEdgeTransformation( transformation, self.bPerGroup )
		transformation.centredTransform( Matrix4.scale( self.scale ) )
		return inputObject



class ScaleEdgesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshScaleEdges

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.transformation = MeshTransformation()
		self.mesh.initEdgeTransformation( self.transformation, self._proc.bPerGroup )

	def adjust(self):
		self.transformation.centredTransform( Matrix4.scale( self._proc.scale ) )

	def getResult(self):
		return self.mesh




class ToolMeshScaleEdges (ToolMeshEditScale):
	sheet = SheetRefField( ProcMeshScaleEdges )

	functionClass = ProcMeshScaleEdges
	adjustableInvoker = ScaleEdgesAdjustableInvoker
	pageTitle = _( 'Scale' )
	titleText = _( 'Scale Edges' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return self.inputProduct.getMarkedEdgesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	scaleManip = Scale3FieldManipulator3d( sheet.scale, focalPoint, snapping=FieldManipulatorSnappingFraction() )
	scaleManip.enable()
