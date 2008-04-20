##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector3

from Britefury.FileIO.IOXml import IOXmlEnum

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Kernel.Enum import Enum

from Britefury.Cell.Cell import *

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import GSProcedureTool
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Mesh.Mesh import GSProductMesh




# MeshFlattenVerticesMode enumeration
class MeshFlattenVerticesMode (Enum):
	X = 0
	Y = 1
	Z = 2
	NORMAL = 3




# XML IO for MeshFlattenVerticesMode

flattenModeIOXmlEnum = IOXmlEnum( MeshFlattenVerticesMode, 'MeshFlattenVerticesMode', MeshFlattenVerticesMode.NORMAL )
flattenModeIOXmlEnum.value( MeshFlattenVerticesMode.X, 'x' )
flattenModeIOXmlEnum.value( MeshFlattenVerticesMode.Y, 'y' )
flattenModeIOXmlEnum.value( MeshFlattenVerticesMode.Z, 'z' )
flattenModeIOXmlEnum.value( MeshFlattenVerticesMode.NORMAL, 'normal' )

ioXmlReadMeshFlattenVerticesModeProp = flattenModeIOXmlEnum.propReader
ioXmlReadMeshFlattenVerticesModeNode = flattenModeIOXmlEnum.nodeReader
ioXmlWriteMeshFlattenVerticesModeProp = flattenModeIOXmlEnum.propWriter
ioXmlWriteMeshFlattenVerticesModeNode = flattenModeIOXmlEnum.nodeWriter


# Cell for MeshFlattenVerticesMode

class MeshFlattenVerticesModeCell (Cell):
	valueClass = MeshFlattenVerticesMode
	bAllowNone = False

registerCellType( MeshFlattenVerticesMode, MeshFlattenVerticesModeCell )


# Cell editor for MeshFlattenVerticesMode

class MeshFlattenVerticesModeCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshFlattenVerticesModeCellEditRadioGroup, self ).__init__()
		self.x = self.addChoice( MeshFlattenVerticesMode.X, _( 'X-axis' ) )
		self.y = self.addChoice( MeshFlattenVerticesMode.Y, _( 'Y-axis' ) )
		self.z = self.addChoice( MeshFlattenVerticesMode.Z, _( 'Z-axis' ) )
		self.normal = self.addChoice( MeshFlattenVerticesMode.NORMAL, _( 'Normal' ) )


# Field editor for MeshFlattenVerticesMode

class MeshFlattenVerticesModeFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ =  [ 'x', 'y', 'z', 'normal' ]

	def createElement(self, sheetEditContext):
		return MeshFlattenVerticesModeCellEditRadioGroup()






class ProcMeshFlattenVertices (GSProcedure):
	description = _( 'Flatten vertices' )

	bPerRegion = Field( bool, True )
	mode = Field( MeshFlattenVerticesMode, MeshFlattenVerticesMode.NORMAL )


	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mode = self.mode
		if mode == MeshFlattenVerticesMode.X:
			inputObject.flattenVertices( Vector3( 1.0, 0.0, 0.0 ), self.bPerRegion )
		elif mode == MeshFlattenVerticesMode.Y:
			inputObject.flattenVertices( Vector3( 0.0, 1.0, 0.0 ), self.bPerRegion )
		elif mode == MeshFlattenVerticesMode.Z:
			inputObject.flattenVertices( Vector3( 0.0, 0.0, 1.0 ), self.bPerRegion )
		elif mode == MeshFlattenVerticesMode.NORMAL:
			inputObject.flattenVerticesNormal( self.bPerRegion )
		return inputObject




class ToolMeshFlattenVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshFlattenVertices )

	functionClass = ProcMeshFlattenVertices
	pageTitle = _( 'Flatten' )
	titleText = _( 'Flatten vertices' )

	modeLabel = FieldLabelWithFn( sheet.mode, _( 'Flatten mode:' ) )
	modeRadio = MeshFlattenVerticesModeFieldEditRadioGroup( sheet.mode )
	bPerRegionCheck = BoolFieldEditCheckWithFn( sheet.bPerRegion, _( 'Per region' ) )

	layout = FormLayoutFactory()
	layout.row()  <<  modeLabel.label
	layout.row()  <<  modeRadio.x
	layout.row()  <<  modeRadio.y
	layout.row()  <<  modeRadio.z
	layout.row()  <<  modeRadio.normal
	layout.row()
	layout.row()  <<  bPerRegionCheck.check
	layout.row()
	layout.finalise()
