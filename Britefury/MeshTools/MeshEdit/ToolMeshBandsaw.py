##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *

from copy import deepcopy

from Britefury.Math.Math import Colour3f, Point3, Segment3

from Britefury.FileIO.IOXml import ioObjectFactoryRegister, ioXmlReadBoolNode, ioXmlWriteBoolNode

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

from Britefury.Mesh.Mesh import GSProductMesh, pyMeshBandsawTargetList_to_CppMeshBandsawTargetList, CppMeshBandsawTarget, CppMeshBandsawTargetList
from Britefury.Mesh.MeshPickHelper import *

from Britefury.Model import ModelDraw
from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit





class MeshBandsawTarget (object):
	def __init__(self, pick = None, bStopAtMarkedEdges = False):
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.bStopAtMarkedEdges = bStopAtMarkedEdges


	def toCppMeshBandsawTarget(self):
		return CppMeshBandsawTarget( self.pick.toMPick(), self.bStopAtMarkedEdges )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.bStopAtMarkedEdges = ioXmlReadBoolNode( xmlNode.getChild( 'b_stop_at_marked_edges' ), self.bStopAtMarkedEdges )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_stop_at_marked_edges' ), self.bStopAtMarkedEdges )


ioObjectFactoryRegister( 'MeshBandsawTarget', MeshBandsawTarget )





class ProcMeshBandsaw (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'Bandsaw' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		cppTargets = CppMeshBandsawTargetList()
		pyMeshBandsawTargetList_to_CppMeshBandsawTargetList( [ target.toCppMeshBandsawTarget()  for  target in self.targetList ], cppTargets )
		inputObject.bandsaw( cppTargets )
		return inputObject




class MeshBandsawManipulator (Manipulator, MeshPickHelper):
	def __init__(self):
		super( MeshBandsawManipulator, self ).__init__()
		self._targetsCell = None
		self._bStopAtMarkedEdgesCell = None

		self._seedEdgeIndex = -1
		self._seedFaceIndex = -1
		self._seedEdgeSegment = None
		self._seedPickPoint = None

		self._bandsawSegments = []
		self._bandsawPoints = []
		self._bClosed = False

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( MeshBandsawManipulator, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshBandsawManipulator, self ).manipDetachFromView()


	def attachCells(self, targets, bStopAtMarkedEdges):
		assert self._targetsCell is None, 'already attached to a cell'
		self._targetsCell = targets
		self._bStopAtMarkedEdgesCell = bStopAtMarkedEdges
		self._bStopAtMarkedEdgesCell.changedSignal.connect( self._p_onParamsChanged )
		self._o_watchCellValidity( self._targetsCell )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._targetsCell is not None, 'not attached to a cell'
		self._o_stopWatchingCellValidity( self._targetsCell )
		self._bStopAtMarkedEdgesCell.changedSignal.disconnect( self._p_onParamsChanged )
		self._targetsCell = None
		self._bStopAtMarkedEdgesCell = None
		self._o_refreshSensitivity()



	def _o_setUISensitivity(self, bSensitive):
		self._o_setMeshPickHelperSensitivity( bSensitive )

	def _o_isValid(self):
		if self._targetsCell is not None:
			return self._targetsCell.isValid()
		else:
			return False



	def _p_refreshHit(self, viewport, mesh, pick):
		pickedEdgeIndex, self._seedPickPoint, pickedFaceIndex = mesh.pickEdge( pick.toMPick() )

		if pickedEdgeIndex != self._seedEdgeIndex  or  pickedFaceIndex != self._seedFaceIndex:
			# The seed has changed
			self._seedEdgeIndex, self._seedFaceIndex = pickedEdgeIndex, pickedFaceIndex

			if pickedEdgeIndex != -1  and  pickedFaceIndex != -1:
				self._seedEdgeSegment = mesh.getEdgeSegment( pickedEdgeIndex )

				# An edge was chosed; discover the bandsaw edge ring
				self._bandsawSegments, self._bClosed = mesh.bandsawGetRingSegments( pickedEdgeIndex, pickedFaceIndex, self._bStopAtMarkedEdgesCell.value )

		if pickedEdgeIndex != -1  and  pickedFaceIndex != -1:
			point, t = self._seedEdgeSegment.closestPointTo( self._seedPickPoint )
			self._bandsawPoints = [ segment.getPoint( t )  for segment in self._bandsawSegments ]
			return True
		else:
			self._bandsawSegments = []
			self._bandsawPoints = []
			return False



	def _p_paintHighlight(self, viewport, paintLayer):
		# Highlight the seed edge
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

			for point in self._bandsawPoints:
				glVertex3Point3( point )

			glEnd()


	def _p_pickConfirmed(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._targetsCell is not None, 'no cell attached'
		target = MeshBandsawTarget( deepcopy( pick ), self._bStopAtMarkedEdgesCell.value )
		self._targetsCell.literalValue = self._targetsCell.value + [ target ]
		self._p_commandHistoryBreak()


	def _o_resetHighlight(self):
		self._seedEdgeIndex = -1
		self._seedFaceIndex = -1
		self._seedEdgeSegment = None
		self._seedPickPoint = None

		self._bandsawSegments = []
		self._bandsawPoints = []
		self._bClosed = False


	def _p_onParamsChanged(self):
		self._o_resetHighlight()
		self._o_requestHighlightRefresh()







class MeshBandsawTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, targetsField, bStopAtMarkedEdgesField):
		super( MeshBandsawTargetListFieldManipulator, self ).__init__()
		self._targetsField = targetsField
		self._bStopAtMarkedEdgesField = bStopAtMarkedEdgesField


	def createElement(self, sheetEditContext):
		return MeshBandsawManipulator()

	def initialiseElement(self, element, sheetEditContext):
		targets = self._p_getCell( self._targetsField, sheetEditContext )
		bStopAtMarkedEdges = self._p_getCell( self._bStopAtMarkedEdgesField, sheetEditContext )
		element.attachCells( targets, bStopAtMarkedEdges )
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




class MeshBandsawAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshBandsaw

	def __init__(self, *args, **kwargs):
		super( MeshBandsawAdjustableInvoker, self ).__init__( *args, **kwargs )


	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTargets = CppMeshBandsawTargetList()
		pyMeshBandsawTargetList_to_CppMeshBandsawTargetList( [ target.toCppMeshBandsawTarget()  for  target in self._proc.targetList ], cppTargets )
		self.mesh.bandsaw( cppTargets )

	def getResult(self):
		return self.mesh




class ToolMeshBandsaw (GSProcedureTool):
	sheet = SheetRefField( ProcMeshBandsaw )

	functionClass = ProcMeshBandsaw
	adjustableInvoker = MeshBandsawAdjustableInvoker
	pageTitle = _( 'Bandsaw' )
	titleText = _( 'Bandsaw' )


	bStopAtMarkedEdges = Field( bool, False )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'Bandsaw targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshBandsawTargetListFieldManipulator( sheet.targetList, bStopAtMarkedEdges )
	targetListManip.enable()

	bStopAtMarkedEdgesCheck = BoolFieldEditCheck( bStopAtMarkedEdges, _( 'Stop at marked edges' ) )


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click to place bandsaw cuts' ) )
	layout.row()
	layout.row()  <<  bStopAtMarkedEdgesCheck.check
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



