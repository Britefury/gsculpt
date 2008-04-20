##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.Math.Math import Point3, Segment3

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegister

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport import ViewportContextButtonListener
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.UI.FormLayout import FormLayout

from Britefury.Util.CoEventHandler import CoEventHandler

from Britefury.Kernel.Enum import *

from Britefury.Cell.Cell import *

from Britefury.CellEdit.ChoiceCellEditRadioGroup import ChoiceCellEditRadioGroup

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureTool import *
from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker

from Britefury.Manipulator.Manipulator import *
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model import ModelDraw
from Britefury.Mesh.Mesh import GSProductMesh, pyMeshDrawFacePointList_to_MeshDrawFacePointList, CppMeshDrawFacePoint, CppMeshDrawFacePointList, MeshDrawFacePointTarget, MeshDrawFaceState, MeshDrawFaceStateVertexCheckResult
from Britefury.Mesh.MeshManipulatorHelper import MeshManipulatorHelper

from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit
from Britefury.Model.Model import GSProductModelBlank






drawFaceTargetIOXmlEnum = IOXmlEnum( MeshDrawFacePointTarget, 'MeshDrawFacePointTarget', MeshDrawFacePointTarget.NEWPOINT )
drawFaceTargetIOXmlEnum.value( MeshDrawFacePointTarget.NEWPOINT, 'new_point' )
drawFaceTargetIOXmlEnum.value( MeshDrawFacePointTarget.EXISTINGVERTEX, 'existing_vertex' )

ioXmlReadMeshDrawFacePointTargetProp = drawFaceTargetIOXmlEnum.propReader
ioXmlReadMeshDrawFacePointTargetNode = drawFaceTargetIOXmlEnum.nodeReader
ioXmlWriteMeshDrawFacePointTargetProp = drawFaceTargetIOXmlEnum.propWriter
ioXmlWriteMeshDrawFacePointTargetNode = drawFaceTargetIOXmlEnum.nodeWriter







class MeshDrawFacePoint (object):
	def __init__(self, pick=None, position=None, target=MeshDrawFacePointTarget.NEWPOINT):
		if pick is None:
			pick = Pick3d()
		if position is None:
			position = Point3()
		self.pick = pick
		self.position = position
		self.target = target


	def toCppMeshDrawFacePoint(self):
		return CppMeshDrawFacePoint( self.pick.toMPick(), self.position, self.target )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		xmlNode.getChild( 'position' )  >>  self.position
		self.target = ioXmlReadMeshDrawFacePointTargetNode( xmlNode.getChild( 'target' ), self.target )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		xmlNode.addChild( 'position' )  <<  self.position
		ioXmlWriteMeshDrawFacePointTargetNode( xmlNode.addChild( 'target' ), self.target )


ioObjectFactoryRegister( 'MeshDrawFacePoint', MeshDrawFacePoint )







class ProcMeshDrawFace (GSProcedure):
	pointList = Field( TargetList, TargetList() )
	bFlip = Field( bool, False )

	description = _( 'Draw face' )
	inputClass = [ GSProductModelBlank, GSProductMesh ]
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		if isinstance( inputObject, GSProductMesh ):
			mesh = inputObject
		else:
			mesh = GSProductMesh()

		cppPoints = CppMeshDrawFacePointList()
		pyMeshDrawFacePointList_to_MeshDrawFacePointList( [ point.toCppMeshDrawFacePoint()  for  point in self.pointList ], cppPoints )
		mesh.drawFace( cppPoints, self.bFlip  )
		return mesh






class MeshDrawFaceManipulator (Manipulator, MeshManipulatorHelper, ViewportContextButtonListener, Viewport3dPainter):
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

	class ResetException:
		pass

	class TerminateException:
		pass





	def __init__(self):
		super( MeshDrawFaceManipulator, self ).__init__()
		self._pointListCell = None
		self._stateCell = None

		self._coEventHandler = None
		self._storedEvents = []

		self._vertexHighlightPosition = None
		self._bTargetValid = True
		self._bClosingEdgeValid = True
		self._pointHighlightPosition = None

		self._bDisplayRequiresRefresh = True
		self._displayPoints = []
		self._displaySegs = []
		self._displayClosingSeg = []

		self._bComplete = False

		self._o_setUISensitivity( False )







	def manipAttachToView(self, view):
		super( MeshDrawFaceManipulator, self ).manipAttachToView( view )
		self._coEventHandler = CoEventHandler( self._p_manipulatorLoop )
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
		super( MeshDrawFaceManipulator, self ).manipDetachFromView()


	def attachCells(self, pointListCell, stateCell):
		assert self._pointListCell is None, 'already attached to a cell'
		self._pointListCell = pointListCell
		self._stateCell = stateCell
		self._pointListCell.changedSignal.connect( self._p_onPointListChanged )
		self._o_watchCellValidity( self._pointListCell )
		self._o_watchCellValidity( self._stateCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._pointListCell is not None, 'not attached to a cell'
		self._pointListCell.changedSignal.disconnect( self._p_onPointListChanged )
		self._o_stopWatchingCellValidity( self._pointListCell )
		self._o_stopWatchingCellValidity( self._stateCell )
		self._pointListCell = None
		self._stateCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshManipulatorHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._pointListCell is not None  and  self._stateCell is not None:
			return self._pointListCell.isValid()  and  self._stateCell.isValid()
		else:
			return False



	def _p_getPoint(self, event, existingVertexIndex, target):
		mesh = self._o_getMesh()
		pick = event.viewport.createPick()
		position = event.viewport.computeDrawPoint()
		self._pointHighlightPosition = position

		self._bTargetValid = True
		self._bClosingEdgeValid = True

		state = self._p_getState()
		if state is not None:
			if event.viewport.pointerStatus.bShiftKey:
				if mesh is not None:
					vertexIndex, thruFaceIndex = mesh.pickVertex( pick.toMPick() )

					if vertexIndex != existingVertexIndex  or  target != MeshDrawFacePointTarget.EXISTINGVERTEX:
						target = MeshDrawFacePointTarget.EXISTINGVERTEX
						if vertexIndex != -1:
							self._vertexHighlightPosition = mesh.getVertexPosition( vertexIndex )
							self._pointHighlightPosition = self._vertexHighlightPosition
							checkResult = state.checkVertex( vertexIndex )
							self._bTargetValid = checkResult != MeshDrawFaceStateVertexCheckResult.INVALID
							self._bClosingEdgeValid = checkResult == MeshDrawFaceStateVertexCheckResult.GOOD
						else:
							self._vertexHighlightPosition = None
						self._view.postRedraw()
						existingVertexIndex = vertexIndex
					elif vertexIndex == -1:
						self._view.postRedraw()
				else:
					existingVertexIndex = None
					self._vertexHighlightPosition = None
					self._view.postRedraw()
			else:
				target = MeshDrawFacePointTarget.NEWPOINT
				existingVertexIndex = None
				self._vertexHighlightPosition = None
				self._view.postRedraw()

			return pick, position, existingVertexIndex, target
		else:
			return None, None, None, None




	def _p_getPointFromUser(self):
		self._vertexHighlightPosition = None
		self._bTargetValid = True
		self._bClosingEdgeValid = True
		self._pointHighlightPosition = None

		pick = None
		position = None
		existingVertexIndex = None
		target = None



		event = self._coEventHandler.waitForNextEvent()



		# Get the point
		while event.eventType != MeshDrawFaceManipulator.EVENT_BUTTONDOWN  or  not self._bTargetValid:
			if event.eventType == MeshDrawFaceManipulator.EVENT_RESET:
				self._view.postRedraw()
				raise MeshDrawFaceManipulator.ResetException

			if event.eventType == MeshDrawFaceManipulator.EVENT_CONTEXT:
				self._view.postRedraw()
				self._view.removeContextButtonListener( self, Viewport3d )
				raise MeshDrawFaceManipulator.TerminateException


			if event.eventType != MeshDrawFaceManipulator.EVENT_LEAVE:
				pick, position, existingVertexIndex, target = self._p_getPoint( event, existingVertexIndex, target )
			else:
				self._pointHighlightPosition = None
				self._view.postRedraw()

			event = self._coEventHandler.waitForNextEvent()


		drawPoint = MeshDrawFacePoint( deepcopy( pick ), deepcopy( position ), target )
		self._pointListCell.literalValue = self._pointListCell.value + [ drawPoint ]
		self._p_commandHistoryBreak()




	def _p_drawLoop(self):
		try:
			# The context button can be pressed by the user to finish the first row, or finish drawing
			self._view.addContextButtonListener( self, Viewport3d )

			while True:
				try:
					self._p_getPointFromUser()
				except MeshDrawFaceManipulator.ResetException:
					self._view.postRedraw()
		except MeshDrawFaceManipulator.TerminateException:
			self._bComplete = True


	def _p_manipulatorLoop(self):
		while True:
			self._p_drawLoop()

			event = self._coEventHandler.waitForNextEvent()
			while event.eventType != MeshDrawFaceManipulator.EVENT_BUTTONDOWN:
				event = self._coEventHandler.waitForNextEvent()
			self._bComplete = False





	def evPaint3d(self, viewport, paintLayer):
		super( MeshDrawFaceManipulator, self ).evPaint3d( viewport, paintLayer )

		if not self._bComplete:
			if self._vertexHighlightPosition is not None:
				if self._bTargetValid:
					ModelDraw.drawVertexPickHighlight( self._vertexHighlightPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
				else:
					ModelDraw.drawVertexPickHighlight( self._vertexHighlightPosition, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )

			if self._pointHighlightPosition is not None   and   self._vertexHighlightPosition is None:
				if paintLayer == PaintLayer.PAINTLAYER_WIREFRAME  or  paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
					ModelDraw.drawVertexReticle( self._pointHighlightPosition, paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY )


			self._p_updateDisplay()

			for point in self._displayPoints:
				if point is not None:
					ModelDraw.drawCircleHighlight( point, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

			ModelDraw.drawSegments( self._displaySegs, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

			if self._pointHighlightPosition is not None:
				prevInRow = None
				nextInRow = None

				if len( self._displayPoints ) >= 2:
					ModelDraw.drawSegment( Segment3( self._pointHighlightPosition, self._displayPoints[-1] ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )
					if self._bClosingEdgeValid:
						ModelDraw.drawSegment( Segment3( self._pointHighlightPosition, self._displayPoints[0] ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )
					else:
						ModelDraw.drawEdgePickHighlight( Segment3( self._pointHighlightPosition, self._displayPoints[0] ), ModelDraw.WARNING_TARGET_HIGHLIGHT_COLOUR, paintLayer )
			else:
				if self._displayClosingSeg is not None:
					ModelDraw.drawSegment( self._displayClosingSeg, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )



	def _p_faceSegments(self, pointList):
		if len( pointList ) < 2:
			return []
		else:
			return [ Segment3( a, b )    for a, b in zip( pointList[:-1], pointList[1:] )   if a is not None and b is not None ]

	def _p_faceClosingSegment(self, pointList):
		if len( pointList ) >= 3:
			a, b = pointList[-1], pointList[0]
			if a is not None  and  b is not None:
				return Segment3( a, b )
		return None



	def _p_updateDisplay(self):
		if self._bDisplayRequiresRefresh:
			state = self._p_getState()
			if state is not None:
				numVertices = state.getNumVertices()

				self._displayPoints = [ state.getFaceVertexPosition( x )   for x in xrange( 0, numVertices ) ]
				self._displaySegs = self._p_faceSegments( self._displayPoints )
				self._displayClosingSeg = self._p_faceClosingSegment( self._displayPoints )

				self._bDisplayRequiresRefresh = False




	def evPrimaryButtonDown(self, viewport):
		super( MeshDrawFaceManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_BUTTONDOWN, viewport=viewport ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshDrawFaceManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_BUTTONUP, viewport=viewport ) )


	def evMotion(self, viewport, point):
		super( MeshDrawFaceManipulator, self ).evMotion( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_MOTION, viewport=viewport, point=point ) )

	def evEnter(self, viewport, point):
		super( MeshDrawFaceManipulator, self ).evEnter( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_MOTION, viewport=viewport, point=point ) )

	def evLeave(self, viewport, point):
		super( MeshDrawFaceManipulator, self ).evLeave( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_LEAVE, viewport=viewport, point=point ) )

	def evContextButton(self, viewport):
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_CONTEXT, viewport=viewport ) )


	def _p_onPointListChanged(self):
		self._o_reset()

	def _o_reset(self):
		self._bDisplayRequiresRefresh = True
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( MeshDrawFaceManipulator.EVENT_RESET ) )


	def _p_getState(self):
		if self._stateCell is not None:
			return self._stateCell.getValue()
		return None





class MeshDrawFaceFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, pointListField, stateField):
		super( MeshDrawFaceFieldManipulator, self ).__init__()
		self._pointListCellField = pointListField
		self._stateField = stateField

	def createElement(self, sheetEditContext):
		return MeshDrawFaceManipulator()

	def initialiseElement(self, element, sheetEditContext):
		pointList = self._p_getCell( self._pointListCellField, sheetEditContext )
		state = self._p_getCell( self._stateField, sheetEditContext )
		element.attachCells( pointList, state )
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





class DrawFaceAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshDrawFace

	def __init__(self, *args, **kwargs):
		super( DrawFaceAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.state = MeshDrawFaceState()


	def main(self):
		if isinstance( self._inputObject, GSProductMesh ):
			self.mesh = deepcopy( self._inputObject )
		else:
			self.mesh = GSProductMesh()
		cppPoints = CppMeshDrawFacePointList()
		pyMeshDrawFacePointList_to_MeshDrawFacePointList( [ point.toCppMeshDrawFacePoint()  for  point in self._proc.pointList ], cppPoints )
		self.mesh.drawFaceWithState( cppPoints, self._proc.bFlip, self.state  )

	def getResult(self):
		return self.mesh




class ToolMeshDrawFace (GSProcedureTool):
	sheet = SheetRefField( ProcMeshDrawFace )

	functionClass = ProcMeshDrawFace
	adjustableInvoker = DrawFaceAdjustableInvoker
	pageTitle = _( 'DrwFc' )
	titleText = _( 'Draw Face' )


	def _p_state(self):
		try:
			adjustableInvoker = self._adjustableInvoker
		except AttributeError:
			return None

		if adjustableInvoker is not None:
			return adjustableInvoker.state

	_state = FunctionRefField( _p_state )


	targetListLabel = FieldLabelWithFn( sheet.pointList, _( 'Face points:' ) )
	targetListEditor = TargetListFieldEdit( sheet.pointList )
	bFlipCheck = BoolFieldEditCheckWithFn( sheet.bFlip, _( 'Flip' ) )

	pointListManip = MeshDrawFaceFieldManipulator( sheet.pointList, _state )
	pointListManip.enable()



	def _p_onApplyDrawingTemplates(self):
		viewport = self.view.currentViewport
		if viewport is not None:
			if isinstance( viewport, Viewport3d ):
				self._commandHistory.finishCommand()
				points = deepcopy( self.procedure.pointList )
				for point in points:
					ray = point.pick.computeRay()
					pos = viewport.raytraceDrawingTemplates( ray )
					if pos is not None:
						point.position = pos
				self.procedure.pointList = points
				self._commandHistory.finishCommand()



	applyDrawingTemplatesButton = SheetEditorButton( _( 'Apply drawing templates' ), _p_onApplyDrawingTemplates )






	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click to place vertices.\n'  '<shift>-click to weld to an existing vertex in the mesh.\n'  'Right-click to finish.\n' 'After this, you can continue by clicking once.' ) )
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.row()  <<  bFlipCheck.check
	layout.row()
	layout.row()  <<  applyDrawingTemplatesButton.button
	layout.textBlock( _( '(Use if you forgot to enable drawing templates before drawing.)' ) )
	layout.row()  <<  ''
	layout.finalise()




