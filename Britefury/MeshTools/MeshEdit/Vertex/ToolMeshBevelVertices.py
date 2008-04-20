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
from Britefury.Mesh.Mesh import MeshBevelVerticesAdjuster, GSProductMesh




class ProcMeshBevelVertices (GSProcedure):
	description = _( 'Bevel vertices' )

	bevel = Field( float, 0.0 )
	bUseExistingTexturing = Field( bool, True )
	bMarkBoundaryEdges = Field( bool, False )
	bNSharpBoundaryEdges = Field( bool, True )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshBevelVerticesAdjuster()
		inputObject.bevelMarkedVertices( adjuster, self.bUseExistingTexturing, self.bMarkBoundaryEdges, self.bNSharpBoundaryEdges )
		adjuster.setParameters( self.bevel )
		return inputObject



class BevelVerticesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshBevelVertices

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshBevelVerticesAdjuster()
		self.mesh.bevelMarkedVertices( self.adjuster, self._proc.bUseExistingTexturing, self._proc.bMarkBoundaryEdges, self._proc.bNSharpBoundaryEdges )

	def adjust(self):
		self.adjuster.setParameters( self._proc.bevel )

	def getResult(self):
		return self.mesh




class ToolMeshBevelVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshBevelVertices )

	functionClass = ProcMeshBevelVertices
	adjustableInvoker = BevelVerticesAdjustableInvoker
	pageTitle = _( 'Bevel' )
	titleText = _( 'Bevel Vertices' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedVerticesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	bevelLabel = FieldLabelWithFn( sheet.bevel, _( 'Bevel:' ) )
	bevelEntry = FloatFieldEditEntry( sheet.bevel, 0.0, 1.0e8, 0.1, 10.0, 4 )
	bUseExistingTexturingCheck = BoolFieldEditCheckWithFn( sheet.bUseExistingTexturing, _( 'Use existing texturing' ) )
	bMarkBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkBoundaryEdges, _( 'Mark boundary edges' ) )
	bNSharpBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpBoundaryEdges, _( 'N-sharp boundary edges' ) )

	bevelManip = Real1FieldManipulator( sheet.bevel, 0.0, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ), snapping=FieldManipulatorSnappingDistance() )
	bevelManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  bevelLabel.label  <<  bevelEntry.entry
	layout.row()
	layout.row()  <<  bUseExistingTexturingCheck.check
	layout.row()  <<  bMarkBoundaryEdgesCheck.check
	layout.row()  <<  bNSharpBoundaryEdgesCheck.check
	layout.row()
	layout.finalise()
