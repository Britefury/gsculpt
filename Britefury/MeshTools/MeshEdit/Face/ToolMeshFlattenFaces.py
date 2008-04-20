##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Vector3

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive

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




# MeshFlattenFacesMode enumeration
class MeshFlattenFacesMode (Enum):
	X = 0
	Y = 1
	Z = 2
	NORMAL = 3




# XML IO for MeshFlattenFacesMode

flattenModeIOXmlEnum = IOXmlEnum( MeshFlattenFacesMode, 'MeshFlattenFacesMode', MeshFlattenFacesMode.NORMAL )
flattenModeIOXmlEnum.value( MeshFlattenFacesMode.X, 'x' )
flattenModeIOXmlEnum.value( MeshFlattenFacesMode.Y, 'y' )
flattenModeIOXmlEnum.value( MeshFlattenFacesMode.Z, 'z' )
flattenModeIOXmlEnum.value( MeshFlattenFacesMode.NORMAL, 'normal' )

ioXmlReadMeshFlattenFacesModeProp = flattenModeIOXmlEnum.propReader
ioXmlReadMeshFlattenFacesModeNode = flattenModeIOXmlEnum.nodeReader
ioXmlWriteMeshFlattenFacesModeProp = flattenModeIOXmlEnum.propWriter
ioXmlWriteMeshFlattenFacesModeNode = flattenModeIOXmlEnum.nodeWriter


# Cell for MeshFlattenFacesMode

class MeshFlattenFacesModeCell (Cell):
	valueClass = MeshFlattenFacesMode
	bAllowNone = False

registerCellType( MeshFlattenFacesMode, MeshFlattenFacesModeCell )


# Cell editor for MeshFlattenFacesMode

class MeshFlattenFacesModeCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshFlattenFacesModeCellEditRadioGroup, self ).__init__()
		self.x = self.addChoice( MeshFlattenFacesMode.X, _( 'X-axis' ) )
		self.y = self.addChoice( MeshFlattenFacesMode.Y, _( 'Y-axis' ) )
		self.z = self.addChoice( MeshFlattenFacesMode.Z, _( 'Z-axis' ) )
		self.normal = self.addChoice( MeshFlattenFacesMode.NORMAL, _( 'Normal' ) )


# Field editor for MeshFlattenFacesMode

class MeshFlattenFacesModeFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'x', 'y', 'z', 'normal' ]

	def createElement(self, rowEditContext):
		return MeshFlattenFacesModeCellEditRadioGroup()





class ProcMeshFlattenFaces (GSProcedure):
	description = _( 'Flatten faces' )

	bPerRegion = Field( bool, True )
	mode = Field( MeshFlattenFacesMode, MeshFlattenFacesMode.NORMAL )


	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mode = self.mode
		if mode == MeshFlattenFacesMode.X:
			inputObject.flattenFaces( Vector3( 1.0, 0.0, 0.0 ), self.bPerRegion )
		elif mode == MeshFlattenFacesMode.Y:
			inputObject.flattenFaces( Vector3( 0.0, 1.0, 0.0 ), self.bPerRegion )
		elif mode == MeshFlattenFacesMode.Z:
			inputObject.flattenFaces( Vector3( 0.0, 0.0, 1.0 ), self.bPerRegion )
		elif mode == MeshFlattenFacesMode.NORMAL:
			inputObject.flattenFacesNormal( self.bPerRegion )
		return inputObject




class ToolMeshFlattenFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshFlattenFaces )

	functionClass = ProcMeshFlattenFaces
	pageTitle = _( 'Flatten' )
	titleText = _( 'Flatten faces' )

	modeLabel = FieldLabelWithFn( sheet.mode, _( 'Flatten mode:' ) )
	modeRadio = MeshFlattenFacesModeFieldEditRadioGroup( sheet.mode )
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
