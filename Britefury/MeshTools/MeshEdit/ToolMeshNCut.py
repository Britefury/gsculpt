##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Segment3

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

from Britefury.Model import ModelDraw
from Britefury.Mesh.Mesh import GSProductMesh, pyMeshCutTargetList_to_CppMeshCutTargetList, CppMeshCutTargetList, MeshCutTargetType, MeshNCutState
from Britefury.Mesh.MeshManipulatorHelper import *

from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit

from MeshCutTarget import MeshCutTarget





class ProcMeshNCut (GSProcedure):
	numCuts = Field( int, 1 )
	targetList = Field( TargetList, TargetList() )

	description = _( 'N-Cut' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		cppTargets = CppMeshCutTargetList()
		pyMeshCutTargetList_to_CppMeshCutTargetList( [ target.toCppMeshCutTarget()  for  target in self.targetList ], cppTargets )
		inputObject.nCut( self.numCuts + 1, cppTargets, None )
		return inputObject




class MeshNCutManipulator (Manipulator, MeshManipulatorHelper, Viewport3dPainter):
	EV_BUTTON_DOWN = 0
	EV_BUTTON_UP = 1
	EV_MOTION = 2
	EV_LEAVE = 3
	EV_RESET = 4


	class Event (object):
		def __init__(self, eventType, **kwargs):
			self.eventType = eventType
			self.__dict__.update( kwargs )


	class ResetException:
		pass



	def __init__(self):
		super( MeshNCutManipulator, self ).__init__()
		self._cell = None
		self._cutStateCell = None

		self._targetType = MeshCutTargetType.EDGE
		self._vertexPosition = None
		self._edgeSegment = None
		self._bTargetValid = False

		self._o_setUISensitivity( False )



	def manipAttachToView(self, view):
		super( MeshNCutManipulator, self ).manipAttachToView( view )
		self._coEventHandler = CoEventHandler( self._p_cutLoop )
		self._coEventHandler.start()
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		self._coEventHandler.terminate()
		self._coEventHandler = None
		super( MeshNCutManipulator, self ).manipDetachFromView()


	def attachCells(self, cell, cutStateCell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._cutStateCell = cutStateCell
		self._o_watchCellValidity( self._cell )
		self._o_watchCellValidity( self._cutStateCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._cell is not None, 'not attached to a cell'
		self._o_stopWatchingCellValidity( self._cell )
		self._o_stopWatchingCellValidity( self._cutStateCell )
		self._cell = None
		self._cutStateCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshManipulatorHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()  and  self._cutStateCell.isValid()
		else:
			return False



	def evPaint3d(self, viewport, paintLayer):
		super( MeshNCutManipulator, self ).evPaint3d( viewport, paintLayer )

		if self._targetType == MeshCutTargetType.VERTEX  and  self._vertexPosition is not None:
			if self._bTargetValid:
				ModelDraw.drawVertexPickHighlight( self._vertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawVertexPickHighlight( self._vertexPosition, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer );
		elif self._targetType == MeshCutTargetType.EDGE  and  self._edgeSegment is not None:
			if self._bTargetValid:
				ModelDraw.drawEdgePickHighlight( self._edgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawEdgePickHighlight( self._edgeSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )


		cutState = self._p_getCutState()
		if cutState is not None:
			if paintLayer == PaintLayer.PAINTLAYER_WIREFRAME  or  paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
				for position in cutState.lastSplitVertexPositions():
					ModelDraw.drawVertexReticle( position, paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY )




	def _p_getVertexTarget(self, viewport, cutState, mesh, targetPick, targetVertexIndex):
		self._targetType = MeshCutTargetType.VERTEX
		target = MeshCutTarget( deepcopy( targetPick ), MeshCutTargetType.VERTEX )

		# Target vertices
		vertexIndex, thruFaceIndex = mesh.pickVertex( targetPick.toMPick() )

		if vertexIndex != targetVertexIndex:
			targetVertexIndex = vertexIndex

			if targetVertexIndex != -1:
				self._vertexPosition = mesh.getVertexPosition( vertexIndex )
				self._bTargetValid = mesh.isValidNCutTarget( target.toCppMeshCutTarget(), cutState )
			else:
				self._vertexPosition = None
				self._bTargetValid = False
			self._view.postRedraw()

		return target, targetVertexIndex



	def _p_getEdgeTarget(self, viewport, cutState, mesh, targetPick, targetEdgeIndex):
		self._targetType = MeshCutTargetType.EDGE
		target = MeshCutTarget( deepcopy( targetPick ), MeshCutTargetType.EDGE )

		# Target edges
		edgeIndex, closestPoint, thruFaceIndex = mesh.pickEdge( targetPick.toMPick() )

		if edgeIndex != targetEdgeIndex:
			targetEdgeIndex = edgeIndex

			if targetEdgeIndex != -1:
				self._edgeSegment = mesh.getEdgeSegment( targetEdgeIndex )
				self._bTargetValid = mesh.isValidNCutTarget( target.toCppMeshCutTarget(), cutState )
			else:
				self._edgeSegment = None
				self._bTargetValid = False
			self._view.postRedraw()

		return target, targetEdgeIndex



	def _p_getCutTarget(self, viewport, targetVertexIndex, targetEdgeIndex):
		cutState = self._p_getCutState()
		if cutState is not None:
			targetPick = viewport.createPick()
			pstat = viewport.pointerStatus
			mesh = self._o_getMesh()

			if pstat.bShiftKey:
				# target vertices
				target, targetVertexIndex = self._p_getVertexTarget( viewport, cutState, mesh, targetPick, targetVertexIndex )
			else:
				# target edges
				target, targetEdgeIndex = self._p_getEdgeTarget( viewport, cutState, mesh, targetPick, targetEdgeIndex )

			return target, targetVertexIndex, targetEdgeIndex



	def _p_getCutTargetFromUser(self):
		self._targetType = MeshCutTargetType.EDGE
		self._vertexPosition = None
		self._edgeSegment = None
		self._bTargetValid = False
		target = None
		targetVertexIndex = -1
		targetEdgeIndex = -1

		if self._view.pointerViewport is not None:
			target, targetVertexIndex, targetEdgeIndex = self._p_getCutTarget( self._view.pointerViewport, targetVertexIndex, targetEdgeIndex )

		event = self._coEventHandler.waitForNextEvent()

		# Get the target component
		while not ( event.eventType == self.EV_BUTTON_DOWN   and  self._bTargetValid ):
			if event.eventType == self.EV_RESET:
				self._view.postRedraw()
				raise self.ResetException

			viewport = event.viewport

			if event.eventType != self.EV_LEAVE  and  viewport is not None:
				target, targetVertexIndex, targetEdgeIndex = self._p_getCutTarget( viewport, targetVertexIndex, targetEdgeIndex )

			event = self._coEventHandler.waitForNextEvent()


		# Add the target to the appropriate list
		if target is not None:
			self._cell.literalValue = self._cell.value + [ target ]
			self._p_commandHistoryBreak()



	def _p_cutLoop(self):
		while True:
			try:
				self._p_getCutTargetFromUser()
			except self.ResetException:
				self._view.postRedraw()




	def evPrimaryButtonDown(self, viewport):
		super( MeshNCutManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=False ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshNCutManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_UP, viewport=viewport, bScaledMotion=False ) )


	def evSecondaryButtonDown(self, viewport):
		super( MeshNCutManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=True ) )

	def evSecondaryButtonUp(self, viewport):
		super( MeshNCutManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_UP, viewport=viewport, bScaledMotion=True ) )


	def evMotion(self, viewport, point):
		super( MeshNCutManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( self.Event( self.EV_MOTION, viewport=viewport ) )

	def evEnter(self, viewport, point):
		super( MeshNCutManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( self.Event( self.EV_MOTION, viewport=viewport ) )

	def evLeave(self, viewport, point):
		super( MeshNCutManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( self.Event( self.EV_LEAVE, viewport=viewport ) )



	def _o_reset(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( self.EV_RESET ) )



	def _p_getCutState(self):
		if self._cutStateCell is not None:
			return self._cutStateCell.getImmutableValue()
		return None







class MeshNCutTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, field, cutStateField):
		super( MeshNCutTargetListFieldManipulator, self ).__init__()
		self._field = field
		self._cutStateField = cutStateField


	def createElement(self, sheetEditContext):
		return MeshNCutManipulator()

	def initialiseElement(self, element, sheetEditContext):
		cell = self._p_getCell( self._field, sheetEditContext )
		cutStateCell = self._p_getCell( self._cutStateField, sheetEditContext )
		element.attachCells( cell, cutStateCell )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCells()
		element.commandHistory = None


	def installElementEventHandlers(self, element, sheetEditContext):
		element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.manipDetachFromView()


	def attachOutputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachOutputProductCell(self, element, cell):
		element.detachMeshCell()




class MeshNCutAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshNCut

	def __init__(self, *args, **kwargs):
		super( MeshNCutAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.cutState = MeshNCutState()


	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTargets = CppMeshCutTargetList()
		pyMeshCutTargetList_to_CppMeshCutTargetList( [ target.toCppMeshCutTarget()  for  target in self._proc.targetList ], cppTargets )
		self.mesh.nCut( self._proc.numCuts + 1, cppTargets, self.cutState )

	def getResult(self):
		return self.mesh




class ToolMeshNCut (GSProcedureTool):
	sheet = SheetRefField( ProcMeshNCut )

	functionClass = ProcMeshNCut
	adjustableInvoker = MeshNCutAdjustableInvoker
	pageTitle = _( 'N-Cut' )
	titleText = _( 'N-Cut' )


	def _p_cutState(self):
		try:
			adjustableInvoker = self._adjustableInvoker
		except AttributeError:
			return None

		if adjustableInvoker is not None:
			return adjustableInvoker.cutState

	_cutState = FunctionRefField( _p_cutState )


	numCutsLabel = FieldLabelWithFn( sheet.numCuts, _( 'Num. cuts:' ) )
	numCutsEntry = IntFieldEditEntry( sheet.numCuts, 1, 1048576, 1, 10 )
	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'N-Cut targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshNCutTargetListFieldManipulator( sheet.targetList, _cutState )
	targetListManip.enable()

	numCutsManip = IntFieldScrollManipulator( sheet.numCuts, 1, 1048576 )
	numCutsManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Cut a path into a mesh.\n'  'Click to add points to the cut path.\n'  'Edges will be targeted. Hold <shift> to target vertices\n'  'Use the scroll wheel to change the number of cuts' ) )
	layout.row()
	layout.row()  <<  numCutsLabel.label  <<  numCutsEntry.entry
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



