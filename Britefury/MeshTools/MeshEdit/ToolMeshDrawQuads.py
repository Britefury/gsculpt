##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

import pygtk
pygtk.require( '2.0' )
import gtk

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
from Britefury.Mesh.Mesh import GSProductMesh, pyMeshDrawQuadsPointList_to_MeshDrawQuadsPointList, CppMeshDrawQuadsPoint, CppMeshDrawQuadsPointList, MeshDrawQuadsPointTarget, MeshDrawQuadsDirection, MeshDrawQuadsState
from Britefury.Mesh.MeshManipulatorHelper import MeshManipulatorHelper

from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit
from Britefury.Model.Model import GSProductModelBlank






drawQuadsTargetIOXmlEnum = IOXmlEnum( MeshDrawQuadsPointTarget, 'MeshDrawQuadsPointTarget', MeshDrawQuadsPointTarget.NEWPOINT )
drawQuadsTargetIOXmlEnum.value( MeshDrawQuadsPointTarget.NEWPOINT, 'new_point' )
drawQuadsTargetIOXmlEnum.value( MeshDrawQuadsPointTarget.EXISTINGVERTEX, 'existing_vertex' )

ioXmlReadMeshDrawQuadsPointTargetProp = drawQuadsTargetIOXmlEnum.propReader
ioXmlReadMeshDrawQuadsPointTargetNode = drawQuadsTargetIOXmlEnum.nodeReader
ioXmlWriteMeshDrawQuadsPointTargetProp = drawQuadsTargetIOXmlEnum.propWriter
ioXmlWriteMeshDrawQuadsPointTargetNode = drawQuadsTargetIOXmlEnum.nodeWriter





drawQuadsDirectionIOXmlEnum = IOXmlEnum( MeshDrawQuadsDirection, 'MeshDrawQuadsDirection', MeshDrawQuadsDirection.FORWARD )
drawQuadsDirectionIOXmlEnum.value( MeshDrawQuadsDirection.FORWARD, 'forward' )
drawQuadsDirectionIOXmlEnum.value( MeshDrawQuadsDirection.PINGPONG, 'pingpong' )

ioXmlReadMeshDrawQuadsPointDirectionProp = drawQuadsDirectionIOXmlEnum.propReader
ioXmlReadMeshDrawQuadsPointDirectionNode = drawQuadsDirectionIOXmlEnum.nodeReader
ioXmlWriteMeshDrawQuadsPointDirectionProp = drawQuadsDirectionIOXmlEnum.propWriter
ioXmlWriteMeshDrawQuadsPointDirectionNode = drawQuadsDirectionIOXmlEnum.nodeWriter




class MeshDrawQuadsPoint (object):
	def __init__(self, pick=None, position=None, target=MeshDrawQuadsPointTarget.NEWPOINT, direction=MeshDrawQuadsDirection.FORWARD):
		if pick is None:
			pick = Pick3d()
		if position is None:
			position = Point3()
		self.pick = pick
		self.position = position
		self.target = target
		self.direction = direction


	def toCppMeshDrawQuadsPoint(self):
		return CppMeshDrawQuadsPoint( self.pick.toMPick(), self.position, self.target, self.direction )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		xmlNode.getChild( 'position' )  >>  self.position
		self.target = ioXmlReadMeshDrawQuadsPointTargetNode( xmlNode.getChild( 'target' ), self.target )
		self.direction = ioXmlReadMeshDrawQuadsPointDirectionNode( xmlNode.getChild( 'direction' ), self.direction )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		xmlNode.addChild( 'position' )  <<  self.position
		ioXmlWriteMeshDrawQuadsPointTargetNode( xmlNode.addChild( 'target' ), self.target )
		ioXmlWriteMeshDrawQuadsPointDirectionNode( xmlNode.addChild( 'direction' ), self.direction )


ioObjectFactoryRegister( 'MeshDrawQuadsPoint', MeshDrawQuadsPoint )







class ProcMeshDrawQuads (GSProcedure):
	pointList = Field( TargetList, TargetList() )
	bCloseRows = Field( bool, False )
	bCloseColumns = Field( bool, False )
	numColumns = Field( int, 1 )
	bFlip = Field( bool, False )
	bNumColumnsSet = Field( bool, False )

	description = _( 'Draw quad mesh' )
	inputClass = [ GSProductModelBlank, GSProductMesh ]
	outputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		if isinstance( inputObject, GSProductMesh ):
			mesh = inputObject
		else:
			mesh = GSProductMesh()

		cppPoints = CppMeshDrawQuadsPointList()
		pyMeshDrawQuadsPointList_to_MeshDrawQuadsPointList( [ point.toCppMeshDrawQuadsPoint()  for  point in self.pointList ], cppPoints )
		mesh.drawQuads( cppPoints, self.bCloseRows, self.bCloseColumns, self.numColumns, self.bFlip  )
		return mesh






class MeshDrawQuadsManipulator (Manipulator, MeshManipulatorHelper, ViewportContextButtonListener, Viewport3dPainter):
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
		super( MeshDrawQuadsManipulator, self ).__init__()
		self._pointList = None
		self._bCloseRows = None
		self._bCloseColumns = None
		self._numColumns = None
		self._bNumColumnsSet = None
		self._stateCell = None

		self._coEventHandler = None
		self._storedEvents = []

		self._vertexHighlightPosition = None
		self._bTargetValid = True
		self._pointHighlightPosition = None

		self._bDisplayRequiresRefresh = True
		self._displayRow0Points = []
		self._displayRow1Points = []
		self._displayRowSegs = []
		self._displayRow0ClosingSeg = None
		self._displayRow1ClosingSeg = None
		self._displayColumnSegs = []

		self._bComplete = False

		self._o_setUISensitivity( False )







	def manipAttachToView(self, view):
		super( MeshDrawQuadsManipulator, self ).manipAttachToView( view )
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
		super( MeshDrawQuadsManipulator, self ).manipDetachFromView()


	def attachCells(self, pointList, bCloseRows, bCloseColumns, numColumns, bNumColumnsSet, state):
		assert self._pointList is None, 'already attached to a cell'
		self._pointList = pointList
		self._bCloseRows = bCloseRows
		self._bCloseColumns = bCloseColumns
		self._numColumns = numColumns
		self._bNumColumnsSet = bNumColumnsSet
		self._stateCell = state
		self._pointList.changedSignal.connect( self._p_onPointListChanged )
		self._bCloseRows.changedSignal.connect( self._p_onCellChanged )
		self._bCloseColumns.changedSignal.connect( self._p_onCellChanged )
		self._numColumns.changedSignal.connect( self._p_onCellChanged )
		self._bNumColumnsSet.changedSignal.connect( self._p_onCellChanged )
		self._o_watchCellValidity( self._pointList )
		self._o_watchCellValidity( self._bCloseRows )
		self._o_watchCellValidity( self._bCloseColumns )
		self._o_watchCellValidity( self._numColumns )
		self._o_watchCellValidity( self._bNumColumnsSet )
		self._o_watchCellValidity( self._stateCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._pointList is not None, 'not attached to a cell'
		self._pointList.changedSignal.disconnect( self._p_onPointListChanged )
		self._bCloseRows.changedSignal.disconnect( self._p_onCellChanged )
		self._bCloseColumns.changedSignal.disconnect( self._p_onCellChanged )
		self._numColumns.changedSignal.disconnect( self._p_onCellChanged )
		self._bNumColumnsSet.changedSignal.disconnect( self._p_onCellChanged )
		self._o_stopWatchingCellValidity( self._pointList )
		self._o_stopWatchingCellValidity( self._bCloseRows )
		self._o_stopWatchingCellValidity( self._bCloseColumns )
		self._o_stopWatchingCellValidity( self._numColumns )
		self._o_stopWatchingCellValidity( self._bNumColumnsSet )
		self._o_stopWatchingCellValidity( self._stateCell )
		self._pointList = None
		self._bCloseRows = None
		self._bCloseColumns = None
		self._numColumns = None
		self._bNumColumnsSet = None
		self._stateCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshManipulatorHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._pointList is not None  and  self._bCloseRows is not None  and  self._bCloseColumns is not None  and  self._numColumns is not None  and  self._bNumColumnsSet is not None  and self._stateCell is not None:
			return self._pointList.isValid()  and  self._bCloseRows.isValid()  and  self._bCloseColumns.isValid()  and  self._numColumns.isValid()  and  self._bNumColumnsSet.isValid()  and  self._stateCell.isValid()
		else:
			return False



	def _p_getPoint(self, event, existingVertexIndex, target):
		mesh = self._o_getMesh()
		pick = event.viewport.createPick()
		position = event.viewport.computeDrawPoint()
		self._pointHighlightPosition = position

		self._bTargetValid = True

		state = self._p_getState()
		if state is not None:
			if event.viewport.pointerStatus.bShiftKey:
				if mesh is not None:
					vertexIndex, thruFaceIndex = mesh.pickVertex( pick.toMPick() )

					if vertexIndex != existingVertexIndex  or  target != MeshDrawQuadsPointTarget.EXISTINGVERTEX:
						target = MeshDrawQuadsPointTarget.EXISTINGVERTEX
						if vertexIndex != -1:
							self._vertexHighlightPosition = mesh.getVertexPosition( vertexIndex )
							self._pointHighlightPosition = self._vertexHighlightPosition
							self._bTargetValid = state.checkVertex( vertexIndex )
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
				target = MeshDrawQuadsPointTarget.NEWPOINT
				existingVertexIndex = None
				self._vertexHighlightPosition = None
				self._view.postRedraw()

			return pick, position, existingVertexIndex, target
		else:
			return None, None, None, None




	def _p_getPointFromUser(self):
		self._vertexHighlightPosition = None
		self._bTargetValid = True
		self._pointHighlightPosition = None

		pick = None
		position = None
		existingVertexIndex = None
		target = None



		event = self._coEventHandler.waitForNextEvent()

		bNumColumnsSet = self._bNumColumnsSet.value




		# Get the point
		while event.eventType != MeshDrawQuadsManipulator.EVENT_BUTTONDOWN  or  not self._bTargetValid:
			if event.eventType == MeshDrawQuadsManipulator.EVENT_RESET:
				self._view.postRedraw()
				raise MeshDrawQuadsManipulator.ResetException

			if event.eventType == MeshDrawQuadsManipulator.EVENT_CONTEXT:
				if not bNumColumnsSet:
					self._numColumns.literalValue = len( self._pointList.value )
					self._bNumColumnsSet.literalValue = True
					bNumColumnsSet = True
					self._p_commandHistoryBreak()
					self._view.postRedraw()
				else:
					self._view.postRedraw()
					self._view.removeContextButtonListener( self, Viewport3d )
					raise MeshDrawQuadsManipulator.TerminateException


			if event.eventType != MeshDrawQuadsManipulator.EVENT_LEAVE:
				pick, position, existingVertexIndex, target = self._p_getPoint( event, existingVertexIndex, target )
			else:
				self._pointHighlightPosition = None
				self._view.postRedraw()

			event = self._coEventHandler.waitForNextEvent()


		if self._bCloseRows.value:
			direction = MeshDrawQuadsDirection.FORWARD
		else:
			direction = MeshDrawQuadsDirection.PINGPONG



		drawPoint = MeshDrawQuadsPoint( deepcopy( pick ), deepcopy( position ), target, direction )
		self._pointList.literalValue = self._pointList.value + [ drawPoint ]
		if not bNumColumnsSet:
			self._numColumns.literalValue = len( self._pointList.value )  +  1
		self._p_commandHistoryBreak()




	def _p_drawLoop(self):
		try:
			# The context button can be pressed by the user to finish the first row, or finish drawing
			self._view.addContextButtonListener( self, Viewport3d )

			while True:
				try:
					self._p_getPointFromUser()
				except MeshDrawQuadsManipulator.ResetException:
					self._view.postRedraw()
		except MeshDrawQuadsManipulator.TerminateException:
			self._bComplete = True



	def _p_manipulatorLoop(self):
		while True:
			self._p_drawLoop()

			event = self._coEventHandler.waitForNextEvent()
			while event.eventType != MeshDrawQuadsManipulator.EVENT_BUTTONDOWN:
				event = self._coEventHandler.waitForNextEvent()
			self._bComplete = False




	def evPaint3d(self, viewport, paintLayer):
		super( MeshDrawQuadsManipulator, self ).evPaint3d( viewport, paintLayer )

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

			for point in self._displayRow0Points:
				if point is not None:
					ModelDraw.drawCircleHighlight( point, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

			for point in self._displayRow1Points:
				if point is not None:
					ModelDraw.drawCircleHighlight( point, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

			ModelDraw.drawSegments( self._displayRowSegs, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )
			ModelDraw.drawSegments( self._displayColumnSegs, ModelDraw.MODIFICATION2_HIGHLIGHT_COLOUR, paintLayer )

			if self._displayRow0ClosingSeg is not None:
				ModelDraw.drawSegment( self._displayRow0ClosingSeg, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

			if self._pointHighlightPosition is not None:
				state = self._p_getState()
				if state is not None:
					column = state.getColumn()
					bDirectionForward = state.isRowDirectionForward()


					prevInRow = None
					nextInRow = None

					if len( self._displayRow1Points ) > 0:
						if bDirectionForward:
							if column > 0:
								prevInRow = self._displayRow1Points[column-1]
								if state.areRowsClosed()   and   column  ==  state.getNumColumns() - 1:
									nextInRow = self._displayRow1Points[0]
						else:
							if column < ( state.getNumColumns() - 1 ):
								prevInRow = self._displayRow1Points[column+1]
								if state.areRowsClosed():
									nextInRow = self._displayRow1Points[-1]
					if prevInRow is not None:
						ModelDraw.drawSegment( Segment3( self._pointHighlightPosition, prevInRow ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )
					if nextInRow is not None:
						ModelDraw.drawSegment( Segment3( self._pointHighlightPosition, nextInRow ), ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )

					try:
						pointInPrevRow = self._displayRow0Points[column]
					except IndexError:
						pass
					else:
						ModelDraw.drawSegment( Segment3( self._pointHighlightPosition, pointInPrevRow ), ModelDraw.MODIFICATION2_HIGHLIGHT_COLOUR, paintLayer )

					if state.areColumnsClosed()  and  state.getNumColumns() > 0  and  state.getNumRows() > 1:
						pointInFirstRow = state.getQuadMeshVertexPosition( column, 0 )
						if pointInFirstRow is not None:
							ModelDraw.drawSegment( Segment3( self._pointHighlightPosition, pointInFirstRow ), ModelDraw.MODIFICATION2_HIGHLIGHT_COLOUR, paintLayer )
				else:
					if self._displayRow1ClosingSeg is not None:
						ModelDraw.drawSegment( self._displayRow1ClosingSeg, ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR, paintLayer )



	def _p_rowSegments(self, pointList):
		if len( pointList ) < 2:
			return []
		else:
			return [ Segment3( a, b )    for a, b in zip( pointList[:-1], pointList[1:] )   if a is not None and b is not None ]

	def _p_rowClosingSegment(self, pointList):
		if len( pointList ) >= 3:
			a, b = pointList[-1], pointList[0]
			if a is not None  and  b is not None:
				return Segment3( a, b )
		return None

	def _p_columnSegments(self, row0, row1):
		return [ Segment3( a, b )    for a, b in zip( row0, row1 )   if a is not None and b is not None ]


	def _p_updateDisplay(self):
		if self._bDisplayRequiresRefresh:
			state = self._p_getState()

			if state is not None:
				numColumns = state.getNumColumns()
				if not self._bNumColumnsSet.value:
					numColumns -= 1

				numRows = state.getNumRows()
				bCloseRows = state.areRowsClosed()
				bCloseColumns = state.areColumnsClosed()
				row = state.getRow()
				if not self._bNumColumnsSet.value:
					row = 0

				rowPrev = row - 1
				if rowPrev >= 0  and  rowPrev < numRows:
					self._displayRow0Points = [ state.getQuadMeshVertexPosition( x, rowPrev )   for x in xrange( 0, numColumns ) ]
				else:
					self._displayRow0Points = []

				if row >= 0  and  row < numRows:
					self._displayRow1Points = [ state.getQuadMeshVertexPosition( x, row )   for x in xrange( 0, numColumns ) ]
				else:
					self._displayRow1Points = []

				self._displayRowSegs = self._p_rowSegments( self._displayRow0Points )
				self._displayRowSegs += self._p_rowSegments( self._displayRow1Points )
				self._displayColumnSegs = self._p_columnSegments( self._displayRow0Points, self._displayRow1Points )

				if bCloseRows:
					self._displayRow0ClosingSeg = self._p_rowClosingSegment( self._displayRow0Points )
					self._displayRow1ClosingSeg = self._p_rowClosingSegment( self._displayRow1Points )
				else:
					self._displayRow0ClosingSeg = None
					self._displayRow1ClosingSeg = None

				if bCloseColumns  and  numRows > 2:
					firstRow = [ state.getQuadMeshVertexPosition( x, 0 )   for x in xrange( 0, numColumns ) ]
					self._displayColumnSegs += self._p_columnSegments( self._displayRow1Points, firstRow )

				self._bDisplayRequiresRefresh = False




	def evPrimaryButtonDown(self, viewport):
		super( MeshDrawQuadsManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_BUTTONDOWN, viewport=viewport ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshDrawQuadsManipulator, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_BUTTONUP, viewport=viewport ) )


	def evMotion(self, viewport, point):
		super( MeshDrawQuadsManipulator, self ).evMotion( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_MOTION, viewport=viewport, point=point ) )

	def evEnter(self, viewport, point):
		super( MeshDrawQuadsManipulator, self ).evEnter( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_MOTION, viewport=viewport, point=point ) )

	def evLeave(self, viewport, point):
		super( MeshDrawQuadsManipulator, self ).evLeave( viewport, point )
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_LEAVE, viewport=viewport, point=point ) )

	def evContextButton(self, viewport):
		if self._bSensitive:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_CONTEXT, viewport=viewport ) )


	def _p_onCellChanged(self):
		self._bDisplayRequiresRefresh = True
		if self._view is not None:
			self._view.postRedraw()

	def _p_onPointListChanged(self):
		self._o_reset()

	def _o_reset(self):
		self._bDisplayRequiresRefresh = True
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_RESET ) )


	def _p_getState(self):
		if self._stateCell is not None:
			return self._stateCell.getValue()
		return None





class MeshDrawQuadsFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, pointListField, bCloseRowsField, bCloseColumnsField, numColumnsField, bNumColumnsSetField, stateField):
		super( MeshDrawQuadsFieldManipulator, self ).__init__()
		self._pointListField = pointListField
		self._bCloseRowsField = bCloseRowsField
		self._bCloseColumnsField = bCloseColumnsField
		self._numColumnsField = numColumnsField
		self._bNumColumnsSetField = bNumColumnsSetField
		self._stateField = stateField

	def createElement(self, sheetEditContext):
		return MeshDrawQuadsManipulator()

	def initialiseElement(self, element, sheetEditContext):
		pointList = self._p_getCell( self._pointListField, sheetEditContext )
		bCloseRows = self._p_getCell( self._bCloseRowsField, sheetEditContext )
		bCloseColumns = self._p_getCell( self._bCloseColumnsField, sheetEditContext )
		numColumns = self._p_getCell( self._numColumnsField, sheetEditContext )
		bNumColumnsSet = self._p_getCell( self._bNumColumnsSetField, sheetEditContext )
		state = self._p_getCell( self._stateField, sheetEditContext )
		element.attachCells( pointList, bCloseRows, bCloseColumns, numColumns, bNumColumnsSet, state )
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





class DrawQuadsAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshDrawQuads

	def __init__(self, *args, **kwargs):
		super( DrawQuadsAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.state = MeshDrawQuadsState()


	def main(self):
		if isinstance( self._inputObject, GSProductMesh ):
			self.mesh = deepcopy( self._inputObject )
		else:
			self.mesh = GSProductMesh()
		cppPoints = CppMeshDrawQuadsPointList()
		pyMeshDrawQuadsPointList_to_MeshDrawQuadsPointList( [ point.toCppMeshDrawQuadsPoint()  for  point in self._proc.pointList ], cppPoints )
		self.mesh.drawQuadsWithState( cppPoints, self._proc.bCloseRows, self._proc.bCloseColumns, self._proc.numColumns, self._proc.bFlip, self.state  )

	def getResult(self):
		return self.mesh




class ToolMeshDrawQuads (GSProcedureTool):
	sheet = SheetRefField( ProcMeshDrawQuads )

	functionClass = ProcMeshDrawQuads
	adjustableInvoker = DrawQuadsAdjustableInvoker
	pageTitle = _( 'DrwQMsh' )
	titleText = _( 'Draw Quad Mesh' )


	def _p_state(self):
		try:
			adjustableInvoker = self._adjustableInvoker
		except AttributeError:
			return None

		if adjustableInvoker is not None:
			return adjustableInvoker.state

	_state = FunctionRefField( _p_state )


	targetListLabel = FieldLabelWithFn( sheet.pointList, _( 'Quad mesh points:' ) )
	targetListEditor = TargetListFieldEdit( sheet.pointList )
	bCloseRowsCheck = BoolFieldEditCheckWithFn( sheet.bCloseRows, _( 'Close rows' ) )
	bCloseColumnsCheck = BoolFieldEditCheckWithFn( sheet.bCloseColumns, _( 'Close columns' ) )
	bFlipCheck = BoolFieldEditCheckWithFn( sheet.bFlip, _( 'Flip' ) )

	pointListManip = MeshDrawQuadsFieldManipulator( sheet.pointList, sheet.bCloseRows, sheet.bCloseColumns, sheet.numColumns, sheet.bNumColumnsSet, _state )
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
	layout.textBlock( _( 'Click to place vertices.\n'  'After placing the first row of vertices, right-click to mark the end.\n'  'Further clicks will place vertices in subsequent rows.\n'  'Right-click a second time to finish.\n'  'After this, you can continue by clicking once.\n'  '<shift>-click to weld to an existing vertex in the mesh. Note: this only works on boundaries (first and last rows and columns)' ) )
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.row()  <<  bCloseRowsCheck.check
	layout.row()  <<  bCloseColumnsCheck.check
	layout.row()
	layout.row()  <<  bFlipCheck.check
	layout.row()
	layout.row()  <<  applyDrawingTemplatesButton.button
	layout.textBlock( _( '(Use if you forgot to enable drawing templates before drawing.)' ) )
	layout.row()  <<  ''
	layout.finalise()


