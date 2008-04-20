##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Vector2, Vector3

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshExtrudeEdgesAdjuster, GSProductMesh





class ProcMeshExtrudeEdges (GSProcedure):
	__version__ = '1'
	description = _( 'Extrude edges' )

	extrude = Field( Vector3, Vector3() )
	segments = Field( int, 1 )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshExtrudeEdgesAdjuster()
		inputObject.extrudeMarkedEdges( adjuster, self.segments, False, self.getMarkExtrudedEdges() )
		adjuster.setParameters( self.extrude, Vector2( 0.0, 1.0 ) )
		return inputObject


	def getMarkExtrudedEdges(self):
		try:
			v = self.__version__
		except AttributeError:
			v = None

		if v is None:
			return False
		elif v == '1':
			return True
		else:
			return True


class ExtrudeEdgesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshExtrudeEdges

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		self.adjuster = MeshExtrudeEdgesAdjuster()
		self.mesh.extrudeMarkedEdges( self.adjuster, self._proc.segments, False, self._proc.getMarkExtrudedEdges() )

	def adjust(self):
		self.adjuster.setParameters( self._proc.extrude, Vector2( 0.0, 1.0 ) )

	def getResult(self):
		return self.mesh




class ToolMeshExtrudeEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshExtrudeEdges )

	functionClass = ProcMeshExtrudeEdges
	adjustableInvoker = ExtrudeEdgesAdjustableInvoker
	pageTitle = _( 'Extrude' )
	titleText = _( 'Extrude Edges' )


	def _p_focalPoint(self):
		return self.inputProduct.getMarkedEdgesCentre()

	focalPoint = FunctionField( _p_focalPoint )

	extrudeLabel = FieldLabelWithFn( sheet.extrude, _( 'Extrude:' ) )
	extrudeEntry = Vector3FieldEditEntry( sheet.extrude, -1.0e8, 1.0e8, 0.1, 10.0, 4 )
	segmentsLabel = FieldLabelWithFn( sheet.segments, _( 'Segments:' ) )
	segmentsEntry = IntFieldEditEntry( sheet.segments, 1, 1048576, 1, 10 )


	extrudeManip = Translation3FieldManipulator3d( sheet.extrude, focalPoint, snapping=FieldManipulatorSnappingDistance() )
	extrudeManip.enable()

	numSegmentsManip = IntFieldScrollManipulator( sheet.segments, 1, 1048576 )
	numSegmentsManip.enable()

	layout = FormLayoutFactory()
	layout.textBlock( _( 'Use the scroll wheel to change the number of segments' ) )
	layout.row()
	layout.row()  <<  extrudeLabel.label
	layout.row()  <<  'X:'  <<  extrudeEntry.x
	layout.row()  <<  'Y:'  <<  extrudeEntry.y
	layout.row()  <<  'Z:'  <<  extrudeEntry.z
	layout.row()
	layout.row()  <<  segmentsLabel.label  <<  segmentsEntry.entry
	layout.row()

	layout.finalise()
