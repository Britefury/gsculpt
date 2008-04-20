##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Matrix4, Axis

from Britefury.Model.Model import GSProductModelBlank

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import PlanePrimitive, MeshVertexList, GSProductMesh




class ProcPlanePrimitive (GSProcedure):
	description = _( 'Plane' )

	axis = Field( Axis, Axis.AXIS_Y )
	uSegments = Field( int, 1 )
	vSegments = Field( int, 1 )
	width = Field( float, 100.0 )
	height = Field( float, 100.0 )
	position = Field( Point3, Point3() )


	inputClass = GSProductModelBlank
	outputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		prim = PlanePrimitive( self.axis, self.uSegments, self.vSegments, self.width, self.height )
		mesh = GSProductMesh()
		verts = MeshVertexList()
		prim.createPlane( mesh, verts )
		verts.transform( Matrix4.translate( self.position.toVector3() ), mesh )
		return mesh



class PlaneAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcPlanePrimitive

	def initInvoke(self):
		self.verts = MeshVertexList()

	def main(self):
		prim = PlanePrimitive( self._proc.axis, self._proc.uSegments, self._proc.vSegments, self._proc.width, self._proc.height )
		self.verts.clear()
		self.mesh = GSProductMesh()
		prim.createPlane( self.mesh, self.verts )
		return self.mesh

	def save(self):
		self.verts.savePositions( self.mesh )

	def restore(self):
		self.verts.restorePositions( self.mesh )

	def adjust(self):
		self.verts.transform( Matrix4.translate( self._proc.position.toVector3() ), self.mesh )

	def getResult(self):
		return self.mesh




class ToolPlanePrimitive (GSProcedureTool):
	sheet = SheetRefField( ProcPlanePrimitive )

	functionClass = ProcPlanePrimitive
	adjustableInvoker = PlaneAdjustableInvoker
	pageTitle = _( 'Plane' )
	titleText = _( 'Create Plane' )

	axisLabel = FieldLabelWithFn( sheet.axis, _( 'Plane axis:' ) )
	axisRadio = AxisFieldEditRadioGroup( sheet.axis )
	uSegsLabel = FieldLabelWithFn( sheet.axis, _( 'U-Segs.:' ) )
	uSegsEntry = IntFieldEditEntry( sheet.uSegments, 1, 1048576, 1, 10 )
	vSegsLabel = FieldLabelWithFn( sheet.axis, _( 'V-Segs.:' ) )
	vSegsEntry = IntFieldEditEntry( sheet.vSegments, 1, 1048576, 1, 10 )
	widthLabel = FieldLabelWithFn( sheet.width, _( 'Width:' ) )
	widthEntry = FloatFieldEditEntry( sheet.width, 0.0, 1.0e8, 0.1, 10.0, 4 )
	heightLabel = FieldLabelWithFn( sheet.height, _( 'Height:' ) )
	heightEntry = FloatFieldEditEntry( sheet.height, 0.0, 1.0e8, 1.0, 10.0, 4 )
	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e8, 1.0e8, 1.0, 10.0, 4 )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	positionManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  axisLabel.label
	layout.row()  <<  axisRadio.x  <<  axisRadio.y  <<  axisRadio.z
	layout.row()
	layout.row()  <<  uSegsLabel.label  <<  uSegsEntry.entry
	layout.row()  <<  vSegsLabel.label  <<  vSegsEntry.entry
	layout.row()
	layout.row()  <<  widthLabel.label  <<  widthEntry.entry
	layout.row()  <<  heightLabel.label  <<  heightEntry.entry
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.finalise()
