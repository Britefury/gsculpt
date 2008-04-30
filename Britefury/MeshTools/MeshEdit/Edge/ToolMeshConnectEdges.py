##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *

from copy import deepcopy

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter
from Britefury.Graphics.Graphics import glColor3Colour3f, glVertex3Point3

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
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import *
from Britefury.Model.Model import pyMPickList_to_MPickList, MPickList






class ProcMeshConnectEdges (GSProcedure):
	targetList = Field( TargetList, TargetList() )
	terminalVertexTargetList = Field( TargetList, TargetList() )

	description = _( 'Connect edges' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		mpicks = MPickList()
		terminalMPicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.targetList ], mpicks )
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self.terminalVertexTargetList ], terminalMPicks )
		inputObject.connectMarkedEdges( mpicks, terminalMPicks, True )
		return inputObject




class MeshConnectEdgesManipulator (Manipulator, MeshPickHelper):
	TARGET_TERMINALVERTEX = 0
	TARGET_EDGE = 1

	def __init__(self):
		super( MeshConnectEdgesManipulator, self ).__init__()
		self._cell = None
		self._terminalCell = None
		self._connectStateCell = None

		self._seedEdgeIndex = -1
		self._seedEdgeSegment = None
		self._seedPickPoint = None
		self._bSeedValid = False

		self._terminalVertexPosition = None

		self._ringSegments = []
		self._cutPoints = []
		self._bClosed = False

		self._target = MeshConnectEdgesManipulator.TARGET_EDGE

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( MeshConnectEdgesManipulator, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshConnectEdgesManipulator, self ).manipDetachFromView()


	def attachCells(self, cell, terminalCell, connectStateCell):
		assert self._cell is None, 'already attached to a cell'
		self._cell = cell
		self._terminalCell = terminalCell
		self._connectStateCell = connectStateCell
		self._o_watchCellValidity( self._cell )
		self._o_watchCellValidity( self._terminalCell )
		self._o_watchCellValidity( self._connectStateCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._cell is not None, 'not attached to a cell'
		self._o_stopWatchingCellValidity( self._cell )
		self._o_stopWatchingCellValidity( self._terminalCell )
		self._o_stopWatchingCellValidity( self._connectStateCell )
		self._cell = None
		self._terminalCell = None
		self._connectStateCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshPickHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._cell is not None  and  self._terminalCell is not None:
			return self._cell.isValid()  and  self._terminalCell.isValid()
		else:
			return False



	def _p_refreshHit(self, viewport, mesh, pick):
		if viewport.pointerStatus.bShiftKey:
			self._target = MeshConnectEdgesManipulator.TARGET_TERMINALVERTEX
			pickedVertexIndex, thruFaceIndex = mesh.pickVertex( pick.toMPick() )
			if pickedVertexIndex != -1:
				self._terminalVertexPosition = mesh.getVertexPosition( pickedVertexIndex )
				return True
			else:
				self._terminalVertexPosition = None
				return False
		else:
			self._target = MeshConnectEdgesManipulator.TARGET_EDGE
			pickedEdgeIndex, self._seedPickPoint, pickedFaceIndex = mesh.pickEdge( pick.toMPick() )

			if pickedEdgeIndex != self._seedEdgeIndex:
				# The seed has changed
				self._seedEdgeIndex = pickedEdgeIndex

				if pickedEdgeIndex != -1:
					self._seedEdgeSegment = mesh.getEdgeSegment( pickedEdgeIndex )
					if mesh.isEdgeMarked( pickedEdgeIndex ):
						# An edge was chosed; discover the bandsaw edge ring
						self._ringSegments, self._bClosed = mesh.connectMarkedEdgesGetRingSegments( pickedEdgeIndex )
						self._bSeedValid = True
					else:
						self._ringSegments = []
						self._bClosed = False
						self._bSeedValid = False
						self._cutPoints = []
						return False

			if pickedEdgeIndex != -1:
				point, t = self._seedEdgeSegment.closestPointTo( self._seedPickPoint )
				self._cutPoints = [ segment.getPoint( t )  for segment in self._ringSegments ]
				return True
			else:
				self._ringSegments = []
				self._cutPoints = []
				return False



	def _p_paintHighlight(self, viewport, paintLayer):
		if self._target == MeshConnectEdgesManipulator.TARGET_TERMINALVERTEX:
			if self._terminalVertexPosition is not None:
				ModelDraw.drawVertexPickHighlight( self._terminalVertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
		elif self._target == MeshConnectEdgesManipulator.TARGET_EDGE:
			# Highlight the seed edge
			if self._bSeedValid:
				ModelDraw.drawEdgePickHighlight( self._seedEdgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )

				# Draw the split position on the seed edge
				ModelDraw.drawSplitPosition( self._seedPickPoint )

				# Draw the bandsaw line
				if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
					colour = ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR

					if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
						colour = ModelDraw.computeOverlayColour( colour )

					glColor3Colour3f( colour )

					if self._bClosed:
						glBegin( GL_LINE_LOOP )
					else:
						glBegin( GL_LINE_STRIP )

					for point in self._cutPoints:
						glVertex3Point3( point )

					glEnd()
			else:
				ModelDraw.drawEdgePickHighlight( self._seedEdgeSegment, ModelDraw.INVALID_TARGET_HIGHLIGHT_COLOUR, paintLayer )


	def evPaint3d(self, viewport, paintLayer):
		super( MeshConnectEdgesManipulator, self ).evPaint3d( viewport, paintLayer )

		if self._connectStateCell is not None:
			connectState = self._connectStateCell.getImmutableValue()
			if connectState is not None:
				for point in connectState:
					ModelDraw.drawVertexReticle( point, paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY )


	def _p_pickConfirmed(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._cell is not None  and  self._terminalCell is not None, 'no cell attached'
		if self._target == MeshConnectEdgesManipulator.TARGET_TERMINALVERTEX:
			self._terminalCell.literalValue = self._terminalCell.value + [ pick ]
			self._p_commandHistoryBreak()
		elif self._target == MeshConnectEdgesManipulator.TARGET_EDGE:
			self._cell.literalValue = self._cell.value + [ pick ]
			self._p_commandHistoryBreak()


	def _o_resetHighlight(self):
		self._seedEdgeIndex = -1
		self._seedEdgeSegment = None
		self._seedPickPoint = None

		self._ringSegments = []
		self._cutPoints = []
		self._bClosed = False


	def _p_onParamsChanged(self):
		self._o_resetHighlight()
		self._o_requestHighlightRefresh()




class MeshConnectEdgesTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, cutTargetListField, terminalVertexTargetListField, connectStateField):
		super( MeshConnectEdgesTargetListFieldManipulator, self ).__init__()
		self._cutTargetListField = cutTargetListField
		self._terminalVertexTargetListField = terminalVertexTargetListField
		self._connectStateField = connectStateField
		self._o_addDependency( cutTargetListField )
		self._o_addDependency( terminalVertexTargetListField )


	def createElement(self, sheetEditContext):
		manip = MeshConnectEdgesManipulator()
		return manip

	def initialiseElement(self, element, sheetEditContext):
		cutTargetListCell = self._p_getCell( self._cutTargetListField, sheetEditContext )
		terminalVertexTargetListCell = self._p_getCell( self._terminalVertexTargetListField, sheetEditContext )
		connectStateCell = self._p_getCell( self._connectStateField, sheetEditContext )
		element.attachCells( cutTargetListCell, terminalVertexTargetListCell, connectStateCell )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCells()
		element.commandHistory = None
		element.connectState = None


	def installElementEventHandlers(self, element, sheetEditContext):
		element.manipAttachToView( sheetEditContext.view )

	def uninstallElementEventHandlers(self, element, sheetEditContext):
		element.manipDetachFromView()


	def attachInputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachInputProductCell(self, element, cell):
		element.detachMeshCell()




class MeshConnectEdgesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshConnectEdges

	def __init__(self, *args, **kwargs):
		super( MeshConnectEdgesAdjustableInvoker, self ).__init__( *args, **kwargs )
		self.connectState = []


	def main(self):
		mpicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.targetList ], mpicks )

		terminalVertexMPicks = MPickList()
		pyMPickList_to_MPickList( [ pick.toMPick()  for  pick in self._proc.terminalVertexTargetList ], terminalVertexMPicks )

		terminalVertexIndices = self._inputObject.connectMarkedEdgesGetTerminalVertices( terminalVertexMPicks )
		self.connectState[:] = [ self._inputObject.getVertexPosition( vertexIndex )  for vertexIndex in terminalVertexIndices ]
		self.mesh = deepcopy( self._inputObject )
		self.mesh.connectMarkedEdges( mpicks, terminalVertexMPicks, False )

	def getResult(self):
		return self.mesh



class ToolMeshConnectEdges (GSProcedureTool):
	sheet = SheetRefField( ProcMeshConnectEdges )

	functionClass = ProcMeshConnectEdges
	adjustableInvoker = MeshConnectEdgesAdjustableInvoker
	pageTitle = _( 'Connect' )
	titleText = _( 'Connect Edges' )


	def _p_connectState(self):
		try:
			adjustableInvoker = self._adjustableInvoker
		except AttributeError:
			return None

		if adjustableInvoker is not None:
			return adjustableInvoker.connectState

	_connectState = FunctionRefField( _p_connectState )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Connect targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )
	terminalVertexTargetListLabel = FieldLabelWithFn( sheet.terminalVertexTargetList, _( 'Terminal vertex targets:' ) )
	terminalVertexTargetListEditor = TargetListFieldEdit( sheet.terminalVertexTargetList )

	targetListManip = MeshConnectEdgesTargetListFieldManipulator( sheet.targetList, sheet.terminalVertexTargetList, _connectState )
	targetListManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Move the pointer onto the mesh and highlight marked edges to see a preview of the cut that will be made. Click to confirm the cut.\n'  'Shift-click to choose terminal vertices, which will be connected to the ends of the cuts.' ) )
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.row()  <<  terminalVertexTargetListLabel.label
	layout.row()  <<  terminalVertexTargetListEditor.numTargets
	layout.row()  <<  None  <<  ( terminalVertexTargetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



