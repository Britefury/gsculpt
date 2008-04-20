##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy, deepcopy

from OpenGL.GL import *
from OpenGL.GLU import *

from Britefury.Math.Math import Colour3f, Point3, Segment3

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegister

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport import ViewportEventHandler
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Graphics.Graphics import BackfaceCull
from Britefury.Graphics.BackfaceCullCell import BackfaceCullCell

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model import ModelDraw
from Britefury.Mesh.Mesh import GSProductMesh, MeshKnifeTargetConstraint, CppMeshKnifeTarget

from Britefury.Model.Pick3d import Pick3d





constraintTypeIOXmlEnum = IOXmlEnum( MeshKnifeTargetConstraint, 'MeshKnifeTargetConstraint', MeshKnifeTargetConstraint.NOTHING )
constraintTypeIOXmlEnum.value( MeshKnifeTargetConstraint.NOTHING, 'nothing' )
constraintTypeIOXmlEnum.value( MeshKnifeTargetConstraint.VERTEX, 'vertex' )
constraintTypeIOXmlEnum.value( MeshKnifeTargetConstraint.EDGE, 'edge' )

ioXmlReadMeshKnifeTargetConstraintProp = constraintTypeIOXmlEnum.propReader
ioXmlReadMeshKnifeTargetConstraintNode = constraintTypeIOXmlEnum.nodeReader
ioXmlWriteMeshKnifeTargetConstraintProp = constraintTypeIOXmlEnum.propWriter
ioXmlWriteMeshKnifeTargetConstraintNode = constraintTypeIOXmlEnum.nodeWriter





class MeshKnifeTarget (object):
	def __init__(self, pick=None, constraint=MeshKnifeTargetConstraint.NOTHING):
		super( MeshKnifeTarget, self ).__init__()
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.constraint = constraint


	def toCppMeshKnifeTarget(self):
		return CppMeshKnifeTarget( self.pick.toMPick(), self.constraint )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.constraint = ioXmlReadMeshKnifeTargetConstraintNode( xmlNode.getChild( 'constraint' ), self.constraint )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteMeshKnifeTargetConstraintNode( xmlNode.addChild( 'constraint' ), self.constraint )


	def __copy__(self):
		return MeshKnifeTarget( self.pick, self.constraint )

ioObjectFactoryRegister( 'MeshKnifeTarget', MeshKnifeTarget )


class MeshKnifeTargetCell (Cell):
	valueClass = MeshKnifeTarget
	bAllowNone = True

registerCellType( MeshKnifeTarget, MeshKnifeTargetCell )









class ProcMeshKnife (GSProcedure):
	startTarget = Field( MeshKnifeTarget, MeshKnifeTarget() )
	endTarget = Field( MeshKnifeTarget, MeshKnifeTarget() )
	faceCull = Field( BackfaceCull, None )
	bMarkedFacesOnly = Field( bool, False )
	bMarkCutEdges = Field( bool, True )
	bCullEnabled = Field( bool, False )
	bKnifeEnabled = Field( bool, False )

	description = _( 'Knife' )
	inputClass = GSProductMesh

	def procedureInvoke(self, inputObject, errorHandler):
		if self.bKnifeEnabled:
			cull = None
			if self.bCullEnabled:
				cull = self.faceCull
			inputObject.knife( self.startTarget.toCppMeshKnifeTarget(), self.endTarget.toCppMeshKnifeTarget(), self.bMarkedFacesOnly, cull, self.bMarkCutEdges )
		return inputObject





class MeshKnifeManipulator (Manipulator, ViewportEventHandler, Viewport3dPainter):
	def __init__(self):
		super( MeshKnifeManipulator, self ).__init__()

		self._startTargetCell = None
		self._endTargetCell = None
		self._knifeEnabledCell = None
		self._backfaceCullCell = None
		self._meshCell = None
		self._knifeViewport = None

		self._p1 = None
		self._p2 = None
		self._startPoint = None
		self._endPoint = None
		self._bButtonDown = False

		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeSplitPosition = None
		self._bHighlightEnabled = False
		self._highlightConstraint = None

		self._startTarget = None
		self._endTarget = None

		self._o_setUISensitivity( False )



	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		super( MeshKnifeManipulator, self ).manipAttachToView( view )
		self._view.addEventHandler( self, Viewport3d )
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		super( MeshKnifeManipulator, self ).manipDetachFromView()



	def attachCells(self, startTarget, endTarget, knifeEnabled, backfaceCull):
		self._startTargetCell = startTarget
		self._endTargetCell = endTarget
		self._knifeEnabledCell = knifeEnabled
		self._backfaceCullCell = backfaceCull

	def detachCells(self):
		self._startTargetCell = None
		self._endTargetCell = None
		self._knifeEnabledCell = None
		self._backfaceCullCell = None



	def attachMeshCell(self, cell):
		assert self._meshCell is None, 'mesh cell already attached'
		self._meshCell = cell
		self._meshCell.changedSignal.connect( self._p_onMeshCellValueChanged )

	def detachMeshCell(self):
		assert self._meshCell is not None, 'no mesh cell attached'
		self._meshCell.changedSignal.disconnect( self._p_onMeshCellValueChanged )
		self._meshCell = None


	def _o_setUISensitivity(self, bSensitive):
		pass



	def evPrimaryButtonDown(self, viewport):
		super( MeshKnifeManipulator, self ).evPrimaryButtonDown( viewport )

		# Note the viewport used for this operation
		self._knifeViewport = viewport

		# Backface culling
		if self._backfaceCullCell is not None:
			self._backfaceCullCell.literalValue = viewport.camera.createBackfaceCull()

		# Set start target
		self._startTarget = self._p_createKnifeTarget( viewport )

		# Note the pointer position
		self._p1 = viewport.pointerStatus.positionF
		self._endPoint = copy( self._startPoint )

		# Button pressed
		self._bButtonDown = True


	def evPrimaryButtonUp(self, viewport):
		super( MeshKnifeManipulator, self ).evPrimaryButtonUp( viewport )

		# Button released
		self._bButtonDown = False
		self._bHighlightEnabled = False

		# Set end target
		self._endTarget = self._p_createKnifeTarget( viewport )

		# Refesh knife cells
		self._p_setKnife( viewport )

		# Finished
		self._p_commandHistoryBreak()
		self._endPoint = None

		# Redraw
		self._view.postRedraw()



	def evMotion(self, viewport, point):
		super( MeshKnifeManipulator, self ).evMotion( viewport, point )

		# Refresh the highlight
		if self._bButtonDown:
			self._endPoint = self._p_refreshHighlight( viewport )
		else:
			self._startPoint = self._p_refreshHighlight( viewport )

		# Need an immediate redraw
		viewport.forceRedraw()


	def evLeave(self, viewport, point):
		super( MeshKnifeManipulator, self ).evLeave( viewport, point )

		# Disable highlight
		self._bHighlightEnabled = False

		# Need a redraw
		viewport.postRedraw()



	def evPaint3d(self, viewport, paintLayer):
		# Paint the target highlight
		if self._bHighlightEnabled:
			if self._highlightVertexPosition is not None:
				ModelDraw.drawVertexPickHighlight( self._highlightVertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			if self._highlightEdgeSegment is not None:
				ModelDraw.drawEdgePickHighlight( self._highlightEdgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
			if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_NODEPTH  and  self._highlightEdgeSplitPosition is not None:
				ModelDraw.drawSplitPosition( self._highlightEdgeSplitPosition )

		if viewport is self._knifeViewport:
			if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_NODEPTH:
				if self._bButtonDown:
					glDepthMask( False )

					glMatrixMode( GL_PROJECTION )
					glPushMatrix()
					viewport.glProjectionIdentity()

					glMatrixMode( GL_MODELVIEW )
					glPushMatrix()
					viewport.glModelViewIdentity()


					glPointSize( 7.0 )
					glColor4d( 0.0, 0.6, 1.0, 0.5 )
					glBegin( GL_POINTS )
					glVertex2d( self._startPoint.x, self._startPoint.y )
					glVertex2d( self._endPoint.x, self._endPoint.y )
					glEnd()

					glBegin( GL_LINES )
					glVertex2d( self._startPoint.x, self._startPoint.y )
					glVertex2d( self._endPoint.x, self._endPoint.y )
					glEnd()

					glPointSize( 1.0 )

					glPopMatrix()
					glMatrixMode( GL_PROJECTION )
					glPopMatrix()
					glMatrixMode( GL_MODELVIEW )

					glDepthMask( True )



	def _p_setKnife(self, viewport):
		# Set the knife target cells
		if self._startTargetCell is not None  and  self._startTarget is not None:
			self._startTargetCell.literalValue = copy( self._startTarget )
		if self._endTargetCell is not None  and  self._endTarget is not None:
			self._endTargetCell.literalValue = copy( self._endTarget )
		if self._knifeEnabledCell is not None  and  self._startTarget is not None  and  self._endTarget is not None:
			self._knifeEnabledCell.literalValue = True



	def _p_refreshHighlight(self, viewport):
		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeSplitPosition = None
		self._bHighlightEnabled = False


		self._highlightConstraint = self._p_getConstraint( viewport )
		pick = viewport.createPick()

		# Get the screen position
		screenPoint = pick.screenPos

		mesh = self._p_getMesh()

		if self._highlightConstraint == MeshKnifeTargetConstraint.VERTEX:
			# Constrained to vertex
			vertexIndex, thruFaceIndex = mesh.pickVertex( pick.toMPick() )

			if vertexIndex != -1:
				# Highlight the vertex
				self._highlightVertexPosition = mesh.getVertexPosition( vertexIndex )
				self._bHighlightEnabled = True
				screenPoint = pick.projection.worldToScreenMatrix.transformHomogeneous2d( self._highlightVertexPosition )
			else:
				self._highlightVertexPosition = None
				self._bHighlightEnabled = False
		elif self._highlightConstraint == MeshKnifeTargetConstraint.EDGE:
			# Constrained to edge
			edgeIndex, closestPoint, thruFaceIndex = mesh.pickEdge( pick.toMPick() )

			if edgeIndex != -1:
				# Highlight the edge
				self._highlightEdgeSegment = mesh.getEdgeSegment( edgeIndex )
				self._highlightEdgeSplitPosition = closestPoint
				self._bHighlightEnabled = True
				screenPoint = pick.projection.worldToScreenMatrix.transformHomogeneous2d( self._highlightEdgeSplitPosition )
			else:
				self._highlightEdgeSegment = None
				self._highlightEdgeSplitPosition = None
				self._bHighlightEnabled = False
		elif self._highlightConstraint == MeshKnifeTargetConstraint.NOTHING:
			# No constraint; no highlight
			self._bHighlightEnabled = False
		else:
			assert False, 'invalid constraint'

		return screenPoint



	def _p_getConstraint(self, viewport):
		pstat = viewport.pointerStatus
		if pstat.bShiftKey:
			return MeshKnifeTargetConstraint.VERTEX
		elif pstat.bControlKey:
			return MeshKnifeTargetConstraint.EDGE
		else:
			return MeshKnifeTargetConstraint.NOTHING


	def _p_createKnifeTarget(self, viewport):
		pick = viewport.createPick()
		return MeshKnifeTarget( viewport.createPick(), self._p_getConstraint( viewport ) )


	def _p_onMeshCellValueChanged(self):
		if self._bButtonDown:
			if self._knifeViewport is not None:
				self._endPoint = self._p_refreshHighlight( self._knifeViewport )
				self._knifeViewport.postRedraw()
		else:
			if self._view is not None:
				viewport = self._view.pointerViewport
				if viewport is not None:
					self._startPoint = self._p_refreshHighlight( viewport )
					viewport.postRedraw()


	def _p_getMesh(self):
		if self._meshCell is not None:
			return self._meshCell.getValueAsClass( GSProductMesh )
		else:
			return None




class MeshKnifeFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, startTargetField, endTargetField, knifeEnabledField, backfaceCullField):
		super( MeshKnifeFieldManipulator, self ).__init__()
		self._startTargetField = startTargetField
		self._endTargetField = endTargetField
		self._knifeEnabledField = knifeEnabledField
		self._backfaceCullField = backfaceCullField


	def createElement(self, sheetEditContext):
		return MeshKnifeManipulator()

	def initialiseElement(self, element, sheetEditContext):
		startTarget = self._p_getCell( self._startTargetField, sheetEditContext )
		endTarget = self._p_getCell( self._endTargetField, sheetEditContext )
		knifeEnabled = self._p_getCell( self._knifeEnabledField, sheetEditContext )
		backfaceCull = self._p_getCell( self._backfaceCullField, sheetEditContext )
		element.attachCells( startTarget, endTarget, knifeEnabled, backfaceCull )
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




class ToolMeshKnife (GSProcedureTool):
	sheet = SheetRefField( ProcMeshKnife )

	functionClass = ProcMeshKnife
	pageTitle = _( 'Knife' )
	titleText = _( 'Knife' )


	bMarkedFacesOnlyCheck = BoolFieldEditCheckWithFn( sheet.bMarkedFacesOnly, _( 'Marked faces only' ) )
	bMarkCutEdgesCheck = BoolFieldEditCheckWithFn( sheet.bMarkCutEdges, _( 'Mark cut edges' ) )
	bCullEnabledCheck = BoolFieldEditCheckWithFn( sheet.bCullEnabled, _( 'Cut front faces only' ) )

	knifeManip = MeshKnifeFieldManipulator( sheet.startTarget, sheet.endTarget, sheet.bKnifeEnabled, sheet.faceCull )
	knifeManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click to set start point, hold, move to end point, and release.\n'  'Hold <shift> while pressing or releasing to constrain to vertices.\n'  'Hold <control> while pressing or releasing to constrain to edges' ) )
	layout.row()
	layout.row()  <<  bMarkedFacesOnlyCheck.check
	layout.row()  <<  bMarkCutEdgesCheck.check
	layout.row()  <<  bCullEnabledCheck.check
	layout.row()
	layout.finalise()



