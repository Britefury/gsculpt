##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Colour3f, Point3, Segment3

from Britefury.Util.CoEventHandler import CoEventHandler

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegisterPrimitive, ioObjectFactoryRegister

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
from Britefury.Mesh.Mesh import GSProductMesh, pyMeshWeldTargetList_to_CppMeshWeldTargetList, CppMeshWeldTarget, CppMeshWeldTargetList, MeshTargetWeldVerticesTargetType
from Britefury.Mesh.MeshManipulatorHelper import *

from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit






weldTargetTypeIOXmlEnum = IOXmlEnum( MeshTargetWeldVerticesTargetType, 'MeshTargetWeldVerticesTargetType', MeshTargetWeldVerticesTargetType.EDGE )
weldTargetTypeIOXmlEnum.value( MeshTargetWeldVerticesTargetType.VERTEX, 'vertex' )
weldTargetTypeIOXmlEnum.value( MeshTargetWeldVerticesTargetType.EDGE_CLOSEST_POINT_TO_SOURCE, 'edge_closest_point_to_source' )
weldTargetTypeIOXmlEnum.value( MeshTargetWeldVerticesTargetType.EDGE, 'edge' )

ioXmlReadMeshWeldTargetTypeProp = weldTargetTypeIOXmlEnum.propReader
ioXmlReadMeshWeldTargetTypeNode = weldTargetTypeIOXmlEnum.nodeReader
ioXmlWriteMeshWeldTargetTypeProp = weldTargetTypeIOXmlEnum.propWriter
ioXmlWriteMeshWeldTargetTypeNode = weldTargetTypeIOXmlEnum.nodeWriter





class MeshWeldVerticesTarget (object):
	def __init__(self, sourcePick = None, targetPick = None, targetType = MeshTargetWeldVerticesTargetType.VERTEX):
		if sourcePick is None:
			sourcePick = Pick3d()
		if targetPick is None:
			targetPick = Pick3d()
		self.sourcePick = sourcePick
		self.targetPick = targetPick
		self.targetType = targetType


	def toCppMeshWeldTarget(self):
		return CppMeshWeldTarget( self.sourcePick.toMPick(), self.targetPick.toMPick(), self.targetType )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'source' )  >>  self.sourcePick
		xmlNode.getChild( 'target' )  >>  self.targetPick
		self.targetType = ioXmlReadMeshWeldTargetTypeNode( xmlNode.getChild( 'target_type' ), self.targetType )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'source' )  <<  self.sourcePick
		xmlNode.addChild( 'target' )  <<  self.targetPick
		ioXmlWriteMeshWeldTargetTypeNode( xmlNode.addChild( 'target_type' ), self.targetType )


ioObjectFactoryRegister( 'MeshWeldVerticesTarget', MeshWeldVerticesTarget )







class ProcMeshTargetWeldVertices (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Target weld vertices' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		cppTargets = CppMeshWeldTargetList()
		pyMeshWeldTargetList_to_CppMeshWeldTargetList( [ target.toCppMeshWeldTarget()  for  target in self.targetList ], cppTargets )
		inputObject.targetWeld( cppTargets )
		return inputObject






class MeshTargetWeldVerticesManipulator (Manipulator, MeshManipulatorHelper, ViewportContextButtonListener, Viewport3dPainter):
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
		super( MeshTargetWeldVerticesManipulator, self ).__init__()
		self._cell = None

		self._coEventHandler = None
		self._storedEvents = []

		self._vertexHighlightPosition = None
		self._edgeHighlightSegment = None
		self._edgeHighlightSplitPosition = None
		self._sourceVertexHighlightPosition = None
		self._bTargetValid = True

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( MeshTargetWeldVerticesManipulator, self ).manipAttachToView( view )
		self._coEventHandler = CoEventHandler( self._p_targetWeldLoop )
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
		super( MeshTargetWeldVerticesManipulator, self ).manipDetachFromView()


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




	def _p_getSourcePick(self, viewport, sourceVertexIndex):
		mesh = self._o_getMesh()
		sourcePick = viewport.createPick()
		vertexIndex, thruFaceIndex = mesh.pickVertex( sourcePick.toMPick() )

		if vertexIndex != sourceVertexIndex:
			if vertexIndex != -1:
				self._vertexHighlightPosition = mesh.getVertexPosition( vertexIndex )
			else:
				self._vertexHighlightPosition = None
			self._view.postRedraw()
			sourceVertexIndex = vertexIndex

		return sourcePick, sourceVertexIndex




	def _p_getTargetPick(self, viewport, sourceVertexIndex, targetVertexIndex, targetEdgeIndex, targetType):
		mesh = self._o_getMesh()
		targetPick = viewport.createPick()

		if viewport.pointerStatus.bShiftKey:
			targetVertexIndex = -1
			self._vertexHighlightPosition = None

			# Target is the closest point on the picked edge
			edgeIndex, closestPoint, thruFaceIndex = mesh.pickEdge( targetPick.toMPick() )

			if edgeIndex != targetEdgeIndex  or  targetType != MeshTargetWeldVerticesTargetType.EDGE_CLOSEST_POINT_TO_SOURCE:
				targetType = MeshTargetWeldVerticesTargetType.EDGE_CLOSEST_POINT_TO_SOURCE
				if edgeIndex != -1:
					self._edgeHighlightSegment = mesh.getEdgeSegment( edgeIndex )
					self._edgeHighlightSplitPosition, t = self._edgeHighlightSegment.closestPointTo( self._sourceVertexHighlightPosition )
					self._bTargetValid = mesh.isValidWeldTargetEdge( sourceVertexIndex, edgeIndex, targetType )
				else:
					self._edgeHighlightSegment = None
					self._edgeHighlightSplitPosition = None
					self._bTargetValid = True
				self._view.postRedraw()
				targetEdgeIndex = edgeIndex

		elif viewport.pointerStatus.bControlKey:
			targetVertexIndex = -1
			self._vertexHighlightPosition = None

			# Target is the chosen point on the picked edge
			edgeIndex, closestPoint, thruFaceIndex = mesh.pickEdge( targetPick.toMPick() )

			targetType = MeshTargetWeldVerticesTargetType.EDGE
			if edgeIndex != -1:
				self._edgeHighlightSegment = mesh.getEdgeSegment( edgeIndex )
				self._edgeHighlightSplitPosition = closestPoint
				self._bTargetValid = mesh.isValidWeldTargetEdge( sourceVertexIndex, edgeIndex, targetType )
			else:
				self._edgeHighlightSegment = None
				self._edgeHighlightSplitPosition = None
				self._bTargetValid = True
			self._view.postRedraw()
			targetEdgeIndex = edgeIndex

		else:
			targetEdgeIndex = -1
			self._edgeHighlightSegment = None
			self._edgeHighlightSplitPosition = None

			# Target is the picked vertex
			vertexIndex, thruFaceIndex = mesh.pickVertex( targetPick.toMPick() )

			if vertexIndex != targetVertexIndex  or  targetType != MeshTargetWeldVerticesTargetType.VERTEX:
				targetType = MeshTargetWeldVerticesTargetType.VERTEX
				if vertexIndex != -1:
					self._vertexHighlightPosition = mesh.getVertexPosition( vertexIndex )
					self._bTargetValid = mesh.isValidWeldTargetVertex( sourceVertexIndex, vertexIndex )
				else:
					self._vertexHighlightPosition = None
					self._bTargetValid = True
				self._view.postRedraw()
				targetVertexIndex = vertexIndex

		return targetPick, targetVertexIndex, targetEdgeIndex, targetType



	def _p_getWeldTargetFromUser(self):
		self._vertexHighlightPosition = None
		self._edgeHighlightSegment = None
		self._edgeHighlightSplitPosition = None
		self._sourceVertexHighlightPosition = None
		self._bTargetValid = True
		sourceVertexIndex = -1
		targetVertexIndex = -1
		targetEdgeIndex = -1
		targetType = None
		targetPick = None


		if self._view.pointerViewport is not None:
			sourcePick, sourceVertexIndex = self._p_getSourcePick( self._view.pointerViewport, sourceVertexIndex )

		event = self._coEventHandler.waitForNextEvent()

		# Get the source vertex
		while not ( event.eventType == MeshTargetWeldVerticesManipulator.EVENT_BUTTONDOWN  and  sourceVertexIndex != -1 ):
			if event.eventType == MeshTargetWeldVerticesManipulator.EVENT_RESET:
				self._view.postRedraw()
				raise MeshTargetWeldVerticesManipulator.ResetException

			viewport = event.viewport

			if event.eventType != MeshTargetWeldVerticesManipulator.EVENT_LEAVE  and  viewport is not None:
				sourcePick, sourceVertexIndex = self._p_getSourcePick( viewport, sourceVertexIndex )

			event = self._coEventHandler.waitForNextEvent()


		# We have a source vertex
		self._sourceVertexHighlightPosition = self._vertexHighlightPosition

		# The context button can be pressed by the user to cancel a weld
		self._view.addContextButtonListener( self, Viewport3d )

		# Now, get the target
		try:
			while not ( event.eventType == MeshTargetWeldVerticesManipulator.EVENT_BUTTONUP  and  ( targetVertexIndex != -1  or  targetEdgeIndex != -1 )  and  self._bTargetValid ):
				# Handle a reset event
				if event.eventType == MeshTargetWeldVerticesManipulator.EVENT_RESET:
					self._view.postRedraw()
					raise MeshTargetWeldVerticesManipulator.ResetException

				# Handle a context button event
				if event.eventType == MeshTargetWeldVerticesManipulator.EVENT_CONTEXT:
					self._view.postRedraw()
					raise MeshTargetWeldVerticesManipulator.ContextException

				viewport = event.viewport

				# Get the target pick
				if event.eventType != MeshTargetWeldVerticesManipulator.EVENT_LEAVE  and  viewport is not None:
					targetPick, targetVertexIndex, targetEdgeIndex, targetType = self._p_getTargetPick( viewport, sourceVertexIndex, targetVertexIndex, targetEdgeIndex, targetType )

				event = self._coEventHandler.waitForNextEvent()


			self._view.removeContextButtonListener( self, Viewport3d )
			weldTarget = MeshWeldVerticesTarget( deepcopy( sourcePick ), deepcopy( targetPick ), targetType )
			self._cell.literalValue = self._cell.value + [ weldTarget ]
			self._p_commandHistoryBreak()
		except MeshTargetWeldVerticesManipulator.ContextException:
			self._view.removeContextButtonListener( self, Viewport3d )
			self._view.postRedraw()




	def _p_targetWeldLoop(self):
		while True:
			try:
				self._p_getWeldTargetFromUser()
			except MeshTargetWeldVerticesManipulator.ResetException:
				self._view.postRedraw()





	def evPaint3d(self, viewport, paintLayer):
		super( MeshTargetWeldVerticesManipulator, self ).evPaint3d( viewport, paintLayer )

		if self._vertexHighlightPosition is not None:
			if self._bTargetValid:
				ModelDraw.drawVertexPickHighlight( self._vertexHighlightPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			else:
				ModelDraw.drawVertexPickHighlight( self._vertexHighlightPosition, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )

		if self._edgeHighlightSegment is not None:
			if self._bTargetValid:
				ModelDraw.drawEdgePickHighlight( self._edgeHighlightSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			else:
				ModelDraw.drawEdgePickHighlight( self._edgeHighlightSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )

		if self._edgeHighlightSplitPosition is not None:
			if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_NODEPTH:
				ModelDraw.drawSplitPosition( self._edgeHighlightSplitPosition )

		if self._sourceVertexHighlightPosition is not None:
			if paintLayer == PaintLayer.PAINTLAYER_WIREFRAME  or  paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
				ModelDraw.drawVertexReticle( self._sourceVertexHighlightPosition, paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY )
			if self._bTargetValid:
				if self._vertexHighlightPosition is not None:
					ModelDraw.drawSegment( Segment3( self._sourceVertexHighlightPosition, self._vertexHighlightPosition ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )
				if self._edgeHighlightSplitPosition is not None:
					ModelDraw.drawSegment( Segment3( self._sourceVertexHighlightPosition, self._edgeHighlightSplitPosition ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )





	def evPrimaryButtonDown(self, viewport):
		super( MeshTargetWeldVerticesManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_BUTTONDOWN, viewport=viewport ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshTargetWeldVerticesManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_BUTTONUP, viewport=viewport ) )


	def evMotion(self, viewport, point):
		super( MeshTargetWeldVerticesManipulator, self ).evMotion( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_MOTION, viewport=viewport, point=point ) )

	def evEnter(self, viewport, point):
		super( MeshTargetWeldVerticesManipulator, self ).evEnter( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_MOTION, viewport=viewport, point=point ) )

	def evLeave(self, viewport, point):
		super( MeshTargetWeldVerticesManipulator, self ).evLeave( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_LEAVE, viewport=viewport, point=point ) )

	def evContextButton(self, viewport):
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_CONTEXT, viewport=viewport ) )


	def _p_onCellChanged(self):
		self._o_reset()

	def _o_reset(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_RESET ) )





class MeshTargetWeldVerticesTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, field):
		super( MeshTargetWeldVerticesTargetListFieldManipulator, self ).__init__()
		self._field = field


	def createElement(self, sheetEditContext):
		return MeshTargetWeldVerticesManipulator()

	def initialiseElement(self, element, sheetEditContext):
		cell = self._p_getCell( self._field, sheetEditContext )
		element.attachCell( cell )
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




class ToolMeshTargetWeldVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshTargetWeldVertices )

	functionClass = ProcMeshTargetWeldVertices
	pageTitle = _( 'Tgt.Weld' )
	titleText = _( 'Target Weld' )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Weld targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshTargetWeldVerticesTargetListFieldManipulator( sheet.targetList )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click and drag to weld:\n'  'Click to choose a source vertex, hold and drag to the target vertex.\n'  'Hold <shift> to weld to the closest point on an edge, instead of to a vertex.\n'  'Hold <control> to weld to a chosen point on an edge.\n'  'Repeat the above steps for each weld.' ) )
	layout.row()

	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



