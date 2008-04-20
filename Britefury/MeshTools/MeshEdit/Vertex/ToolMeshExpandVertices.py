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
from Britefury.Mesh.Mesh import MeshExpandVerticesAdjuster, MeshExpandVerticesInMarkedFacesAdjuster, GSProductMesh



class ProcMeshExpandVertices (GSProcedure):
	description = _( 'Expand vertices' )

	bMarkedFacesOnly = Field( bool, False )
	expand = Field( float, 0.0 )
	extrude = Field( float, 0.0 )
	bMarkBoundaryEdges = Field( bool, False )
	bNSharpBoundaryEdges = Field( bool, True )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		if self.bMarkedFacesOnly:
			adjuster = MeshExpandVerticesInMarkedFacesAdjuster()
			inputObject.expandMarkedVerticesInMarkedFaces( adjuster, self.bMarkBoundaryEdges )
			adjuster.setParameters( self.expand )
		else:
			adjuster = MeshExpandVerticesAdjuster()
			inputObject.expandMarkedVertices( adjuster, self.bMarkBoundaryEdges, self.bNSharpBoundaryEdges )
			adjuster.setParameters( self.expand, self.extrude )
		return inputObject



class ExpandVerticesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshExpandVertices

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		if self._proc.bMarkedFacesOnly:
			self.adjuster = MeshExpandVerticesInMarkedFacesAdjuster()
			self.mesh.expandMarkedVerticesInMarkedFaces( self.adjuster, self._proc.bMarkBoundaryEdges )
		else:
			self.adjuster = MeshExpandVerticesAdjuster()
			self.mesh.expandMarkedVertices( self.adjuster, self._proc.bMarkBoundaryEdges, self._proc.bNSharpBoundaryEdges )

	def adjust(self):
		if self._proc.bMarkedFacesOnly:
			self.adjuster.setParameters( self._proc.expand )
		else:
			self.adjuster.setParameters( self._proc.expand, self._proc.extrude )

	def getResult(self):
		return self.mesh




class ToolMeshExpandVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshExpandVertices )

	functionClass = ProcMeshExpandVertices
	adjustableInvoker = ExpandVerticesAdjustableInvoker
	pageTitle = _( 'Expand' )
	titleText = _( 'Expand Vertices' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedVerticesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	bMarkedFacesCheck = BoolFieldEditCheckWithFn( sheet.bMarkedFacesOnly, _( 'Marked faces only' ) )
	expandLabel = FieldLabelWithFn( sheet.expand, _( 'Expand:' ) )
	expandEntry = FloatFieldEditEntry( sheet.expand, 0.0, 1.0e8, 0.1, 10.0, 4 )
	extrudeLabel = FieldLabelWithFn( sheet.extrude, _( 'Extrude:' ) )
	extrudeEntry = FloatFieldEditEntry( sheet.extrude, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	bMarkBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkBoundaryEdges, _( 'Mark boundary edges' ) )
	bNSharpBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpBoundaryEdges, _( 'N-sharp boundary edges' ) )

	expandManip = Real2FieldManipulator( sheet.expand, 0.0, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ),
									sheet.extrude, -1.0e8, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ),
									xSnapping=FieldManipulatorSnappingDistance(), ySnapping=FieldManipulatorSnappingDistance() )
	expandManip.enable()


	def _p_notMarkedFacesOnly(self):
		if self.sheet is not None:
			return not self.sheet.bMarkedFacesOnly
		else:
			return True

	notMarkedFacesOnly = FunctionField( _p_notMarkedFacesOnly )

	extrudeEntry.sensitivityField = notMarkedFacesOnly
	bNSharpBoundaryEdgesCheck.sensitivityField = notMarkedFacesOnly

	layout = FormLayoutFactory()
	layout.row()  <<  bMarkedFacesCheck.check
	layout.row()  <<  expandLabel.label  <<  expandEntry.entry
	layout.row()  <<  extrudeLabel.label  <<  extrudeEntry.entry
	layout.row()
	layout.row()  <<  bMarkBoundaryEdgesCheck.check
	layout.row()  <<  bNSharpBoundaryEdgesCheck.check
	layout.row()
	layout.finalise()
