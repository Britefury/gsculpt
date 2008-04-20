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
from Britefury.Mesh.Mesh import TorusPrimitive, MeshVertexList, GSProductMesh



class ProcTorusPrimitive (GSProcedure):
	description = _( 'Torus' )

	plane = Field( Axis, Axis.AXIS_Y )
	majorSegments = Field( int, 20 )
	minorSegments = Field( int, 20 )
	majorRadius = Field( float, 100.0 )
	minorRadius = Field( float, 40.0 )
	position = Field( Point3, Point3() )


	inputClass = GSProductModelBlank
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		prim = TorusPrimitive( self.plane, self.majorSegments, self.minorSegments, self.majorRadius, self.minorRadius )
		mesh = GSProductMesh()
		verts = MeshVertexList()
		prim.createTorus( mesh, verts )
		verts.transform( Matrix4.translate( self.position.toVector3() ), mesh )
		return mesh



class TorusAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcTorusPrimitive

	def initInvoke(self):
		self.verts = MeshVertexList()

	def main(self):
		prim = TorusPrimitive( self._proc.plane, self._proc.majorSegments, self._proc.minorSegments, self._proc.majorRadius, self._proc.minorRadius )
		self.verts.clear()
		self.mesh = GSProductMesh()
		prim.createTorus( self.mesh, self.verts )
		return self.mesh

	def save(self):
		self.verts.savePositions( self.mesh )

	def restore(self):
		self.verts.restorePositions( self.mesh )

	def adjust(self):
		self.verts.transform( Matrix4.translate( self._proc.position.toVector3() ), self.mesh )

	def getResult(self):
		return self.mesh




class ToolTorusPrimitive (GSProcedureTool):
	sheet = SheetRefField( ProcTorusPrimitive )

	functionClass = ProcTorusPrimitive
	adjustableInvoker = TorusAdjustableInvoker
	pageTitle = _( 'Torus' )
	titleText = _( 'Create Torus' )

	planeLabel = FieldLabelWithFn( sheet.plane, _( 'Torus plane:' ) )
	planeRadio = AxisFieldEditRadioGroup( sheet.plane )
	majorSegsLabel = FieldLabelWithFn( sheet.majorSegments, _( 'Major segs.:' ) )
	majorSegsEntry = IntFieldEditEntry( sheet.majorSegments, 3, 1048576, 1, 10 )
	minorSegsLabel = FieldLabelWithFn( sheet.minorSegments, _( 'Minor segs.:' ) )
	minorSegsEntry = IntFieldEditEntry( sheet.minorSegments, 3, 1048576, 1, 10 )
	majorRadiusLabel = FieldLabelWithFn( sheet.majorRadius, _( 'Major radius:' ) )
	majorRadiusEntry = FloatFieldEditEntry( sheet.majorRadius, 0.0, 1.0e8, 0.1, 10.0, 4 )
	minorRadiusLabel = FieldLabelWithFn( sheet.minorRadius, _( 'Minor radius:' ) )
	minorRadiusEntry = FloatFieldEditEntry( sheet.minorRadius, 0.0, 1.0e8, 0.1, 10.0, 4 )
	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e8, 1.0e8, 1.0, 10.0, 4 )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	positionManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  planeLabel.label
	layout.row()  <<  planeRadio.x  <<  planeRadio.y  <<  planeRadio.z
	layout.row()
	layout.row()  <<  majorSegsLabel.label  <<  majorSegsEntry.entry
	layout.row()  <<  minorSegsLabel.label  <<  minorSegsEntry.entry
	layout.row()
	layout.row()  <<  majorRadiusLabel.label  <<  majorRadiusEntry.entry
	layout.row()  <<  minorRadiusLabel.label  <<  minorRadiusEntry.entry
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.finalise()
