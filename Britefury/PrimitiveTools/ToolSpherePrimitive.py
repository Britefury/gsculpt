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
from Britefury.Mesh.Mesh import SpherePrimitive, MeshVertexList, GSProductMesh



class ProcSpherePrimitive (GSProcedure):
	description = _( 'Sphere' )

	axis = Field( Axis, Axis.AXIS_Y )
	stacks = Field( int, 12 )
	slices = Field( int, 24 )
	radius = Field( float, 50.0 )
	position = Field( Point3, Point3() )


	inputClass = GSProductModelBlank
	outputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		prim = SpherePrimitive( self.axis, self.stacks, self.slices, self.radius )
		mesh = GSProductMesh()
		verts = MeshVertexList()
		prim.createSphere( mesh, verts )
		verts.transform( Matrix4.translate( self.position.toVector3() ), mesh )
		return mesh



class SphereAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcSpherePrimitive

	def initInvoke(self):
		self.verts = MeshVertexList()

	def main(self):
		prim = SpherePrimitive( self._proc.axis, self._proc.stacks, self._proc.slices, self._proc.radius )
		self.verts.clear()
		self.mesh = GSProductMesh()
		prim.createSphere( self.mesh, self.verts )
		return self.mesh

	def save(self):
		self.verts.savePositions( self.mesh )

	def restore(self):
		self.verts.restorePositions( self.mesh )

	def adjust(self):
		self.verts.transform( Matrix4.translate( self._proc.position.toVector3() ), self.mesh )

	def getResult(self):
		return self.mesh




class ToolSpherePrimitive (GSProcedureTool):
	sheet = SheetRefField( ProcSpherePrimitive )

	functionClass = ProcSpherePrimitive
	adjustableInvoker = SphereAdjustableInvoker
	pageTitle = _( 'Sphere' )
	titleText = _( 'Create Sphere' )

	axisLabel = FieldLabelWithFn( sheet.axis, _( 'Sphere axis:' ) )
	axisRadio = AxisFieldEditRadioGroup( sheet.axis )
	stacksLabel = FieldLabelWithFn( sheet.stacks, _( 'Stacks:' ) )
	stacksEntry = IntFieldEditEntry( sheet.stacks, 3, 1048576, 1, 10 )
	slicesLabel = FieldLabelWithFn( sheet.slices, _( 'Slices:' ) )
	slicesEntry = IntFieldEditEntry( sheet.slices, 3, 1048576, 1, 10 )
	radiusLabel = FieldLabelWithFn( sheet.radius, _( 'Radius:' ) )
	radiusEntry = FloatFieldEditEntry( sheet.radius, 0.0, 1.0e8, 0.1, 10.0, 4 )
	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e8, 1.0e8, 1.0, 10.0, 4 )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	positionManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  axisLabel.label
	layout.row()  <<  axisRadio.x  <<  axisRadio.y  <<  axisRadio.z
	layout.row()
	layout.row()  <<  stacksLabel.label  <<  stacksEntry.entry
	layout.row()  <<  slicesLabel.label  <<  slicesEntry.entry
	layout.row()
	layout.row()  <<  radiusLabel.label  <<  radiusEntry.entry
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.finalise()
