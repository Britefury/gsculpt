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
from Britefury.Mesh.Mesh import CylinderPrimitive, MeshVertexList, GSProductMesh



class ProcCylinderPrimitive (GSProcedure):
	description = _( 'Cylinder' )

	axis = Field( Axis, Axis.AXIS_Y )
	angularSegments = Field( int, 12 )
	lengthSegments = Field( int, 1 )
	radialSegments = Field( int, 1 )
	radius = Field( float, 20.0 )
	length = Field( float, 100.0 )
	bCapStart = Field( bool, True )
	bCapEnd = Field( bool, True )
	position = Field( Point3, Point3() )


	inputClass = GSProductModelBlank
	outputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		prim = CylinderPrimitive( self.axis, self.angularSegments, self.lengthSegments, self.radialSegments, self.radius, self.radius, self.length, self.bCapStart, self.bCapEnd )
		mesh = GSProductMesh()
		verts = MeshVertexList()
		prim.createCylinder( mesh, verts )
		verts.transform( Matrix4.translate( self.position.toVector3() ), mesh )
		return mesh



class CylinderAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcCylinderPrimitive

	def initInvoke(self):
		self.verts = MeshVertexList()

	def main(self):
		prim = CylinderPrimitive( self._proc.axis, self._proc.angularSegments, self._proc.lengthSegments, self._proc.radialSegments, self._proc.radius, self._proc.radius, self._proc.length, self._proc.bCapStart, self._proc.bCapEnd )
		self.verts.clear()
		self.mesh = GSProductMesh()
		prim.createCylinder( self.mesh, self.verts )
		return self.mesh

	def save(self):
		self.verts.savePositions( self.mesh )

	def restore(self):
		self.verts.restorePositions( self.mesh )

	def adjust(self):
		self.verts.transform( Matrix4.translate( self._proc.position.toVector3() ), self.mesh )

	def getResult(self):
		return self.mesh




class ToolCylinderPrimitive (GSProcedureTool):
	sheet = SheetRefField( ProcCylinderPrimitive )

	functionClass = ProcCylinderPrimitive
	adjustableInvoker = CylinderAdjustableInvoker
	pageTitle = _( 'Cylinder' )
	titleText = _( 'Create Cylinder' )

	axisLabel = FieldLabelWithFn( sheet.axis, _( 'Cylinder axis:' ) )
	axisRadio = AxisFieldEditRadioGroup( sheet.axis )
	angularSegsLabel = FieldLabelWithFn( sheet.angularSegments, _( 'Angular segs.:' ) )
	angularSegsEntry = IntFieldEditEntry( sheet.angularSegments, 3, 1048576, 1, 10 )
	lengthSegsLabel = FieldLabelWithFn( sheet.lengthSegments, _( 'Length segs.:' ) )
	lengthSegsEntry = IntFieldEditEntry( sheet.lengthSegments, 1, 1048576, 1, 10 )
	radialSegsLabel = FieldLabelWithFn( sheet.radialSegments, _( 'Radial segs.:' ) )
	radialSegsEntry = IntFieldEditEntry( sheet.radialSegments, 1, 1048576, 1, 10 )
	radiusLabel = FieldLabelWithFn( sheet.radius, _( 'Radius:' ) )
	radiusEntry = FloatFieldEditEntry( sheet.radius, 0.0, 1.0e8, 0.1, 10.0, 4 )
	lengthLabel = FieldLabelWithFn( sheet.length, _( 'Length:' ) )
	lengthEntry = FloatFieldEditEntry( sheet.length, 0.0, 1.0e8, 1.0, 10.0, 4 )
	bCapStartCheck = BoolFieldEditCheckWithFn( sheet.bCapStart, _( 'Cap start' ) )
	bCapEndCheck = BoolFieldEditCheckWithFn( sheet.bCapEnd, _( 'Cap end' ) )
	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e8, 1.0e8, 1.0, 10.0, 4 )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	positionManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  axisLabel.label
	layout.row()  <<  axisRadio.x  <<  axisRadio.y  <<  axisRadio.z
	layout.row()
	layout.row()  <<  angularSegsLabel.label  <<  angularSegsEntry.entry
	layout.row()  <<  lengthSegsLabel.label  <<  lengthSegsEntry.entry
	layout.row()  <<  radialSegsLabel.label  <<  radialSegsEntry.entry
	layout.row()
	layout.row()  <<  radiusLabel.label  <<  radiusEntry.entry
	layout.row()  <<  lengthLabel.label  <<  lengthEntry.entry
	layout.row()
	layout.row()  <<  bCapStartCheck.check  <<  bCapEndCheck.check
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.finalise()
