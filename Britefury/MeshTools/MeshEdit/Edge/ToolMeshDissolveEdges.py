##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

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

from Britefury.Mesh import Mesh
from Britefury.Mesh.Mesh import MeshDissolveEdgesStyle, GSProductMesh




# XML IO for MeshDissolveEdgesStyle

dissolveStyleIOXmlEnum = IOXmlEnum( MeshDissolveEdgesStyle, 'MeshDissolveEdgesStyle', MeshDissolveEdgesStyle.MIDPOINT_MAT_A )
dissolveStyleIOXmlEnum.value( MeshDissolveEdgesStyle.A, 'a' )
dissolveStyleIOXmlEnum.value( MeshDissolveEdgesStyle.B, 'b' )
dissolveStyleIOXmlEnum.value( MeshDissolveEdgesStyle.MIDPOINT_MAT_A, 'midpoint_mat_a' )
dissolveStyleIOXmlEnum.value( MeshDissolveEdgesStyle.MIDPOINT_MAT_B, 'midpoint_mat_b' )

ioXmlReadMeshDissolveEdgesStyleProp = dissolveStyleIOXmlEnum.propReader
ioXmlReadMeshDissolveEdgesStyleNode = dissolveStyleIOXmlEnum.nodeReader
ioXmlWriteMeshDissolveEdgesStyleProp = dissolveStyleIOXmlEnum.propWriter
ioXmlWriteMeshDissolveEdgesStyleNode = dissolveStyleIOXmlEnum.nodeWriter


# Cell for MeshDissolveEdgesStyle

class MeshDissolveEdgesStyleCell (Cell):
	valueClass = MeshDissolveEdgesStyle
	bAllowNone = False

registerCellType( MeshDissolveEdgesStyle, MeshDissolveEdgesStyleCell )


# Cell editor for MeshDissolveEdgesStyle

class MeshDissolveEdgesStyleCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshDissolveEdgesStyleCellEditRadioGroup, self ).__init__()
		self.a = self.addChoice( MeshDissolveEdgesStyle.A, 'A' )
		self.b = self.addChoice( MeshDissolveEdgesStyle.B, 'B' )
		self.midPointMatA = self.addChoice( MeshDissolveEdgesStyle.MIDPOINT_MAT_A, _( 'Mid-point, material A' ) )
		self.midPointMatB = self.addChoice( MeshDissolveEdgesStyle.MIDPOINT_MAT_B, _( 'Mid-point, material B' ) )


# Field editor MeshDissolveEdgesStyle

class MeshDissolveEdgesStyleFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'a', 'b', 'midPointMatA', 'midPointMatB' ]

	def createElement(self, rowEditContext):
		return MeshDissolveEdgesStyleCellEditRadioGroup()







class ProcMeshDissolveEdges (GSProcedure):
	description = _( 'Dissolve edges' )

	style = Field( MeshDissolveEdgesStyle, MeshDissolveEdgesStyle.MIDPOINT_MAT_A )
	bDissolveSimpleValenceTwoVertices = Field( bool, False )

	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		inputObject.dissolveMarkedEdges( self.style, self.bDissolveSimpleValenceTwoVertices )
		return inputObject



class ToolMeshDissolveEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshDissolveEdges )

	functionClass = ProcMeshDissolveEdges
	pageTitle = _( 'Dissolve' )
	titleText = _( 'Dissolve Edges' )

	styleLabel = FieldLabelWithFn( sheet.style, _( 'Style:' ) )
	styleRadio = MeshDissolveEdgesStyleFieldEditRadioGroup( sheet.style )
	bDissolveSimpleValenceTwoVerticesCheck = BoolFieldEditCheckWithFn( sheet.bDissolveSimpleValenceTwoVertices, _( 'Clean dissolve' ) )

	layout = FormLayoutFactory()
	layout.row()  <<  styleLabel.label
	layout.row()  <<  styleRadio.a
	layout.row()  <<  styleRadio.b
	layout.row()  <<  styleRadio.midPointMatA
	layout.row()  <<  styleRadio.midPointMatB
	layout.row()
	layout.row()  <<  bDissolveSimpleValenceTwoVerticesCheck.check
	layout.row()
	layout.finalise()
