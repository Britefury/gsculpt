##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Segment3

from Britefury.Util.CoEventHandler import CoEventHandler

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegister

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport import ViewportContextButtonListener
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model import ModelDraw
from Britefury.Mesh.Mesh import GSProductMesh, pyMeshRewireEdgeTargetList_to_CppMeshRewireEdgeTargetList, CppMeshRewireEdgeTarget, CppMeshRewireEdgeTargetList
from Britefury.Mesh.MeshManipulatorHelper import MeshManipulatorHelper

from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit







class MeshRewireEdgesTarget (object):
	def __init__(self, sourcePick = None, targetPick = None):
		if sourcePick is None:
			sourcePick = Pick3d()
		if targetPick is None:
			targetPick = Pick3d()
		self.sourcePick = sourcePick
		self.targetPick = targetPick


	def toCppMeshRewireEdgeTarget(self):
		return CppMeshRewireEdgeTarget( self.sourcePick.toMPick(), self.targetPick.toMPick() )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'source' )  >>  self.sourcePick
		xmlNode.getChild( 'target' )  >>  self.targetPick

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'source' )  <<  self.sourcePick
		xmlNode.addChild( 'target' )  <<  self.targetPick


ioObjectFactoryRegister( 'MeshRewireEdgesTarget', MeshRewireEdgesTarget )







class ProcMeshRewireEdges (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Rewire edges' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		cppTargets = CppMeshRewireEdgeTargetList()
		pyMeshRewireEdgeTargetList_to_CppMeshRewireEdgeTargetList( [ target.toCppMeshRewireEdgeTarget()  for  target in self.targetList ], cppTargets )
		inputObject.rewireEdges( cppTargets )
		return inputObject






class MeshRewireEdgesManipulator (Manipulator, MeshManipulatorHelper, ViewportContextButtonListener, Viewport3dPainter):
	EVENT_BUTTONDOWN = 0
	EVENT_BUTTONUP = 1
	EVENT_MOTION = 2
	EVENT_LEAVE = 3
	EVENT_CONTEXT = 4
	EVENT_RESET = 5


	class Event (object):
		def __init__(self, eventType, **kwargs):
			self.eventType = eventType
			self.__dict__.update( kwargs )

	class ContextException:
		pass

	class ResetException:
		pass




	def __init__(self):
		super( MeshRewireEdgesManipulator, self ).__init__()
		self._cell = None

		self._coEventHandler = None
		self._storedEvents = []

		self._vertexHighlightPosition = None
		self._edgeHighlightSegment = None
		self._oppositeVertexHighlightPosition = None
		self._sourceVertexHighlightPosition = None
		self._bSourceValid = True
		self._bTargetValid = True

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( MeshRewireEdgesManipulator, self ).manipAttachToView( view )
		self._coEventHandler = CoEventHandler( self._p_rewireEdgesLoop )
		self._coEventHandler.start()
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		# Very important that we remove the context button listener here, as if the manipulator is removed before completing
		# the editing cycle (by undo/redo for example), then the context listener can remain, blocking other listeners with a lower priority
		self._view.removeContextButtonListener( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		self._coEventHandler.terminate()
		self._coEventHandler = None
		super( MeshRewireEdgesManipulator, self ).manipDetachFromView()


	def attachCell(self, cell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._cell.changedSignal.connect( self._p_onCellChanged )
		self._o_watchCellValidity( self._cell )
		self._o_refreshSensitivity()

	def detachCell(self):
		assert self._cell is not None, 'not attached to a cell'
		self._cell.changedSignal.disconnect( self._p_onCellChanged )
		self._o_stopWatchingCellValidity( self._cell )
		self._cell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshManipulatorHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._cell is not None:
			return self._cell.isValid()
		else:
			return False




	def _p_getSourcePick(self, viewport, edgeToRewireIndex, sourceVertexIndex):
		mesh = self._o_getMesh()
		sourcePick = viewport.createPick()
		vertexIndex, thruFaceIndex, thruEdgeIndex = mesh.pickVertexThruEdge( sourcePick.toMPick() )

		if vertexIndex != sourceVertexIndex  or  thruEdgeIndex != edgeToRewireIndex:
			if vertexIndex != -1:
				self._vertexHighlightPosition = mesh.getVertexPosition( vertexIndex )
				self._edgeHighlightSegment = mesh.getEdgeSegment( thruEdgeIndex )
				self._bSourceValid = mesh.canEdgeBeRewired( thruEdgeIndex )
			else:
				self._vertexHighlightPosition = None
				self._edgeHighlightSegment = None
			self._view.postRedraw()
			edgeToRewireIndex = thruEdgeIndex
			sourceVertexIndex = vertexIndex

		return sourcePick, edgeToRewireIndex, sourceVertexIndex




	def _p_getTargetPick(self, viewport, edgeToRewireIndex, sourceVertexIndex, targetVertexIndex):
		mesh = self._o_getMesh()
		targetPick = viewport.createPick()

		# Target is the picked vertex
		vertexIndex, thruFaceIndex = mesh.pickVertex( targetPick.toMPick() )

		if vertexIndex != targetVertexIndex:
			if vertexIndex != -1:
				self._vertexHighlightPosition = mesh.getVertexPosition( vertexIndex )
				self._bTargetValid = mesh.isValidRewireEdgeTarget( edgeToRewireIndex, sourceVertexIndex, vertexIndex )
			else:
				self._vertexHighlightPosition = None
				self._bTargetValid = True
			self._view.postRedraw()
			targetVertexIndex = vertexIndex

		return targetPick, targetVertexIndex



	def _p_getRewireEdgeTargetFromUser(self):
		self._vertexHighlightPosition = None
		self._edgeHighlightSegment = None
		self._oppositeVertexHighlightPosition = None
		self._sourceVertexHighlightPosition = None
		self._bTargetValid = True
		edgeToRewireIndex = -1
		sourceVertexIndex = -1
		targetVertexIndex = -1
		targetPick = None


		if self._view.pointerViewport is not None:
			sourcePick, edgeToRewireIndex, sourceVertexIndex = self._p_getSourcePick( self._view.pointerViewport, edgeToRewireIndex, sourceVertexIndex )

		event = self._coEventHandler.waitForNextEvent()

		# Get the source vertex
		while not ( event.eventType == MeshRewireEdgesManipulator.EVENT_BUTTONDOWN  and  sourceVertexIndex != -1  and  self._bSourceValid ):
			if event.eventType == MeshRewireEdgesManipulator.EVENT_RESET:
				self._view.postRedraw()
				raise MeshRewireEdgesManipulator.ResetException

			viewport = event.viewport

			if event.eventType != MeshRewireEdgesManipulator.EVENT_LEAVE  and  viewport is not None:
				sourcePick, edgeToRewireIndex, sourceVertexIndex = self._p_getSourcePick( viewport, edgeToRewireIndex, sourceVertexIndex )

			event = self._coEventHandler.waitForNextEvent()


		# We have a source vertex
		self._sourceVertexHighlightPosition = self._vertexHighlightPosition

		# The context button can be pressed by the user to cancel a rewire
		self._view.addContextButtonListener( self, Viewport3d )

		if edgeToRewireIndex != -1  and  sourceVertexIndex != -1:
			mesh = self._o_getMesh()
			oppositeVertexIndex = mesh.getEdgeOppositeVertexIndex( edgeToRewireIndex, sourceVertexIndex )
			self._oppositeVertexHighlightPosition = mesh.getVertexPosition( oppositeVertexIndex )
		else:
			self._oppositeVertexHighlightPosition = None

		# Now, get the target
		try:
			while not ( event.eventType == MeshRewireEdgesManipulator.EVENT_BUTTONUP  and  targetVertexIndex != -1  and  self._bTargetValid ):
				# Handle a reset event
				if event.eventType == MeshRewireEdgesManipulator.EVENT_RESET:
					self._view.postRedraw()
					raise MeshRewireEdgesManipulator.ResetException

				# Handle a context button event
				if event.eventType == MeshRewireEdgesManipulator.EVENT_CONTEXT:
					self._view.postRedraw()
					raise MeshRewireEdgesManipulator.ContextException

				viewport = event.viewport

				# Get the target pick
				if event.eventType != MeshRewireEdgesManipulator.EVENT_LEAVE  and  viewport is not None:
					targetPick, targetVertexIndex = self._p_getTargetPick( viewport, edgeToRewireIndex, sourceVertexIndex, targetVertexIndex )

				event = self._coEventHandler.waitForNextEvent()


			self._view.removeContextButtonListener( self, Viewport3d )
			rewireTarget = MeshRewireEdgesTarget( deepcopy( sourcePick ), deepcopy( targetPick ) )
			self._cell.literalValue = self._cell.value + [ rewireTarget ]
			self._p_commandHistoryBreak()
		except MeshRewireEdgesManipulator.ContextException:
			self._view.removeContextButtonListener( self, Viewport3d )
			self._view.postRedraw()




	def _p_rewireEdgesLoop(self):
		while True:
			try:
				self._p_getRewireEdgeTargetFromUser()
			except MeshRewireEdgesManipulator.ResetException:
				self._view.postRedraw()





	def evPaint3d(self, viewport, paintLayer):
		super( MeshRewireEdgesManipulator, self ).evPaint3d( viewport, paintLayer )

		if self._vertexHighlightPosition is not None:
			if self._bTargetValid:
				ModelDraw.drawVertexPickHighlight( self._vertexHighlightPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			else:
				ModelDraw.drawVertexPickHighlight( self._vertexHighlightPosition, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )

		if self._edgeHighlightSegment is not None:
			if self._bSourceValid:
				ModelDraw.drawEdgePickHighlight( self._edgeHighlightSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			else:
				ModelDraw.drawEdgePickHighlight( self._edgeHighlightSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )

		if self._sourceVertexHighlightPosition is not None:
			if paintLayer == PaintLayer.PAINTLAYER_WIREFRAME  or  paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
				ModelDraw.drawVertexReticle( self._sourceVertexHighlightPosition, paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY )

		if self._oppositeVertexHighlightPosition is not None  and  self._vertexHighlightPosition is not None  and  self._bTargetValid  and  self._bSourceValid:
			ModelDraw.drawSegment( Segment3( self._oppositeVertexHighlightPosition, self._vertexHighlightPosition ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )




	def evPrimaryButtonDown(self, viewport):
		super( MeshRewireEdgesManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_BUTTONDOWN, viewport=viewport ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshRewireEdgesManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_BUTTONUP, viewport=viewport ) )


	def evMotion(self, viewport, point):
		super( MeshRewireEdgesManipulator, self ).evMotion( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_MOTION, viewport=viewport, point=point ) )

	def evEnter(self, viewport, point):
		super( MeshRewireEdgesManipulator, self ).evEnter( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_MOTION, viewport=viewport, point=point ) )

	def evLeave(self, viewport, point):
		super( MeshRewireEdgesManipulator, self ).evLeave( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_LEAVE, viewport=viewport, point=point ) )

	def evContextButton(self, viewport):
		if self._bSensitive:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_CONTEXT, viewport=viewport ) )


	def _p_onCellChanged(self):
		self._o_reset()

	def _o_reset(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshRewireEdgesManipulator.Event( MeshRewireEdgesManipulator.EVENT_RESET ) )





class MeshRewireEdgesTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, field):
		super( MeshRewireEdgesTargetListFieldManipulator, self ).__init__()
		self._field = field


	def createElement(self, sheetEditContext):
		return MeshRewireEdgesManipulator()

	def initialiseElement(self, element, sheetEditContext):
		element.attachCell( self._p_getCell( self._field, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCell()
		element.commandHistory = None


	def installElementEventHandlers(self, element, sheetEditContext):
		element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.manipDetachFromView()


	def attachOutputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachOutputProductCell(self, element, cell):
		element.detachMeshCell()




class ToolMeshRewireEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshRewireEdges )

	functionClass = ProcMeshRewireEdges
	pageTitle = _( 'Rewire' )
	titleText = _( 'Rewire Edges' )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Rewire targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshRewireEdgesTargetListFieldManipulator( sheet.targetList )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click and drag to rewire:\n'  '1. Click to choose an edge to rewire. Position the pointer such that the vertex at the end of the edge to be rewired is highlighted.\n'  '2. Then choose a target vertex, and drag the end of the edge to that vertex to rewire.\n'  'Repeat the above steps for each rewire operation.' ) )
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



