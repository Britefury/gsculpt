##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshMoveNormalAdjuster, GSProductMesh




class ProcMeshMoveVerticesNormal (GSProcedure):
	description = _( 'Move vertices normal' )

	distance = Field( float, 0.0 )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshMoveNormalAdjuster()
		inputObject.moveVerticesNormal( adjuster )
		adjuster.setParameters( self.distance )
		return inputObject



class MoveVerticesNormalAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshMoveVerticesNormal

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshMoveNormalAdjuster()
		self.mesh.moveVerticesNormal( self.adjuster )

	def adjust(self):
		self.adjuster.setParameters( self._proc.distance )

	def getResult(self):
		return self.mesh




class ToolMeshMoveVerticesNormal (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMoveVerticesNormal )

	functionClass = ProcMeshMoveVerticesNormal
	adjustableInvoker = MoveVerticesNormalAdjustableInvoker
	pageTitle = _( 'Mv.Nml.' )
	titleText = _( 'Move Normal' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedVerticesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	distanceLabel = FieldLabelWithFn( sheet.distance, _( 'Distance:' ) )
	distanceEntry = FloatFieldEditEntry( sheet.distance, -1.0e18, 1.0e18, 0.001, 0.1, 4 )

	distanceManip = Real1FieldManipulator( sheet.distance, -1.0e18, 1.0e18, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ), snapping=FieldManipulatorSnappingDistance() )
	distanceManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  distanceLabel.label  <<  distanceEntry.entry
	layout.row()
	layout.finalise()
