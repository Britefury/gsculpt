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
		self.prevEdgeIndex = -1



class ProcMeshPickEdgeRingPath (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Pick edge ring path' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.targetList ], mpicks )
		inputObject.markEdgeRingPath_pick( mpicks )
		return inputObject




class MeshPickEdgeRingPathManipulator (Manipulator, MeshPickHelper):
	def __init__(self):
		super( MeshPickEdgeRingPathManipulator, self ).__init__()
		self._cell = None
		self._pathStateCell = None

		self._prevEdgeSegment = None
		self._targetEdgeIndex = None
		self._targetEdgeSegment = None
		self._pathEdgeSegments = None
		self._pathPoints = None
		self._bTargetValid = False

		self._o_setUISensitivity( False )



	def manipAttachToView(self, view):
		super( MeshPickEdgeRingPathManipulator, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshPickEdgeRingPathManipulator, self ).manipDetachFromView()


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
		edgeIndex, closestPoint, thruFaceIndex = mesh.pickEdge( pick.toMPick() )
		if pathState is not None  and  pathState.prevEdgeIndex != -1:
			self._prevEdgeSegment = mesh.getEdgeSegment( pathState.prevEdgeIndex )
		else:
			self._prevEdgeSegment = None
		if edgeIndex != -1:
			if edgeIndex != self._targetEdgeIndex:
				self._targetEdgeIndex = edgeIndex
				self._targetEdgeSegment = mesh.getEdgeSegment( edgeIndex )
				if pathState is not None  and  pathState.prevEdgeIndex != -1:
					self._bTargetValid, pathEdgeIndices = mesh.discoverEdgeRingPath( pathState.prevEdgeIndex, edgeIndex )
					if self._bTargetValid:
						self._pathEdgeSegments = [ mesh.getEdgeSegment( pathEdgeIndex )   for pathEdgeIndex in pathEdgeIndices ]
						self._pathPoints = [ seg.getMidPoint()   for seg in self._pathEdgeSegments ]
					else:
						self._pathEdgeSegments = None
						self._pathPoints = None
				else:
					self._pathEdgeSegments = None
					self._pathPoints = None
					self._bTargetValid = True
			return True
		else:
			self._targetEdgeIndex = None
			self._targetEdgeSegment = None
			self._pathEdgeSegments = None
			self._pathPoints = None
			self._bTargetvalid = False
			return False



	def _p_paintHighlight(self, viewport, paintLayer):
		if self._pathEdgeSegments is not None:
			for seg in self._pathEdgeSegments:
				ModelDraw.drawEdgePickHighlight( seg, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			if self._pathPoints is not None:
				ModelDraw.drawPickPath( self._pathPoints, ModelDraw.PATH_HIGHLIGHT_COLOUR, paintLayer )
		else:
			if self._bTargetValid:
				ModelDraw.drawEdgePickHighlight( self._targetEdgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			else:
				ModelDraw.drawEdgePickHighlight( self._targetEdgeSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )
		if self._prevEdgeSegment is not None:
			ModelDraw.drawEdgePickHighlight( self._prevEdgeSegment, ModelDraw.PATH_HIGHLIGHT_COLOUR, paintLayer )



	def _p_pickConfirmed(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._cell is not None, 'no cell attached'
		self._cell.literalValue = self._cell.value + [ deepcopy( pick ) ]
		self._p_commandHistoryBreak()


	def _o_resetHighlight(self):
		"Protected - reset the highlight"
		self._prevEdgeSegment = None
		self._targetEdgeIndex = None
		self._targetEdgeSegment = None
		self._pathEdgeSegments = None
		self._pathPoints = None
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





class MeshPickEdgeRingPathFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, pathField, pathStateField):
		super( MeshPickEdgeRingPathFieldManipulator, self ).__init__()
		self._pathField = pathField
		self._pathStateField = pathStateField


	def createElement(self, sheetEditContext):
		return MeshPickEdgeRingPathManipulator()

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




class MeshPickEdgeRingPathAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshPickEdgeRingPath

	def __init__(self, *args, **kwargs):
		super( MeshPickEdgeRingPathAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.pathState = PickPathState()


	def save(self):
		self._inputObject.markEdges_save()

	def restore(self):
		self._inputObject.markEdges_restore()

	def adjust(self):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.targetList ], mpicks )
		self._inputObject.markEdgeRingPath_pick( mpicks )

		if len( self._proc.targetList ) > 0:
			self.pathState.prevEdgeIndex, closestPoint, thruFaceIndex = self._inputObject.pickEdge( self._proc.targetList[-1].toMPick() )
		else:
			self.pathState.prevEdgeIndex = -1

	def getResult(self):
		return self._inputObject




class ToolMeshPickEdgeRingPath (GSProcedureTool):
	sheet = SheetRefField( ProcMeshPickEdgeRingPath )

	functionClass = ProcMeshPickEdgeRingPath
	adjustableInvoker = MeshPickEdgeRingPathAdjustableInvoker
	pageTitle = _( 'RingPth.' )
	titleText = _( 'Pick Edge Ring Path' )


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

	targetListManip = MeshPickEdgeRingPathFieldManipulator( sheet.targetList, _pathState )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



