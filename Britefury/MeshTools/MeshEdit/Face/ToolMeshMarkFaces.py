##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
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

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.MarkPredicateFieldEditor import *

from Britefury.Mesh.Mesh import GSProductMesh



# MeshMarkFacesTarget enumeration
class MeshMarkFacesTarget (Enum):
	ALL = 0
	VERTEXMARKED = 1
	WHOLLYVERTEXMARKED = 2
	EDGEMARKED = 3
	WHOLLYEDGEMARKED = 4




# XML IO for MeshMarkFacesTarget

markTargetIOXmlEnum = IOXmlEnum( MeshMarkFacesTarget, 'MeshMarkFacesTarget', MeshMarkFacesTarget.ALL )
markTargetIOXmlEnum.value( MeshMarkFacesTarget.ALL, 'all' )
markTargetIOXmlEnum.value( MeshMarkFacesTarget.VERTEXMARKED, 'vertex_marked' )
markTargetIOXmlEnum.value( MeshMarkFacesTarget.WHOLLYVERTEXMARKED, 'wholly_vertex_marked' )
markTargetIOXmlEnum.value( MeshMarkFacesTarget.EDGEMARKED, 'edge_marked' )
markTargetIOXmlEnum.value( MeshMarkFacesTarget.WHOLLYEDGEMARKED, 'wholly_edge_marked' )

ioXmlReadMeshMarkFacesTargetProp = markTargetIOXmlEnum.propReader
ioXmlReadMeshMarkFacesTargetNode = markTargetIOXmlEnum.nodeReader
ioXmlWriteMeshMarkFacesTargetProp = markTargetIOXmlEnum.propWriter
ioXmlWriteMeshMarkFacesTargetNode = markTargetIOXmlEnum.nodeWriter


# Cell for MeshMarkFacesTarget

class MeshMarkFacesTargetCell (Cell):
	valueClass = MeshMarkFacesTarget
	bAllowNone = False

registerCellType( MeshMarkFacesTarget, MeshMarkFacesTargetCell )


# Cell editor for MeshMarkFacesTarget

class MeshMarkFacesTargetCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshMarkFacesTargetCellEditRadioGroup, self ).__init__()
		self.all = self.addChoice( MeshMarkFacesTarget.ALL, _( 'All' ) )
		self.vertexMarked = self.addChoice( MeshMarkFacesTarget.VERTEXMARKED, _( 'With selected vertices' ) )
		self.whollyVertexMarked = self.addChoice( MeshMarkFacesTarget.WHOLLYVERTEXMARKED, _( 'With all vertices selected' ) )
		self.edgeMarked = self.addChoice( MeshMarkFacesTarget.EDGEMARKED, _( 'With selected edges' ) )
		self.whollyEdgeMarked = self.addChoice( MeshMarkFacesTarget.WHOLLYEDGEMARKED, _( 'With all edges selected' ) )


# Field editor for MeshMarkFacesTarget

class MeshMarkFacesTargetFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'all', 'vertexMarked', 'whollyVertexMarked', 'edgeMarked', 'whollyEdgeMarked' ]

	def createElement(self, rowEditContext):
		return MeshMarkFacesTargetCellEditRadioGroup()





class ProcMeshMarkFaces (GSProcedure):
	predicate = Field( MarkPredicate, MarkPredicate.MARK )
	target = Field( MeshMarkFacesTarget, MeshMarkFacesTarget.ALL )


	def getDescription(self):
		if self.predicate == MarkPredicate.MARK:
			return _( 'Select faces' )
		elif self.predicate == MarkPredicate.UNMARK:
			return _( 'Unselect faces' )
		elif self.predicate == MarkPredicate.ADD:
			return _( 'Select faces' )
		elif self.predicate == MarkPredicate.FILTER:
			return _( 'Filter face selection' )
		elif self.predicate == MarkPredicate.INVERT:
			return _( 'Invert face selection' )


	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		target = self.target
		if target == MeshMarkFacesTarget.ALL:
			inputObject.markFaces_all( self.predicate )
		elif target == MeshMarkFacesTarget.VERTEXMARKED:
			inputObject.markFaces_vertexMarked( self.predicate )
		elif target == MeshMarkFacesTarget.WHOLLYVERTEXMARKED:
			inputObject.markFaces_whollyVertexMarked( self.predicate )
		elif target == MeshMarkFacesTarget.EDGEMARKED:
			inputObject.markFaces_edgeMarked( self.predicate )
		elif target == MeshMarkFacesTarget.WHOLLYEDGEMARKED:
			inputObject.markFaces_whollyEdgeMarked( self.predicate )
		return inputObject



class ToolMeshMarkFaces (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMarkFaces )

	functionClass = ProcMeshMarkFaces
	pageTitle = _( 'Select' )
	titleText = _( 'Select faces' )

	predicateLabel = FieldLabelWithFn( sheet.predicate, _( 'Select mode:' ) )
	predicateRadio = MarkPredicateFieldEditRadioGroup( sheet.predicate )
	targetLabel = FieldLabelWithFn( sheet.target, _( 'Target:' ) )
	targetRadio = MeshMarkFacesTargetFieldEditRadioGroup( sheet.target )

	layout = FormLayoutFactory()
	layout.row()  <<  predicateLabel.label
	layout.row()  <<  predicateRadio.mark
	layout.row()  <<  predicateRadio.unmark
	layout.row()  <<  predicateRadio.add
	layout.row()  <<  predicateRadio.filter
	layout.row()  <<  predicateRadio.invert
	layout.row()
	layout.row()  <<  targetLabel.label
	layout.row()  <<  targetRadio.all
	layout.row()  <<  targetRadio.vertexMarked
	layout.row()  <<  targetRadio.whollyVertexMarked
	layout.row()  <<  targetRadio.edgeMarked
	layout.row()  <<  targetRadio.whollyEdgeMarked
	layout.row()
	layout.finalise()

