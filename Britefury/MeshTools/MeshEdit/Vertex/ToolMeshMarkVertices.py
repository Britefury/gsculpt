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




# MeshMarkVerticesTarget enumeration
class MeshMarkVerticesTarget (Enum):
	ALL = 0
	EDGEMARKED = 1
	FACEMARKED = 2
	FACEMARKBOUNDARY = 3
	FACEMARKINTERNAL = 4
	BOUNDARY = 5
	INTERNAL = 6




# XML IO for MeshMarkVerticesTarget

markTargetIOXmlEnum = IOXmlEnum( MeshMarkVerticesTarget, 'MeshMarkVerticesTarget', MeshMarkVerticesTarget.ALL )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.ALL, 'all' )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.EDGEMARKED, 'edge_marked' )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.FACEMARKED, 'face_marked' )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.FACEMARKBOUNDARY, 'face_mark_boundary' )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.FACEMARKINTERNAL, 'face_mark_internal' )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.BOUNDARY, 'boundary' )
markTargetIOXmlEnum.value( MeshMarkVerticesTarget.INTERNAL, 'internal' )

ioXmlReadMeshMarkVerticesTargetProp = markTargetIOXmlEnum.propReader
ioXmlReadMeshMarkVerticesTargetNode = markTargetIOXmlEnum.nodeReader
ioXmlWriteMeshMarkVerticesTargetProp = markTargetIOXmlEnum.propWriter
ioXmlWriteMeshMarkVerticesTargetNode = markTargetIOXmlEnum.nodeWriter


# v for MeshMarkVerticesTarget

class MeshMarkVerticesTargetCell (Cell):
	valueClass = MeshMarkVerticesTarget
	bAllowNone = False

registerCellType( MeshMarkVerticesTarget, MeshMarkVerticesTargetCell )


# Cell editor for MeshMarkVerticesTarget

class MeshMarkVerticesTargetCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshMarkVerticesTargetCellEditRadioGroup, self ).__init__()
		self.all = self.addChoice( MeshMarkVerticesTarget.ALL, _( 'All' ) )
		self.edgeMarked = self.addChoice( MeshMarkVerticesTarget.EDGEMARKED, _( 'With selected edges' ) )
		self.faceMarked = self.addChoice( MeshMarkVerticesTarget.FACEMARKED, _( 'With selected faces' ) )
		self.faceMarkBoundary = self.addChoice( MeshMarkVerticesTarget.FACEMARKBOUNDARY, _( 'On face selection boundary' ) )
		self.faceMarkInternal = self.addChoice( MeshMarkVerticesTarget.FACEMARKINTERNAL, _( 'All faces selected' ) )
		self.boundary = self.addChoice( MeshMarkVerticesTarget.BOUNDARY, _( 'Boundary' ) )
		self.internal = self.addChoice( MeshMarkVerticesTarget.INTERNAL, _( 'Internal' ) )


# Field editor for MeshMarkVerticesTarget

class MeshMarkVerticesTargetFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ =  [ 'all', 'edgeMarked', 'faceMarked', 'faceMarkBoundary', 'faceMarkInternal', 'boundary', 'internal' ]

	def createElement(self, rowEditContext):
		return MeshMarkVerticesTargetCellEditRadioGroup()





class ProcMeshMarkVertices (GSProcedure):
	predicate = Field( MarkPredicate, MarkPredicate.MARK )
	target = Field( MeshMarkVerticesTarget, MeshMarkVerticesTarget.ALL )


	def getDescription(self):
		if self.predicate == MarkPredicate.MARK:
			return _( 'Select vertices' )
		elif self.predicate == MarkPredicate.UNMARK:
			return _( 'Unselect vertices' )
		elif self.predicate == MarkPredicate.ADD:
			return _( 'Select vertices' )
		elif self.predicate == MarkPredicate.FILTER:
			return _( 'Filter vertex selection' )
		elif self.predicate == MarkPredicate.INVERT:
			return _( 'Invert vertex selection' )


	inputClass = GSProductMesh
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		target = self.target
		if target == MeshMarkVerticesTarget.ALL:
			inputObject.markVertices_all( self.predicate )
		elif target == MeshMarkVerticesTarget.EDGEMARKED:
			inputObject.markVertices_edgeMarked( self.predicate )
		elif target == MeshMarkVerticesTarget.FACEMARKED:
			inputObject.markVertices_faceMarked( self.predicate )
		elif target == MeshMarkVerticesTarget.FACEMARKBOUNDARY:
			inputObject.markVertices_faceMarkBoundary( self.predicate )
		elif target == MeshMarkVerticesTarget.FACEMARKINTERNAL:
			inputObject.markVertices_faceMarkInternal( self.predicate )
		elif target == MeshMarkVerticesTarget.BOUNDARY:
			inputObject.markVertices_boundary( self.predicate )
		elif target == MeshMarkVerticesTarget.INTERNAL:
			inputObject.markVertices_internal( self.predicate )
		return inputObject



class ToolMeshMarkVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshMarkVertices )

	functionClass = ProcMeshMarkVertices
	pageTitle = _( 'Select' )
	titleText = _( 'Select vertices' )

	predicateLabel = FieldLabelWithFn( sheet.predicate, _( 'Select mode:' ) )
	predicateRadio = MarkPredicateFieldEditRadioGroup( sheet.predicate )
	targetLabel = FieldLabelWithFn( sheet.target, _( 'Target:' ) )
	targetRadio = MeshMarkVerticesTargetFieldEditRadioGroup( sheet.target )

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
	layout.row()  <<  targetRadio.edgeMarked
	layout.row()  <<  targetRadio.faceMarked
	layout.row()  <<  targetRadio.faceMarkBoundary
	layout.row()  <<  targetRadio.faceMarkInternal
	layout.row()  <<  targetRadio.boundary
	layout.row()  <<  targetRadio.internal
	layout.row()
	layout.finalise()

