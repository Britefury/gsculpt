##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshExpandFacesAdjuster, GSProductMesh





class ProcMeshExpandFaces (GSProcedure):
	description = _( 'Expand faces' )

	expand = Field( float, 0.0 )
	extrude = Field( float, 0.0 )
	bMarkBoundaryEdges = Field( bool, False )
	bMarkExpandEdges = Field( bool, False )
	bNSharpBoundaryEdges = Field( bool, True )
	bNSharpExpandEdges = Field( bool, False )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshExpandFacesAdjuster()
		inputObject.expandMarkedFaces( adjuster, self.bMarkBoundaryEdges, self.bMarkExpandEdges, self.bNSharpBoundaryEdges, self.bNSharpExpandEdges )
		adjuster.setParameters( self.expand, self.extrude )
		return inputObject



class ExpandFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshExpandFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshExpandFacesAdjuster()
		self.mesh.expandMarkedFaces( self.adjuster, self._proc.bMarkBoundaryEdges, self._proc.bMarkExpandEdges, self._proc.bNSharpBoundaryEdges, self._proc.bNSharpExpandEdges )

	def adjust(self):
		self.adjuster.setParameters( self._proc.expand, self._proc.extrude )

	def getResult(self):
		return self.mesh




class ToolMeshExpandFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshExpandFaces )

	functionClass = ProcMeshExpandFaces
	adjustableInvoker = ExpandFacesAdjustableInvoker
	pageTitle = _( 'Expand' )
	titleText = _( 'Expand Faces' )


	def _p_focalPoint(self):
		product = self.inputProduct
		if product is not None:
			return product.getMarkedFacesCentre()
		else:
			return None

	focalPoint = FunctionField( _p_focalPoint )

	expandLabel = FieldLabelWithFn( sheet.expand, _( 'Expand:' ) )
	expandEntry = FloatFieldEditEntry( sheet.expand, 0.0, 1.0e8, 0.1, 10.0, 4 )
	extrudeLabel = FieldLabelWithFn( sheet.extrude, _( 'Extrude:' ) )
	extrudeEntry = FloatFieldEditEntry( sheet.extrude, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	bMarkBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkBoundaryEdges, _( 'Mark boundary edges' ) )
	bMarkExpandEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkExpandEdges, _( 'Mark expand edges' ) )
	bNSharpBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpBoundaryEdges, _( 'Normal sharpen boundary edges' ) )
	bNSharpExpandEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpExpandEdges, _( 'Normal sharpen expand edges' ) )

	expandManip = Real2FieldManipulator( sheet.expand, 0.0, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ),
									sheet.extrude, -1.0e8, 1.0e8, RealFieldManipulator.WorldSpaceScaleFactory( focalPoint ),
									xSnapping=FieldManipulatorSnappingDistance(), ySnapping=FieldManipulatorSnappingDistance() )
	expandManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  expandLabel.label  <<  expandEntry.entry
	layout.row()  <<  extrudeLabel.label  <<  extrudeEntry.entry
	layout.row()
	layout.row()  <<  bMarkBoundaryEdgesCheck.check
	layout.row()  <<  bMarkExpandEdgesCheck.check
	layout.row()  <<  bNSharpBoundaryEdgesCheck.check
	layout.row()  <<  bNSharpExpandEdgesCheck.check
	layout.row()
	layout.finalise()
