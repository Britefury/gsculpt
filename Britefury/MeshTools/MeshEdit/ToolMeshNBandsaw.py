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

from Britefury.FileIO.IOXml import *

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

from Britefury.Mesh.Mesh import GSProductMesh, pyMeshNBandsawTargetList_to_CppMeshNBandsawTargetList, CppMeshNBandsawTarget, CppMeshNBandsawTargetList
from Britefury.Mesh.MeshPickHelper import *

from Britefury.Model import ModelDraw
from Britefury.Model.Pick3d import Pick3d
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit





class MeshNBandsawTarget (object):
	def __init__(self, pick = None, bStopAtMarkedEdges = False, numCuts = 1):
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.bStopAtMarkedEdges = bStopAtMarkedEdges
		self.numCuts = numCuts


	def toCppMeshNBandsawTarget(self):
		return CppMeshNBandsawTarget( self.pick.toMPick(), self.bStopAtMarkedEdges, self.numCuts )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.bStopAtMarkedEdges = ioXmlReadBoolNode( xmlNode.getChild( 'b_stop_at_marked_edges' ), self.bStopAtMarkedEdges )
		self.numCuts = ioXmlReadIntNode( xmlNode.getChild( 'num_cuts' ), self.numCuts )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_stop_at_marked_edges' ), self.bStopAtMarkedEdges )
		ioXmlWriteIntNode( xmlNode.addChild( 'num_cuts' ), self.numCuts )


ioObjectFactoryRegister( 'MeshNBandsawTarget', MeshNBandsawTarget )





class ProcMeshNBandsaw (GSProcedure):
	targetList = Field( TargetList, TargetList() )

	description = _( 'N-Bandsaw' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		cppTargets = CppMeshNBandsawTargetList()
		pyMeshNBandsawTargetList_to_CppMeshNBandsawTargetList( [ target.toCppMeshNBandsawTarget()  for  target in self.targetList ], cppTargets )
		inputObject.nBandsaw( cppTargets )
		return inputObject




class MeshNBandsawManipulator (Manipulator, MeshPickHelper):
	def __init__(self):
		super( MeshNBandsawManipulator, self ).__init__()
		self._targetsCell = None
		self._bStopAtMarkedEdgesCell = None
		self._numCutsCell = None

		self._seedEdgeIndex = -1
		self._seedFaceIndex = -1
		self._seedEdgeSegment = None
		self._seedPickPoint = None

		self._bandsawSegments = []
		self._bandsawPoints = []
		self._bClosed = False

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		super( MeshNBandsawManipulator, self ).manipAttachToView( view )
		self.attachView( view )
		self._view.addEventHandler( self, Viewport3d )		# HACK: need to handle other viewport types
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self.detachView()
		super( MeshNBandsawManipulator, self ).manipDetachFromView()


	def attachCells(self, targets, bStopAtMarkedEdges, numCuts):
		assert self._targetsCell is None, 'already attached to a cell'
		self._targetsCell = targets
		self._bStopAtMarkedEdgesCell = bStopAtMarkedEdges
		self._numCutsCell = numCuts
		self._o_watchCellValidity( self._targetsCell )
		self._bStopAtMarkedEdgesCell.changedSignal.connect( self._p_onParamsChanged )
		self._numCutsCell.changedSignal.connect( self._p_onParamsChanged )
		self._o_refreshSensitivity()

	def detachCells(self):
		assert self._targetsCell is not None, 'not attached to a cell'
		self._o_stopWatchingCellValidity( self._targetsCell )
		self._bStopAtMarkedEdgesCell.changedSignal.disconnect( self._p_onParamsChanged )
		self._numCutsCell.changedSignal.disconnect( self._p_onParamsChanged )
		self._targetsCell = None
		self._bStopAtMarkedEdgesCell = None
		self._numCutsCell = None
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
			numCuts = self._numCutsCell.value
			numSegments = numCuts + 1
			deltaT = 1.0  /  float( numSegments )
			splitFractions = [ deltaT * i  for i in xrange( 1, numSegments ) ]
			self._bandsawPoints = [ [ segment.getPoint( t )  for segment in self._bandsawSegments ]    for t in splitFractions ]
			return True
		else:
			self._bandsawSegments = []
			self._bandsawPoints = []
			return False



	def _p_paintHighlight(self, viewport, paintLayer):
		# Highlight the seed edge
		ModelDraw.drawEdgePickHighlight( self._seedEdgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )

		# Draw the bandsaw lines
		if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY  or  paintLayer == PaintLayer.PAINTLAYER_WIREFRAME:
			colour = ModelDraw.MODIFICATION1_HIGHLIGHT_COLOUR

			if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
				colour = ModelDraw.computeOverlayColour( colour )

			glColor3Colour3f( colour )

			for pointList in self._bandsawPoints:
				if self._bClosed:
					glBegin( GL_LINE_LOOP )
				else:
					glBegin( GL_LINE_STRIP )

				for point in pointList:
					glVertex3Point3( point )

				glEnd()


	def _p_pickConfirmed(self, viewport, pick):
		"Protected - call to commit a pick"
		assert self._targetsCell is not None, 'no cell attached'
		target = MeshNBandsawTarget( deepcopy( pick ), self._bStopAtMarkedEdgesCell.value, self._numCutsCell.value + 1 )
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





class MeshNBandsawTargetListFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, targetsField, bStopAtMarkedEdgesField, numCutsField):
		super( MeshNBandsawTargetListFieldManipulator, self ).__init__()
		self._targetsField = targetsField
		self._bStopAtMarkedEdgesField = bStopAtMarkedEdgesField
		self._numCutsField = numCutsField


	def createElement(self, sheetEditContext):
		return MeshNBandsawManipulator()

	def initialiseElement(self, element, sheetEditContext):
		targets = self._p_getCell( self._targetsField, sheetEditContext )
		bStopAtMarkedEdges = self._p_getCell( self._bStopAtMarkedEdgesField, sheetEditContext )
		numCuts = self._p_getCell( self._numCutsField, sheetEditContext )
		element.attachCells( targets, bStopAtMarkedEdges, numCuts )
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




class MeshNBandsawAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshNBandsaw

	def __init__(self, *args, **kwargs):
		super( MeshNBandsawAdjustableInvoker, self ).__init__( *args, **kwargs )


	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTargets = CppMeshNBandsawTargetList()
		pyMeshNBandsawTargetList_to_CppMeshNBandsawTargetList( [ target.toCppMeshNBandsawTarget()  for  target in self._proc.targetList ], cppTargets )
		self.mesh.nBandsaw( cppTargets )

	def getResult(self):
		return self.mesh




class ToolMeshNBandsaw (GSProcedureTool):
	sheet = SheetRefField( ProcMeshNBandsaw )

	functionClass = ProcMeshNBandsaw
	adjustableInvoker = MeshNBandsawAdjustableInvoker
	pageTitle = _( 'N-Bandsaw' )
	titleText = _( 'N-Bandsaw' )


	bStopAtMarkedEdges = Field( bool, False )
	numCuts = Field( int, 1 )


	targetListLabel = FieldLabelWithFn( sheet.targetList, _( 'N-Bandsaw targets:' ) )
	targetListEditor = TargetListFieldEdit( sheet.targetList )

	targetListManip = MeshNBandsawTargetListFieldManipulator( sheet.targetList, bStopAtMarkedEdges, numCuts )
	targetListManip.enable()

	bStopAtMarkedEdgesCheck = BoolFieldEditCheck( bStopAtMarkedEdges, _( 'Stop at marked edges' ) )
	numCutsEntry = IntFieldEditEntry( numCuts, 1, 1048576, 1, 10 )

	numCutsManip = IntFieldScrollManipulator( numCuts, 1, 1048576 )
	numCutsManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click to place bandsaw cuts\n'  'Use the scroll wheel to change the number of cuts.' ) )
	layout.row()
	layout.row()  <<  bStopAtMarkedEdgesCheck.check
	layout.row()  <<  _( 'Num. cuts:' )  <<  numCutsEntry.entry
	layout.row()
	layout.row()  <<  targetListLabel.label
	layout.row()  <<  targetListEditor.numTargets
	layout.row()  <<  None  <<  ( targetListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



