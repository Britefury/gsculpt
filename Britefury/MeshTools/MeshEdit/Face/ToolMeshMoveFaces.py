##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import *

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import GSProductMesh, MeshTransformation

from Britefury.MeshTools.MeshEdit.ToolMeshEditMove import *




class ProcMeshMoveFaces (ProcMeshEditMove):
	description = _( 'Move faces' )

	def procedureInvoke(self, inputObject, errorHandler):
		transformation = MeshTransformation()
		inputObject.initFaceTransformation( transformation, False )
		transformation.translate( self.translation )
		return inputObject



class MoveFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshMoveFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.transformation = MeshTransformation()
		self.mesh.initFaceTransformation( self.transformation, False )

	def adjust(self):
		self.transformation.translate( self._proc.translation )

	def getResult(self):
		return self.mesh




class ToolMeshMoveFaces (ToolMeshEditMove):
	sheet = SheetRefField( ProcMeshMoveFaces )

	functionClass = ProcMeshMoveFaces
	adjustableInvoker = MoveFacesAdjustableInvoker
	pageTitle = _( 'Move' )
	titleText = _( 'Move Faces' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return self.inputProduct.getMarkedFacesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	translationManip = Translation3FieldManipulator3d( sheet.translation, focalPoint, snapping=FieldManipulatorSnappingDistance() )
	translationManip.enable()
