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
from Britefury.Mesh.Mesh import GSProductMesh, pyMeshCutTargetList_to_CppMeshCutTargetList, CppMeshCutTargetList, MeshCutTargetType, MeshCutState
from Britefury.Mesh.MeshManipulatorHelper import *

from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit

from Britefury.MeshTools.MeshEdit.MeshCutTarget import MeshCutTarget




class ProcMeshCut (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Cut' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		cppTargets = CppMeshCutTargetList()
		pyMeshCutTargetList_to_CppMeshCutTargetList( [ target.toCppMeshCutTarget()  for  target in self.targetList ], cppTargets )
		inputObject.cut( cppTargets, None )
		return inputObject




class MeshCutManipulator (Manipulator, MeshManipulatorHelper, Viewport3dPainter):
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
		super( MeshCutManipulator, self ).__init__()
		self._cell = None
		self._cutStateCell = None

		self._targetType = MeshCutTargetType.EDGE
		self._vertexPosition = None
		self._edgeSegment = None
		self._edgeSplitPosition = None
		self._faceCurrentPoint = None
		self._facePoints = []
		self._faceBValid = False
		self._bTargetValid = False

		self._o_setUISensitivity( False )



	def manipAttachToView(self, view):
		super( MeshCutManipulator, self ).manipAttachToView( view )
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
		super( MeshCutManipulator, self ).manipDetachFromView()


	def attachCells(self, cell, cutStateCell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._cutStateCell = cutStateCell
		self._cell.changedSignal.connect( self._p_onCellChanged )
		self._o_watchCellValidity( self._cell )
		self._o_watchCellValidity( self._cutStateCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._cell is not None, 'not attached to a cell'
		self._cell.changedSignal.disconnect( self._p_onCellChanged )
		self._o_stopWatchingCellValidity( self._cell )
		self._o_stopWatchingCellValidity( self._cutStateCell )
		self._cell = None
		self._cutStateCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshManipulatorHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False



	def evPaint3d(self, viewport, paintLayer):
		super( MeshCutManipulator, self ).evPaint3d( viewport, paintLayer )

		if self._vertexPosition is not None  and  self._targetType == MeshCutTargetType.VERTEX:
			if self._bTargetValid:
				ModelDraw.drawVertexPickHighlight( self._vertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawVertexPickHighlight( self._vertexPosition, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer );

			if self._p_hasPreviousCutPoint()  and  self._bTargetValid:
				ModelDraw.drawSegment( Segment3( self._p_getPreviousCutPoint(), self._vertexPosition ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer );

		if self._edgeSegment is not None  and  self._targetType == MeshCutTargetType.EDGE:
			if self._bTargetValid:
				ModelDraw.drawEdgePickHighlight( self._edgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawEdgePickHighlight( self._edgeSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer );

			if self._p_hasPreviousCutPoint()  and  self._bTargetValid:
				ModelDraw.drawSegment( Segment3( self._p_getPreviousCutPoint(), self._edgeSplitPosition ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer );

			if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_NODEPTH  and  self._edgeSplitPosition is not None  and  self._bTargetValid:
				ModelDraw.drawSplitPosition( self._edgeSplitPosition );

		if self._faceCurrentPoint is not None  and  self._targetType == MeshCutTargetType.FACE:
			if self._p_hasPreviousCutPoint()  and  self._bTargetValid:
				ModelDraw.drawSegment( Segment3( self._p_getPreviousCutPoint(), self._faceCurrentPoint ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer );

			if self._bTargetValid:
				ModelDraw.drawCircleHighlight( self._faceCurrentPoint, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer );
			else:
				ModelDraw.drawCircleHighlight( self._faceCurrentPoint, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer );


		if paintLayer == PaintLayer.PAINTLAYER_WIREFRAME  or  paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
			cutState = self._p_getCutState()
			if cutState is not None:
				if cutState.lastSplitVertexIndex != -1:
					lastSplitVertexPosition = cutState.lastSplitVertexPosition
					ModelDraw.drawVertexReticle( lastSplitVertexPosition, paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY )

					if len( self._facePoints ) > 0:
						ModelDraw.drawSegment( Segment3( lastSplitVertexPosition, self._facePoints[0] ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

				ModelDraw.drawLineStrip( self._facePoints, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

				for point in self._facePoints:
					ModelDraw.drawCircleHighlight( point, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )



	def _p_getVertexTarget(self, viewport, cutState, mesh, targetPick, targetVertexIndex):
		self._targetType = MeshCutTargetType.VERTEX
		target = MeshCutTarget( deepcopy( targetPick ), MeshCutTargetType.VERTEX )

		# Target vertices
		vertexIndex, thruFaceIndex = mesh.pickVertex( targetPick.toMPick() )

		if vertexIndex != targetVertexIndex:
			targetVertexIndex = vertexIndex

			if targetVertexIndex != -1:
				self._vertexPosition = mesh.getVertexPosition( vertexIndex )
				self._bTargetValid = mesh.isValidCutTarget( target.toCppMeshCutTarget(), cutState )
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
				self._bTargetValid = mesh.isValidCutTarget( target.toCppMeshCutTarget(), cutState )
			else:
				self._edgeSegment = None
				self._bTargetValid = False
			self._view.postRedraw()

		if edgeIndex != -1:
			self._edgeSplitPosition = closestPoint
			self._view.postRedraw()
		else:
			self._edgeSplitPosition = None

		return target, targetEdgeIndex



	def _p_getFaceTarget(self, viewport, cutState, mesh, targetPick, targetFaceIndex):
		self._targetType = MeshCutTargetType.FACE
		target = MeshCutTarget( deepcopy( targetPick ), MeshCutTargetType.FACE )

		# Target faces
		faceIndex, intersectionPoint = mesh.pickFace( targetPick.toMPick() )

		if faceIndex != targetFaceIndex:
			targetFaceIndex = faceIndex

			if targetFaceIndex != -1:
				self._bTargetValid = mesh.isValidCutTarget( target.toCppMeshCutTarget(), cutState )
			else:
				self._faceCurrentPoint = None
				self._bTargetValid = False
			self._view.postRedraw()

		if faceIndex != -1:
			self._faceCurrentPoint = intersectionPoint
			self._view.postRedraw()
		else:
			self._faceCurrentPoint = None

		return target, targetFaceIndex



	def _p_getCutTarget(self, viewport, targetVertexIndex, targetEdgeIndex, targetFaceIndex):
		cutState = self._p_getCutState()
		if cutState is not None:
			targetPick = viewport.createPick()
			pstat = viewport.pointerStatus
			mesh = self._o_getMesh()

			if pstat.bShiftKey:
				# target vertices
				target, targetVertexIndex = self._p_getVertexTarget( viewport, cutState, mesh, targetPick, targetVertexIndex )
			elif pstat.bControlKey:
				# target faces
				target, targetFaceIndex = self._p_getFaceTarget( viewport, cutState, mesh, targetPick, targetFaceIndex )
			else:
				# target edges
				target, targetEdgeIndex = self._p_getEdgeTarget( viewport, cutState, mesh, targetPick, targetEdgeIndex )

			return target, targetVertexIndex, targetEdgeIndex, targetFaceIndex



	def _p_getCutTargetFromUser(self):
		self._targetType = MeshCutTargetType.EDGE
		self._vertexPosition = None
		self._edgeSegment = None
		self._edgeSplitPosition = None
		self._faceCurrentPoint = None
		self._bTargetValid = False
		target = None
		targetVertexIndex = -1
		targetEdgeIndex = -1
		targetFaceIndex = -1

		if self._view.pointerViewport is not None:
			target, targetVertexIndex, targetEdgeIndex, targetFaceIndex = self._p_getCutTarget( self._view.pointerViewport, targetVertexIndex, targetEdgeIndex, targetFaceIndex )

		event = self._coEventHandler.waitForNextEvent()

		# Get the target component
		while not ( event.eventType == self.EV_BUTTON_DOWN   and  self._bTargetValid ):
			if event.eventType == self.EV_RESET:
				self._view.postRedraw()
				raise self.ResetException

			viewport = event.viewport

			if event.eventType != self.EV_LEAVE  and  viewport is not None:
				target, targetVertexIndex, targetEdgeIndex, targetFaceIndex = self._p_getCutTarget( viewport, targetVertexIndex, targetEdgeIndex, targetFaceIndex )

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
		super( MeshCutManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=False ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshCutManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_UP, viewport=viewport, bScaledMotion=False ) )


	def evSecondaryButtonDown(self, viewport):
		super( MeshCutManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=True ) )

	def evSecondaryButtonUp(self, viewport):
		super( MeshCutManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( self.Event( self.EV_BUTTON_UP, viewport=viewport, bScaledMotion=True ) )


	def evMotion(self, viewport, point):
		super( MeshCutManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( self.Event( self.EV_MOTION, viewport=viewport ) )

	def evEnter(self, viewport, point):
		super( MeshCutManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( self.Event( self.EV_MOTION, viewport=viewport ) )

	def evLeave(self, viewport, point):
		super( MeshCutManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( self.Event( self.EV_LEAVE, viewport=viewport ) )



	def _o_reset(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( self.EV_RESET ) )



	def _p_hasPreviousCutPoint(self):
		cutState = self._p_getCutState()
		if cutState is not None:
			return cutState.lastSplitVertexIndex != -1  and  self._faceBValid
		return False


	def _p_getPreviousCutPoint(self):
		cutState = self._p_getCutState()
		if cutState is not None:
			if self._faceBValid  and  len( self._facePoints ) > 0:
				return self._facePoints[-1]
			else:
				return cutState.lastSplitVertexPosition
		return None


	def _p_onCellChanged(self):
		self._facePoints = []
		self._faceBValid = True

		targetList = self._cell.value

		if targetList is not None:
			lastVertexOrEdgeTargetIndex = -1
			for i, target in reversed( list( enumerate( targetList ) ) ):
				if target.targetType == MeshCutTargetType.VERTEX  or  target.targetType == MeshCutTargetType.EDGE:
					lastVertexOrEdgeTargetIndex = i
					break

			if lastVertexOrEdgeTargetIndex != -1:
				mesh = self._o_getMesh()

				if mesh is not None:
					for i in xrange( lastVertexOrEdgeTargetIndex + 1, len( targetList ) ):
						faceIndex, pointOnFace = mesh.pickFace( targetList[i].pick.toMPick() )

						if faceIndex != -1:
							self._facePoints.append( pointOnFace )
							self._faceBValid = True
						else:
							self._facePoints = []
							self._faceBValid = False
							break
		else:
			self._facePoints = []
			self._faceBValid = False

		self._o_reset()


	def _p_getCutState(self):
		if self._cutStateCell is not None:
			return self._cutStateCell.getValue()
		return None





class MeshCutTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, cutTargetsField, cutStateField):
		super( MeshCutTargetListFieldManipulator, self ).__init__()
		self._cutTargetsField = cutTargetsField
		self._cutStateField = cutStateField


	def createElement(self, sheetEditContext):
		return MeshCutManipulator()

	def initialiseElement(self, element, sheetEditContext):
		cutTargetsCell = self._p_getCell( self._cutTargetsField, sheetEditContext )
		cutStateCell = self._p_getCell( self._cutStateField, sheetEditContext )
		element.attachCells( cutTargetsCell, cutStateCell )
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




class MeshCutAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshCut

	def __init__(self, *args, **kwargs):
		super( MeshCutAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.cutState = MeshCutState()


	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTargets = CppMeshCutTargetList()
		pyMeshCutTargetList_to_CppMeshCutTargetList( [ target.toCppMeshCutTarget()  for  target in self._proc.targetList ], cppTargets )
		self.mesh.cut( cppTargets, self.cutState )

	def getResult(self):
		return self.mesh




class ToolMeshCut (GSProcedureTool):
	sheet = SheetRefField( ProcMeshCut )

	functionClass = ProcMeshCut
	adjustableInvoker = MeshCutAdjustableInvoker
	pageTitle = _( 'Cut' )
	titleText = _( 'Cut' )


	def _p_cutState(self):
		try:
			adjustableInvoker = self._adjustableInvoker
		except AttributeError:
			return None

		if adjustableInvoker is not None:
			return adjustableInvoker.cutState

	_cutState = FunctionRefField( _p_cutState )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Cut targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshCutTargetListFieldManipulator( sheet.targetList, _cutState )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Cut shapes into a mesh\n'  'Click to add points to the cut path.\n'  'Edges will be targeted, or:\n'  'Hold <shift> to target vertices\n'  'Hold <control> to place points in faces.\nNote that points in faces must be preceeded and followed by vertex or edge points' ) )
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



