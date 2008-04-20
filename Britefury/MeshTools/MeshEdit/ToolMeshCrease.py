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
from Britefury.Manipulator import RealManipulator

from Britefury.MeshTools.MeshEditComponent import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshSharpenFunction, MeshSharpnessBackup, GSProductMesh




class ProcMeshCrease (GSProcedure):
	targetComponent = Field( MeshEditComponent, MeshEditComponent.EDGES )
	crease = Field( float, 0.0 )


	inputClass = GSProductMesh


	def getDescription(self):
		if self.targetComponent == MeshEditComponent.VERTICES:
			return _( 'Crease vertices' )
		elif self.targetComponent == MeshEditComponent.EDGES:
			return _( 'Crease edges' )



	def procedureInvoke(self, inputObject, errorHandler):
		if self.targetComponent == MeshEditComponent.VERTICES:
			inputObject.setMarkedVertexSharpness( self.crease, MeshSharpenFunction.ACCUMULATE )
		elif self.targetComponent == MeshEditComponent.EDGES:
			inputObject.setMarkedEdgeSharpness( self.crease, MeshSharpenFunction.ACCUMULATE )
		return inputObject




class CreaseAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshCrease

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.sharpnessBackup = MeshSharpnessBackup()

	def save(self):
		if self._proc.targetComponent == MeshEditComponent.VERTICES:
			self.mesh.saveMarkedVertexSharpness( self.sharpnessBackup )
		elif self._proc.targetComponent == MeshEditComponent.EDGES:
			self.mesh.saveMarkedEdgeSharpness( self.sharpnessBackup )

	def restore(self):
		if self._proc.targetComponent == MeshEditComponent.VERTICES:
			self.mesh.restoreMarkedVertexSharpness( self.sharpnessBackup )
		elif self._proc.targetComponent == MeshEditComponent.EDGES:
			self.mesh.restoreMarkedEdgeSharpness( self.sharpnessBackup )

	def adjust(self):
		if self._proc.targetComponent == MeshEditComponent.VERTICES:
			self.mesh.setMarkedVertexSharpness( self._proc.crease, MeshSharpenFunction.ACCUMULATE )
		elif self._proc.targetComponent == MeshEditComponent.EDGES:
			self.mesh.setMarkedEdgeSharpness( self._proc.crease, MeshSharpenFunction.ACCUMULATE )

	def getResult(self):
		return self.mesh




class ToolMeshCrease (GSProcedureTool):
	sheet = SheetRefField( ProcMeshCrease )

	functionClass = ProcMeshCrease
	adjustableInvoker = CreaseAdjustableInvoker
	pageTitle = _( 'Crease' )
	titleText = _( 'Crease' )

	creaseLabel = FieldLabelWithFn( sheet.crease,  _( 'Crease:' ) )
	creaseEntry = FloatFieldEditEntry( sheet.crease, -32.0, 32.0, 0.01, 1.1, 4 )

	creaseManip = Real1FieldManipulator( sheet.crease, -32.0, 32.0, RealManipulator.RealManipulator.FixedScale( 0.0025 ), snapping=FieldManipulatorSnappingFraction() )
	creaseManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  creaseLabel.label  <<  creaseEntry.entry
	layout.row()
	layout.textBlock( _( 'Creasing affects subdivision. Enable live subdivision to see the result.' ) )
	layout.row()  <<  ''
	layout.finalise()
