##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from OpenGL.GL import *

from copy import deepcopy

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Util.CoEventHandler import CoEventHandler

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Mesh.Mesh import GSProductMesh
from Britefury.Mesh.MeshPickHelper import *

from Britefury.Model import ModelDraw
from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.Model import pyMPickList_to_MPickList, MPickList
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit





class ProcMeshSplitEdges (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Split edges' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.targetList ], mpicks )
		inputObject.splitEdges( mpicks )
		return inputObject




class MeshSplitEdgesManipulator (Manipulator, MeshPickHelper):
	def __init__(self):
		super( MeshSplitEdgesManipulator, self ).__init__()
		self._cell = None

		self._highlightedEdgeSegment = None
		self._splitPoint = None

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( MeshSplitEdgesManipulator, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshSplitEdgesManipulator, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'
		self._o_stopWatchingCellValidity( self._cell )
		self._cell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshPickHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False



	def _p_refreshHit(self, viewport, mesh, pick):
		pickedEdgeIndex, self._splitPoint, pickedFaceIndex = mesh.pickEdge( pick.toMPick() )

		if pickedEdgeIndex != -1:
			self._highlightedEdgeSegment = mesh.getEdgeSegment( pickedEdgeIndex )
			return True
		else:
			return False



	def _p_paintHighlight(self, viewport, paintLayer):
		# Highlight the picked edge
		ModelDraw.drawEdgePickHighlight( self._highlightedEdgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )

		# Draw the split position on the seed edge
		ModelDraw.drawSplitPosition( self._splitPoint )


	def _p_pickConfirmed(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._cell is not None, 'no cell attached'
		self._cell.literalValue = self._cell.value + [ deepcopy( pick ) ]
		self._p_commandHistoryBreak()


	def _o_resetHighlight(self):
		self._highlightedEdgeSegment = None
		self._splitPoint = None


	def _p_onParamsChanged(self):
		self._o_resetHighlight()
		self._o_requestHighlightRefresh()









class MeshSplitEdgesTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, field):
		super( MeshSplitEdgesTargetListFieldManipulator, self ).__init__()
		self._field = field


	def createElement(self, rowEditContext):
		manip = MeshSplitEdgesManipulator()
		return manip

	def initialiseElement(self, element, rowEditContext):
		element.attachCell( self._p_getCell( self._field, rowEditContext ) )
		if rowEditContext.commandHistory is not None:
			element.commandHistory = rowEditContext.commandHistory

	def shutdownElement(self, element, rowEditContext):
		element.detachCell()
		element.commandHistory = None


	def installElementEventHandlers(self, element, rowEditContext):
		element.manipAttachToView( rowEditContext.view )

	def uninstallElementEventHandlers(self, element, rowEditContext):
		element.manipDetachFromView()


	def attachOutputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachOutputProductCell(self, element, cell):
		element.detachMeshCell()




class MeshSplitEdgesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshSplitEdges

	def __init__(self, *args, **kwargs):
		super( MeshSplitEdgesAdjustableInvoker, self ).__init__( *args, **kwargs )


	def main(self):
		self.mesh = deepcopy( self._inputObject )
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.targetList ], mpicks )
		self.mesh.splitEdges( mpicks )

	def getResult(self):
		return self.mesh




class ToolMeshSplitEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshSplitEdges )

	functionClass = ProcMeshSplitEdges
	adjustableInvoker = MeshSplitEdgesAdjustableInvoker
	pageTitle = _( 'Split' )
	titleText = _( 'Split edges' )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Split targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshSplitEdgesTargetListFieldManipulator( sheet.targetList )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click to insert vertices into edges' ) )
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



