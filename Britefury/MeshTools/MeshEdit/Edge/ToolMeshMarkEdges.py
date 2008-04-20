##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
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

from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model.MarkPredicateFieldEditor import *

from Britefury.Mesh.Mesh import GSProductMesh



# MeshMarkEdgesTarget enumeration
class MeshMarkEdgesTarget (Enum):
	ALL = 0
	VERTEXMARKED = 1
	WHOLLYVERTEXMARKED = 2
	FACEMARKED = 3
	FACEMARKBOUNDARY = 4
	FACEMARKINTERNAL = 5
	BOUNDARY = 6
	INTERNAL = 7




# XML IO for MeshMarkEdgesTarget

markTargetIOXmlEnum = IOXmlEnum( MeshMarkEdgesTarget, 'MeshMarkEdgesTarget', MeshMarkEdgesTarget.ALL )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.ALL, 'all' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.VERTEXMARKED, 'vertex_marked' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.WHOLLYVERTEXMARKED, 'wholly_vertex_marked' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.FACEMARKED, 'face_marked' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.FACEMARKBOUNDARY, 'face_mark_boundary' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.FACEMARKINTERNAL, 'face_mark_internal' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.BOUNDARY, 'boundary' )
markTargetIOXmlEnum.value( MeshMarkEdgesTarget.INTERNAL, 'internal' )

ioXmlReadMeshMarkEdgesTargetProp = markTargetIOXmlEnum.propReader
ioXmlReadMeshMarkEdgesTargetNode = markTargetIOXmlEnum.nodeReader
ioXmlWriteMeshMarkEdgesTargetProp = markTargetIOXmlEnum.propWriter
ioXmlWriteMeshMarkEdgesTargetNode = markTargetIOXmlEnum.nodeWriter



# Cell for MeshMarkEdgesTarget

class MeshMarkEdgesTargetCell (Cell):
	valueClass = MeshMarkEdgesTarget
	bAllowNone = False

registerCellType( MeshMarkEdgesTarget, MeshMarkEdgesTargetCell )


# Cell editor for MeshMarkEdgesTarget

class MeshMarkEdgesTargetCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshMarkEdgesTargetCellEditRadioGroup, self ).__init__()
		self.all = self.addChoice( MeshMarkEdgesTarget.ALL, _( 'All' ) )
		self.vertexMarked = self.addChoice( MeshMarkEdgesTarget.VERTEXMARKED, _( 'With selected vertices' ) )
		self.whollyVertexMarked = self.addChoice( MeshMarkEdgesTarget.WHOLLYVERTEXMARKED, _( 'With all vertices selected' ) )
		self.faceMarked = self.addChoice( MeshMarkEdgesTarget.FACEMARKED, _( 'With selected faces' ) )
		self.faceMarkBoundary = self.addChoice( MeshMarkEdgesTarget.FACEMARKBOUNDARY, _( 'Face selection boundary' ) )
		self.faceMarkInternal = self.addChoice( MeshMarkEdgesTarget.FACEMARKINTERNAL, _( 'Both faces selected' ) )
		self.boundary = self.addChoice( MeshMarkEdgesTarget.BOUNDARY, _( 'Boundary' ) )
		self.internal = self.addChoice( MeshMarkEdgesTarget.INTERNAL, _( 'Internal' ) )


# Field editor for MeshMarkEdgesTarget

class MeshMarkEdgesTargetFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ =  [ 'all', 'vertexMarked', 'whollyVertexMarked', 'faceMarked', 'faceMarkBoundary', 'faceMarkInternal', 'boundary', 'internal' ]

	def createElement(self, rowEditContext):
		return MeshMarkEdgesTargetCellEditRadioGroup()





class ProcMeshMarkEdges (GSProcedure):
	predicate = Field( MarkPredicate, MarkPredicate.MARK )
	target = Field( MeshMarkEdgesTarget, MeshMarkEdgesTarget.ALL )


	def getDescription(self):
		if self.predicate == MarkPredicate.MARK:
			return _( 'Select edges' )
		elif self.predicate == MarkPredicate.UNMARK:
			return _( 'Unselect edges' )
		elif self.predicate == MarkPredicate.ADD:
			return _( 'Select edges' )
		elif self.predicate == MarkPredicate.FILTER:
			return _( 'Filter edge selection' )
		elif self.predicate == MarkPredicate.INVERT:
			return _( 'Invert edge selection' )


	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		target = self.target
		if target == MeshMarkEdgesTarget.ALL:
			inputObject.markEdges_all( self.predicate )
		elif target == MeshMarkEdgesTarget.VERTEXMARKED:
			inputObject.markEdges_vertexMarked( self.predicate )
		elif target == MeshMarkEdgesTarget.WHOLLYVERTEXMARKED:
			inputObject.markEdges_whollyVertexMarked( self.predicate )
		elif target == MeshMarkEdgesTarget.FACEMARKED:
			inputObject.markEdges_faceMarked( self.predicate )
		elif target == MeshMarkEdgesTarget.FACEMARKBOUNDARY:
			inputObject.markEdges_faceMarkBoundary( self.predicate )
		elif target == MeshMarkEdgesTarget.FACEMARKINTERNAL:
			inputObject.markEdges_faceMarkInternal( self.predicate )
		elif target == MeshMarkEdgesTarget.BOUNDARY:
			inputObject.markEdges_boundary( self.predicate )
		elif target == MeshMarkEdgesTarget.INTERNAL:
			inputObject.markEdges_internal( self.predicate )
		return inputObject



class ToolMeshMarkEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMarkEdges )

	functionClass = ProcMeshMarkEdges
	pageTitle = _( 'Select' )
	titleText = _( 'Select edges' )

	predicateLabel = FieldLabelWithFn( sheet.predicate, _( 'Select mode:' ) )
	predicateRadio = MarkPredicateFieldEditRadioGroup( sheet.predicate )
	targetLabel = FieldLabelWithFn( sheet.target, _( 'Target:' ) )
	targetRadio = MeshMarkEdgesTargetFieldEditRadioGroup( sheet.target )

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
	layout.row()  <<  targetRadio.faceMarked
	layout.row()  <<  targetRadio.faceMarkBoundary
	layout.row()  <<  targetRadio.faceMarkInternal
	layout.row()  <<  targetRadio.boundary
	layout.row()  <<  targetRadio.internal
	layout.row()
	layout.finalise()

