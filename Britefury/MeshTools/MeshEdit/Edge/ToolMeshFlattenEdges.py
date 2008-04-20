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




# MeshFlattenEdgesMode enumeration
class MeshFlattenEdgesMode (Enum):
	X = 0
	Y = 1
	Z = 2
	NORMAL = 3
	FLATTEN_RUNS = 4
	STRAIGHTEN_RUNS = 5




# XML IO for MeshFlattenEdgesMode

flattenModeIOXmlEnum = IOXmlEnum( MeshFlattenEdgesMode, 'MeshFlattenEdgesMode', MeshFlattenEdgesMode.NORMAL )
flattenModeIOXmlEnum.value( MeshFlattenEdgesMode.X, 'x' )
flattenModeIOXmlEnum.value( MeshFlattenEdgesMode.Y, 'y' )
flattenModeIOXmlEnum.value( MeshFlattenEdgesMode.Z, 'z' )
flattenModeIOXmlEnum.value( MeshFlattenEdgesMode.NORMAL, 'normal' )
flattenModeIOXmlEnum.value( MeshFlattenEdgesMode.FLATTEN_RUNS, 'flatten_runs' )
flattenModeIOXmlEnum.value( MeshFlattenEdgesMode.STRAIGHTEN_RUNS, 'straighten_runs' )

ioXmlReadMeshFlattenEdgesModeProp = flattenModeIOXmlEnum.propReader
ioXmlReadMeshFlattenEdgesModeNode = flattenModeIOXmlEnum.nodeReader
ioXmlWriteMeshFlattenEdgesModeProp = flattenModeIOXmlEnum.propWriter
ioXmlWriteMeshFlattenEdgesModeNode = flattenModeIOXmlEnum.nodeWriter



# Cell for MeshFlattenEdgesMode

class MeshFlattenEdgesModeCell (Cell):
	valueClass = MeshFlattenEdgesMode
	bAllowNone = False

registerCellType( MeshFlattenEdgesMode, MeshFlattenEdgesModeCell )


# Cell editor for MeshFlattenEdgesMode

class MeshFlattenEdgesModeCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshFlattenEdgesModeCellEditRadioGroup, self ).__init__()
		self.x = self.addChoice( MeshFlattenEdgesMode.X, _( 'X-axis' ) )
		self.y = self.addChoice( MeshFlattenEdgesMode.Y, _( 'Y-axis' ) )
		self.z = self.addChoice( MeshFlattenEdgesMode.Z, _( 'Z-axis' ) )
		self.normal = self.addChoice( MeshFlattenEdgesMode.NORMAL, _( 'Normal' ) )
		self.flattenRuns = self.addChoice( MeshFlattenEdgesMode.FLATTEN_RUNS, _( 'Flatten edge runs' ) )
		self.straightenRuns = self.addChoice( MeshFlattenEdgesMode.STRAIGHTEN_RUNS, _( 'Straighten edge runs' ) )


# Field editor for MeshFlattenEdgesMode

class MeshFlattenEdgesModeFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ =  [ 'x', 'y', 'z', 'normal', 'flattenRuns', 'straightenRuns' ]

	def createElement(self, rowEditContext):
		return MeshFlattenEdgesModeCellEditRadioGroup()





class ProcMeshFlattenEdges (GSProcedure):
	description = _( 'Flatten edges' )

	bPerRegion = Field( bool, True )
	mode = Field( MeshFlattenEdgesMode, MeshFlattenEdgesMode.NORMAL )


	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mode = self.mode
		if mode == MeshFlattenEdgesMode.X:
			inputObject.flattenEdges( Vector3( 1.0, 0.0, 0.0 ), self.bPerRegion )
		elif mode == MeshFlattenEdgesMode.Y:
			inputObject.flattenEdges( Vector3( 0.0, 1.0, 0.0 ), self.bPerRegion )
		elif mode == MeshFlattenEdgesMode.Z:
			inputObject.flattenEdges( Vector3( 0.0, 0.0, 1.0 ), self.bPerRegion )
		elif mode == MeshFlattenEdgesMode.NORMAL:
			inputObject.flattenEdgesNormal( self.bPerRegion )
		elif mode == MeshFlattenEdgesMode.FLATTEN_RUNS:
			inputObject.flattenEdgeRuns()
		elif mode == MeshFlattenEdgesMode.STRAIGHTEN_RUNS:
			inputObject.straightenEdgeRuns()
		return inputObject




class ToolMeshFlattenEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshFlattenEdges )

	functionClass = ProcMeshFlattenEdges
	pageTitle = _( 'Flatten' )
	titleText = _( 'Flatten edges' )

	bPerRegionCheck = BoolFieldEditCheckWithFn( sheet.bPerRegion, _( 'Per region' ) )
	modeLabel = FieldLabelWithFn( sheet.mode, _( 'Flatten mode:' ) )
	modeRadio = MeshFlattenEdgesModeFieldEditRadioGroup( sheet.mode )

	layout = FormLayoutFactory()
	layout.row()  <<  modeLabel.label
	layout.row()  <<  modeRadio.x
	layout.row()  <<  modeRadio.y
	layout.row()  <<  modeRadio.z
	layout.row()  <<  modeRadio.normal
	layout.row()  <<  modeRadio.flattenRuns
	layout.row()  <<  modeRadio.straightenRuns
	layout.row()
	layout.row()  <<  bPerRegionCheck.check
	layout.row()
	layout.finalise()
