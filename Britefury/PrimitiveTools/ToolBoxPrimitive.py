##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Matrix4

from Britefury.Model.Model import GSProductModelBlank

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import BoxPrimitive, MeshVertexList, GSProductMesh





class ProcBoxPrimitive (GSProcedure):
	description = _( 'Box' )

	xSegments = Field( int, 1 )
	ySegments = Field( int, 1 )
	zSegments = Field( int, 1 )
	width = Field( float, 100.0 )
	height = Field( float, 100.0 )
	depth = Field( float, 100.0 )
	bFront = Field( bool, True )
	bBack = Field( bool, True )
	bLeft = Field( bool, True )
	bRight = Field( bool, True )
	bBottom = Field( bool, True )
	bTop = Field( bool, True )
	position = Field( Point3, Point3() )


	inputClass = GSProductModelBlank
	outputClass = GSProductMesh


	def procedureInvoke(self, inputObject, errorHandler):
		prim = BoxPrimitive( self.xSegments, self.ySegments, self.zSegments, self.width, self.height, self.depth,
						self.bFront, self.bBack, self.bLeft, self.bRight, self.bBottom, self.bTop )
		mesh = GSProductMesh()
		verts = MeshVertexList()
		prim.createBox( mesh, verts )
		verts.transform( Matrix4.translate( self.position.toVector3() ), mesh )
		return mesh



class BoxAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcBoxPrimitive

	def initInvoke(self):
		self.verts = MeshVertexList()

	def main(self):
		prim = BoxPrimitive( self._proc.xSegments, self._proc.ySegments, self._proc.zSegments, self._proc.width, self._proc.height, self._proc.depth, self._proc.bFront, self._proc.bBack, self._proc.bLeft, self._proc.bRight, self._proc.bBottom, self._proc.bTop )
		self.verts.clear()
		self.mesh = GSProductMesh()
		prim.createBox( self.mesh, self.verts )
		return self.mesh

	def save(self):
		self.verts.savePositions( self.mesh )

	def restore(self):
		self.verts.restorePositions( self.mesh )

	def adjust(self):
		self.verts.transform( Matrix4.translate( self._proc.position.toVector3() ), self.mesh )

	def getResult(self):
		return self.mesh




class ToolBoxPrimitive (GSProcedureTool):
	sheet = SheetRefField( ProcBoxPrimitive )

	functionClass = ProcBoxPrimitive
	adjustableInvoker = BoxAdjustableInvoker
	pageTitle = _( 'Box' )
	titleText = _( 'Create Box' )

	xSegsLabel = FieldLabelWithFn( sheet.xSegments, _( 'X-Segs:' ) )
	xSegsEntry = IntFieldEditEntry( sheet.xSegments, 1, 1048576, 1, 10 )
	ySegsLabel = FieldLabelWithFn( sheet.ySegments, _( 'Y-Segs:' ) )
	ySegsEntry = IntFieldEditEntry( sheet.ySegments, 1, 1048576, 1, 10 )
	zSegsLabel = FieldLabelWithFn( sheet.zSegments, _( 'Z-Segs:' ) )
	zSegsEntry = IntFieldEditEntry( sheet.zSegments, 1, 1048576, 1, 10 )
	widthLabel = FieldLabelWithFn( sheet.width, _( 'Width:' ) )
	widthEntry = FloatFieldEditEntry( sheet.width, 0.0, 1.0e8, 0.1, 10.0, 4 )
	heightLabel = FieldLabelWithFn( sheet.height, _( 'Height:' ) )
	heightEntry = FloatFieldEditEntry( sheet.height, 0.0, 1.0e8, 1.0, 10.0, 4 )
	depthLabel = FieldLabelWithFn( sheet.depth, _( 'Depth:' ) )
	depthEntry = FloatFieldEditEntry( sheet.depth, 0.0, 1.0e8, 1.0, 10.0, 4 )
	positionLabel = FieldLabelWithFn( sheet.position, _( 'Position:' ) )
	positionEntry = Point3FieldEditEntry( sheet.position, -1.0e8, 1.0e8, 1.0, 10.0, 4 )
	bFrontCheck = BoolFieldEditCheckWithFn( sheet.bFront, _( 'Front' ) )
	bBackCheck = BoolFieldEditCheckWithFn( sheet.bBack, _( 'Back' ) )
	bLeftCheck = BoolFieldEditCheckWithFn( sheet.bLeft, _( 'Left' ) )
	bRightCheck = BoolFieldEditCheckWithFn( sheet.bRight, _( 'Right' ) )
	bBottomCheck = BoolFieldEditCheckWithFn( sheet.bBottom, _( 'Bottom' ) )
	bTopCheck = BoolFieldEditCheckWithFn( sheet.bTop, _( 'Top' ) )

	positionManip = Point3FieldManipulator( sheet.position, snapping=FieldManipulatorSnappingDistance() )
	positionManip.enable()

	layout = FormLayoutFactory()
	layout.row()  <<  xSegsLabel.label  <<  xSegsEntry.entry
	layout.row()  <<  ySegsLabel.label  <<  ySegsEntry.entry
	layout.row()  <<  zSegsLabel.label  <<  zSegsEntry.entry
	layout.row()
	layout.row()  <<  widthLabel.label  <<  widthEntry.entry
	layout.row()  <<  heightLabel.label  <<  heightEntry.entry
	layout.row()  <<  depthLabel.label  <<  depthEntry.entry
	layout.row()
	layout.row()  <<  positionLabel.label
	layout.row()  <<  'X:'  <<  positionEntry.x
	layout.row()  <<  'Y:'  <<  positionEntry.y
	layout.row()  <<  'Z:'  <<  positionEntry.z
	layout.row()
	layout.row()  <<  bFrontCheck.check  <<  bBackCheck.check
	layout.row()  <<  bLeftCheck.check  <<  bRightCheck.check
	layout.row()  <<  bBottomCheck.check  <<  bTopCheck.check
	layout.row()
	layout.finalise()
