##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Colour3f, Point3, Segment3

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport3d import Viewport3d

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model import ModelDraw, Pick3d
from Britefury.Model.Model import pyMPickList_to_MPickList, MPickList
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit

from Britefury.Mesh.Mesh import GSProductMesh
from Britefury.Mesh.MeshPickHelper import MeshPickHelper

from Britefury.Manipulator.Manipulator import Manipulator




class PickPathState (object):
	def __init__(self):
		self.prevVertexIndex = -1



class ProcMeshPickEdgeLoopPath (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Pick edge loop path' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.targetList ], mpicks )
		inputObject.markEdgeLoopPath_pick( mpicks )
		return inputObject




class MeshPickEdgeLoopPathManipulator (Manipulator, MeshPickHelper):
	def __init__(self):
		super( MeshPickEdgeLoopPathManipulator, self ).__init__()
		self._cell = None
		self._pathStateCell = None

		self._prevVertexPosition = None
		self._targetVertexIndex = None
		self._targetVertexPosition = None
		self._pathEdgeSegments = None
		self._bTargetValid = False

		self._o_setUISensitivity( False )


	def manipAttachToView(self, view):
		super( MeshPickEdgeLoopPathManipulator, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshPickEdgeLoopPathManipulator, self ).manipDetachFromView()


	def attachCells(self, cell, pathStateCell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._pathStateCell = pathStateCell
		self._cell.changedSignal.connect( self._p_onCellChanged )
		self._o_watchCellValidity( self._cell )
		self._o_watchCellValidity( self._pathStateCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._cell is not None, 'not attached to a cell'
		self._cell.changedSignal.disconnect( self._p_onCellChanged )
		self._o_stopWatchingCellValidity( self._cell )
		self._o_stopWatchingCellValidity( self._pathStateCell )
		self._cell = None
		self._pathStateCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshPickHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._cell is not None  and  self._pathStateCell is not None:
			return self._cell.isValid()  and  self._pathStateCell.isValid()
		else:
			return False



	def _p_refreshHit(self, viewport, mesh, pick):
		pathState = self._p_getPathState()
		if pathState is not None  and  pathState.prevVertexIndex != -1:
			self._prevVertexPosition = mesh.getVertexPosition( pathState.prevVertexIndex )
		else:
			self._prevVertexPosition = None
		vertexIndex, thruFaceIndex = mesh.pickVertex( pick.toMPick() )
		if vertexIndex != -1:
			if vertexIndex != self._targetVertexIndex:
				self._targetVertexIndex = vertexIndex
				self._targetVertexPosition = mesh.getVertexPosition( vertexIndex )
				if pathState is not None  and  pathState.prevVertexIndex != -1:
					self._bTargetValid, pathEdgeIndices = mesh.discoverEdgeLoopPath( pathState.prevVertexIndex, vertexIndex )
					if self._bTargetValid:
						self._pathEdgeSegments = [ mesh.getEdgeSegment( pathEdgeIndex )   for pathEdgeIndex in pathEdgeIndices ]
					else:
						self._pathEdgeSegments = None
				else:
					self._pathEdgeSegments = None
					self._bTargetValid = True
			return True
		else:
			self._targetVertexIndex = None
			self._targetVertexPosition = None
			self._pathEdgeSegments = None
			self._bTargetvalid = False
			return False



	def _p_paintHighlight(self, viewport, paintLayer):
		if self._bTargetValid:
			ModelDraw.drawVertexPickHighlight( self._targetVertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
		else:
			ModelDraw.drawVertexPickHighlight( self._targetVertexPosition, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )

		if self._pathEdgeSegments is not None:
			for seg in self._pathEdgeSegments:
				ModelDraw.drawEdgePickHighlight( seg, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )

		if self._prevVertexPosition is not None:
			ModelDraw.drawVertexPickHighlight( self._prevVertexPosition, ModelDraw.PATH_HIGHLIGHT_COLOUR, paintLayer )


	def _p_pickConfirmed(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._cell is not None, 'no cell attached'
		self._cell.literalValue = self._cell.value + [ deepcopy( pick ) ]
		self._p_commandHistoryBreak()


	def _o_resetHighlight(self):
		"Protected - reset the highlight"
		self._prevVertexPosition = None
		self._targetVertexIndex = None
		self._targetVertexPosition = None
		self._pathEdgeSegments = None
		self._bTargetValid = False


	def _p_onCellChanged(self):
		"Cell changed"
		self._o_resetHighlight()
		self._o_requestHighlightRefresh()


	def _p_getPathState(self):
		if self._pathStateCell is not None:
			return self._pathStateCell.getValue()
		else:
			return None





class MeshPickEdgeLoopPathFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, pathField, pathStateField):
		super( MeshPickEdgeLoopPathFieldManipulator, self ).__init__()
		self._pathField = pathField
		self._pathStateField = pathStateField


	def createElement(self, sheetEditContext):
		return MeshPickEdgeLoopPathManipulator()

	def initialiseElement(self, element, sheetEditContext):
		pathCell = self._p_getCell( self._pathField, sheetEditContext )
		pathStateCell = self._p_getCell( self._pathStateField, sheetEditContext )
		element.attachCells( pathCell, pathStateCell )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCells()
		element.commandHistory = None


	def installElementEventHandlers(self, element, sheetEditContext):
		element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.manipDetachFromView()


	def attachInputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachInputProductCell(self, element, cell):
		element.detachMeshCell()




class MeshPickEdgeLoopPathAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshPickEdgeLoopPath

	def __init__(self, *args, **kwargs):
		super( MeshPickEdgeLoopPathAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.pathState = PickPathState()


	def save(self):
		self._inputObject.markEdges_save()

	def restore(self):
		self._inputObject.markEdges_restore()

	def adjust(self):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.targetList ], mpicks )
		self._inputObject.markEdgeLoopPath_pick( mpicks )

		if len( self._proc.targetList ) > 0:
			self.pathState.prevVertexIndex, thruFaceIndex = self._inputObject.pickVertex( self._proc.targetList[-1].toMPick() )
		else:
			self.pathState.prevVertexIndex = -1

	def getResult(self):
		return self._inputObject




class ToolMeshPickEdgeLoopPath (GSProcedureTool):
	sheet = SheetRefField( ProcMeshPickEdgeLoopPath )

	functionClass = ProcMeshPickEdgeLoopPath
	adjustableInvoker = MeshPickEdgeLoopPathAdjustableInvoker
	pageTitle = _( 'LoopPth.' )
	titleText = _( 'Pick Edge Loop Path' )


	def _p_pathState(self):
		try:
			adjustableInvoker = self._adjustableInvoker
		except AttributeError:
			return None

		if adjustableInvoker is not None:
			return adjustableInvoker.pathState

	_pathState = FunctionRefField( _p_pathState )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Path points:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshPickEdgeLoopPathFieldManipulator( sheet.targetList, _pathState )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



