##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector3

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh.Mesh import MeshVectorExtrudeFacesAdjuster, GSProductMesh




class ProcMeshFreeExtrudeFaces (GSProcedure):
	description = _( 'Free extrude faces' )

	extrusion = Field( Vector3, Vector3() )
	segments = Field( int, 1 )
	bMarkBoundaryEdges = Field( bool, False )
	bMarkExtrudedEdges = Field( bool, False )
	bNSharpBoundaryEdges = Field( bool, True )
	bNSharpExtrudedEdges = Field( bool, True )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshVectorExtrudeFacesAdjuster()
		inputObject.vectorExtrudeMarkedFaces( adjuster, self.segments, self.bMarkBoundaryEdges, self.bMarkExtrudedEdges, self.bNSharpBoundaryEdges, self.bNSharpExtrudedEdges )
		adjuster.setParameters( self.extrusion )
		return inputObject



class FreeExtrudeFacesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshFreeExtrudeFaces

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshVectorExtrudeFacesAdjuster()
		self.mesh.vectorExtrudeMarkedFaces( self.adjuster, self._proc.segments, self._proc.bMarkBoundaryEdges, self._proc.bMarkExtrudedEdges, self._proc.bNSharpBoundaryEdges, self._proc.bNSharpExtrudedEdges )

	def adjust(self):
		self.adjuster.setParameters( self._proc.extrusion )

	def getResult(self):
		return self.mesh




class ToolMeshFreeExtrudeFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshFreeExtrudeFaces )

	functionClass = ProcMeshFreeExtrudeFaces
	adjustableInvoker = FreeExtrudeFacesAdjustableInvoker
	pageTitle = _( 'F.Extr.' )
	titleText = _( 'Free Extrude Faces' )


	def _p_focalPoint(self):
		return self.inputProduct.getMarkedFacesCentre()

	focalPoint = FunctionField( _p_focalPoint )

	extrusionLabel = FieldLabelWithFn( sheet.extrusion, _( 'Extrusion:' ) )
	extrusionEntry =Vector3FieldEditEntry( sheet.extrusion, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	segmentsLabel = FieldLabelWithFn( sheet.segments, _( 'Segments:' ) )
	segmentsEntry = IntFieldEditEntry( sheet.segments, 1, 1048576, 1, 10 )
	bMarkBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkBoundaryEdges, _( 'Mark boundary edges' ) )
	bMarkExtrudedEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkExtrudedEdges, _( 'Mark extruded edges' ) )
	bNSharpBoundaryEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpBoundaryEdges, _( 'N-sharp boundary edges' ) )
	bNSharpExtrudedEdgesCheck = BoolFieldEditCheckWithFn( sheet.bNSharpExtrudedEdges, _( 'N-sharp extruded edges' ) )

	extrudeManip = Translation3FieldManipulator3d( sheet.extrusion, focalPoint, snapping=FieldManipulatorSnappingDistance() )
	extrudeManip.enable()

	numSegmentsManip = IntFieldScrollManipulator( sheet.segments, 1, 1048576 )
	numSegmentsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of segments' ) )
	layout.row()
	layout.row()  <<  extrusionLabel.label
	layout.row()  <<  'X:'  <<  extrusionEntry.x
	layout.row()  <<  'Y:'  <<  extrusionEntry.y
	layout.row()  <<  'Z:'  <<  extrusionEntry.z
	layout.row()
	layout.row()  <<  segmentsLabel.label  <<  segmentsEntry.entry
	layout.row()
	layout.row()  <<  bMarkBoundaryEdgesCheck.check
	layout.row()  <<  bMarkExtrudedEdgesCheck.check
	layout.row()  <<  bNSharpBoundaryEdgesCheck.check
	layout.row()  <<  bNSharpExtrudedEdgesCheck.check
	layout.row()
	layout.finalise()
