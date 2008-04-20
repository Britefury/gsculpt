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

from Britefury.Math.Math import Colour3f, Vector3, Matrix4, Plane

from Britefury.Graphics.Graphics import glColor4Colour3fA, glVertex3Point3

from Britefury.FileIO.IOXml import IOXmlEnum, ioXmlReadBoolNode, ioXmlWriteBoolNode, ioXmlReadFloatNode, ioXmlWriteFloatNode, ioObjectFactoryRegister, ioObjectFactoryRegisterPrimitive

from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport import ViewportEventHandler, ViewportScrollListener
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.gSculptConfig import gSculptConfig

from Britefury.UI.FormLayout import FormLayoutFactory

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
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit
from Britefury.Model.Pick3d import Pick3d

from Britefury.Mesh.Mesh import GSProductMesh, CppMeshSurfaceTweakComponent, CppMeshSurfaceTweakComponentList, pyMeshSurfaceTweakComponentList_to_CppMeshSurfaceTweakComponentList, MeshSurfaceTweakComponentTarget, MeshSurfaceTweakComponentMode, MeshSurfaceTweakAdjuster, MeshSurfaceTweakNormalAdjuster

from Britefury.MeshTools.MeshEditComponent import MeshEditComponent







# XML IO for MeshSurfaceTweakComponentTarget

targetIOXmlEnum = IOXmlEnum( MeshSurfaceTweakComponentTarget, 'MeshSurfaceTweakComponentTarget', MeshSurfaceTweakComponentTarget.VERTICES )
targetIOXmlEnum.value( MeshSurfaceTweakComponentTarget.VERTICES, 'vertices' )
targetIOXmlEnum.value( MeshSurfaceTweakComponentTarget.EDGES, 'edges' )
targetIOXmlEnum.value( MeshSurfaceTweakComponentTarget.EDGERINGS, 'edge_rings' )
targetIOXmlEnum.value( MeshSurfaceTweakComponentTarget.EDGELOOPS, 'edge_loops' )
targetIOXmlEnum.value( MeshSurfaceTweakComponentTarget.FACES, 'faces' )

ioXmlReadMeshSurfaceTweakComponentTargetProp = targetIOXmlEnum.propReader
ioXmlReadMeshSurfaceTweakComponentTargetNode = targetIOXmlEnum.nodeReader
ioXmlWriteMeshSurfaceTweakComponentTargetProp = targetIOXmlEnum.propWriter
ioXmlWriteMeshSurfaceTweakComponentTargetNode = targetIOXmlEnum.nodeWriter



# Cell for MeshSurfaceTweakComponentTarget

class MeshSurfaceTweakComponentTargetCell (Cell):
	valueClass = MeshSurfaceTweakComponentTarget
	bAllowNone = False


registerCellType( MeshSurfaceTweakComponentTarget, MeshSurfaceTweakComponentTargetCell )


# Cell editor for MeshSurfaceTweakComponentTarget

class MeshSurfaceTweakComponentTargetCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshSurfaceTweakComponentTargetCellEditRadioGroup, self ).__init__()
		self.vertices = self.addChoice( MeshSurfaceTweakComponentTarget.VERTICES, _( 'Vertices' ) )
		self.edges = self.addChoice( MeshSurfaceTweakComponentTarget.EDGES, _( 'Edges' ) )
		self.edgeRings = self.addChoice( MeshSurfaceTweakComponentTarget.EDGERINGS, _( 'Edge rings' ) )
		self.edgeLoops = self.addChoice( MeshSurfaceTweakComponentTarget.EDGELOOPS, _( 'Edge loops' ) )
		self.faces = self.addChoice( MeshSurfaceTweakComponentTarget.FACES, _( 'Faces' ) )




# Field editor for MeshSurfaceTweakComponentTarget

class MeshSurfaceTweakComponentTargetFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'vertices', 'edges', 'edgeRings', 'edgeLoops', 'faces' ]

	def createElement(self, sheetEditContext):
		return MeshSurfaceTweakComponentTargetCellEditRadioGroup()









# XML IO for MeshSurfaceTweakComponentMode

modeIOXmlEnum = IOXmlEnum( MeshSurfaceTweakComponentMode, 'MeshSurfaceTweakComponentMode', MeshSurfaceTweakComponentMode.SURFACE )
modeIOXmlEnum.value( MeshSurfaceTweakComponentMode.SURFACE, 'surface' )
modeIOXmlEnum.value( MeshSurfaceTweakComponentMode.NORMAL, 'normal' )

ioXmlReadMeshSurfaceTweakComponentModeProp = modeIOXmlEnum.propReader
ioXmlReadMeshSurfaceTweakComponentModeNode = modeIOXmlEnum.nodeReader
ioXmlWriteMeshSurfaceTweakComponentModeProp = modeIOXmlEnum.propWriter
ioXmlWriteMeshSurfaceTweakComponentModeNode = modeIOXmlEnum.nodeWriter


# Cell for MeshSurfaceTweakComponentMode

class MeshSurfaceTweakComponentModeCell (Cell):
	valueClass = MeshSurfaceTweakComponentMode
	bAllowNone = False






#
# MeshSurfaceTweakComponent class
#

class MeshSurfaceTweakComponent (object):
	def __init__(self, pick=None, target=MeshSurfaceTweakComponentTarget.VERTICES, mode=MeshSurfaceTweakComponentMode.SURFACE, bClearSelection=True,
				viewVector=None, displacementAlongSurface=None, displacementAlongNormal=None):
		super( MeshSurfaceTweakComponent, self ).__init__()
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.target = target
		self.mode = mode
		self.bClearSelection = bClearSelection
		if viewVector is None:
			viewVector = Vector3()
		self.viewVector = copy( viewVector )
		if displacementAlongSurface is None:
			displacementAlongSurface = Vector3()
		self.displacementAlongSurface = copy( displacementAlongSurface )
		if displacementAlongNormal is None:
			displacementAlongNormal = 0.0
		self.displacementAlongNormal = displacementAlongNormal


	def toCppMeshSurfaceTweakComponent(self):
		return CppMeshSurfaceTweakComponent( self.pick.toMPick(), self.target, self.mode, self.bClearSelection, self.viewVector, self.displacementAlongSurface, self.displacementAlongNormal )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.target = ioXmlReadMeshSurfaceTweakComponentTargetNode( xmlNode.getChild( 'target' ), self.target )
		self.mode = ioXmlReadMeshSurfaceTweakComponentModeNode( xmlNode.getChild( 'mode' ), self.mode )
		self.bClearSelection = ioXmlReadBoolNode( xmlNode.getChild( 'b_clear_selection' ), self.bClearSelection )
		xmlNode.getChild( 'view_vector' )  >>  self.viewVector
		xmlNode.getChild( 'displacement_along_surface' )  >>  self.displacementAlongSurface
		self.displacementAlongNormal = ioXmlReadFloatNode( xmlNode.getChild( 'displacement_along_normal' ), self.displacementAlongNormal )

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteMeshSurfaceTweakComponentTargetNode( xmlNode.addChild( 'target' ), self.target )
		ioXmlWriteMeshSurfaceTweakComponentModeNode( xmlNode.addChild( 'mode' ), self.mode )
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_clear_selection' ), self.bClearSelection )
		xmlNode.addChild( 'view_vector' )  <<  self.viewVector
		xmlNode.addChild( 'displacement_along_surface' )  <<  self.displacementAlongSurface
		ioXmlWriteFloatNode( xmlNode.addChild( 'displacement_along_normal' ), self.displacementAlongNormal )


	def __copy__(self):
		return MeshSurfaceTweakComponent( copy( self.pick ), self.target, self.mode, self.bClearSelection,						\
										copy( self.viewVector ), copy( self.displacementAlongSurface ), self.displacementAlongNormal )

ioObjectFactoryRegister( 'MeshSurfaceTweakComponent', MeshSurfaceTweakComponent )










class ProcMeshSurfaceTweakComponents (GSProcedure):
	description = _( 'Surface tweak components' )

	tweaks = Field( TargetList, TargetList() )

	inputClass = GSProductMesh



	def procedureInvoke(self, inputObject, errorHandler):
		cppTweaks = CppMeshSurfaceTweakComponentList()
		pyMeshSurfaceTweakComponentList_to_CppMeshSurfaceTweakComponentList( [ tweak.toCppMeshSurfaceTweakComponent()   for tweak in self.tweaks ], cppTweaks )
		inputObject.surfaceTweakComponents( cppTweaks, True )
		return inputObject






class MeshSurfaceTweakComponentsManipulator (Manipulator, ViewportEventHandler, ViewportScrollListener, Viewport3dPainter):
	EV_BUTTON_DOWN = 0
	EV_BUTTON_UP = 1
	EV_MOTION = 2
	EV_LEAVE = 3
	EV_TARGET = 4
	EV_RESET = 5
	EV_OUTPUTCHANGED = 6


	class Event (object):
		def __init__(self, eventType, **kwargs):
			self.eventType = eventType
			self.__dict__.update( kwargs )


	class ResetException:
		pass



	def __init__(self):
		super( MeshSurfaceTweakComponentsManipulator, self ).__init__()

		self._tweaksCell = None
		self._targetCell = None

		self._tweakTarget = None
		self._mode = MeshSurfaceTweakComponentMode.SURFACE

		self._coEventHandler = None

		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeRingOrLoopSegments = []
		self._highlightFaceTriangles = None
		self._initialTargetCentre = None

		self._f_onAdjustTweak = None
		self._f_onDebugDraw = None

		self._meshCell = None

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		super( MeshSurfaceTweakComponentsManipulator, self ).manipAttachToView( view )
		self._coEventHandler = CoEventHandler( self._p_tweakLoop )
		self._coEventHandler.start()
		self._view.addEventHandler( self, Viewport3d )
		self._view.addScrollListener( self, Viewport3d )
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removeScrollListener( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self._coEventHandler.terminate()
		self._coEventHandler = None
		super( MeshSurfaceTweakComponentsManipulator, self ).manipDetachFromView()



	def attachCells(self, tweaks, target):
		self._tweaksCell = tweaks
		self._targetCell = target
		self._targetCell.changedSignal.connect( self._p_onTarget )

	def detachCells(self):
		self._targetCell.changedSignal.connect( self._p_onTarget )
		self._tweaksCell = None
		self._targetCell = None


	def _o_setUISensitivity(self, bSensitive):
		pass



	def attachMeshCell(self, cell):
		assert self._meshCell is None, 'mesh cell already attached'
		self._meshCell = cell
		self._meshCell.changedSignal.connect( self._p_onOutputMeshCellValueChanged )

	def detachMeshCell(self):
		assert self._meshCell is not None, 'no mesh cell attached'
		self._meshCell.changedSignal.disconnect( self._p_onOutputMeshCellValueChanged )
		self._meshCell = None


	def attachInputMeshCell(self, cell):
		cell.changedSignal.connect( self._p_onMeshCellValueChanged )

	def detachInputMeshCell(self, cell):
		cell.changedSignal.disconnect( self._p_onMeshCellValueChanged )



	def _p_getTarget(self, viewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs):
		mesh = self._p_getMesh()
		targetPick = viewport.createPick()

		target = self._targetCell.value
		if target == MeshSurfaceTweakComponentTarget.VERTICES:
			# Target vertices
			vertexIndex, thruFaceIndex = mesh.pickVertex( targetPick.toMPick() )

			if vertexIndex != targetComponentIndex:
				if vertexIndex != -1:
					# Highlight the vertex
					self._highlightVertexPosition = mesh.getVertexPosition( vertexIndex )
					self._initialTargetCentre = self._highlightVertexPosition
				else:
					self._highlightVertexPosition = None
					self._initialTargetCentre = None
				self._view.postRedraw()

			targetComponentIndex = vertexIndex
		elif target == MeshSurfaceTweakComponentTarget.EDGES:
			edgeIndex, closestPoint, thruFaceIndex = mesh.pickEdge( targetPick.toMPick() )

			if edgeIndex != targetComponentIndex:
				if edgeIndex != -1:
					# Highlight the edge
					self._highlightEdgeSegment = mesh.getEdgeSegment( edgeIndex )
					self._initialTargetCentre = self._highlightEdgeSegment.getMidPoint()
				else:
					self._highlightEdgeSegment = None
					self._initialTargetCentre = None
				self._view.postRedraw()

			targetComponentIndex = edgeIndex
		elif target == MeshSurfaceTweakComponentTarget.EDGERINGS  or  target == MeshSurfaceTweakComponentTarget.EDGELOOPS:
			seedIndex, closestPoint, thruFaceIndex = mesh.pickEdge( targetPick.toMPick() )


			if seedIndex != targetComponentIndex:
				if seedIndex != -1:
					def getHighlight(seedToList, seedToSegs, discoverFunction):
						segs = []
						try:
							segs = seedToSegs[seedIndex]
						except KeyError:
							try:
								edgeIndices = seedToList[seedIndex]
							except KeyError:
								edgeIndices = discoverFunction( seedIndex )
								seedToList[seedIndex] = edgeIndices
							segs = [ mesh.getEdgeSegment( edgeIndex )   for edgeIndex in edgeIndices ]
							seedToSegs[seedIndex] = segs
						return segs

					# Highlight the edges
					if target == MeshSurfaceTweakComponentTarget.EDGERINGS:
						self._highlightEdgeRingOrLoopSegments = getHighlight( ringSeedToRing, ringSeedToRingSegs, mesh.discoverEdgeRing )
					elif target == MeshSurfaceTweakComponentTarget.EDGELOOPS:
						self._highlightEdgeRingOrLoopSegments = getHighlight( loopSeedToLoop, loopSeedToLoopSegs, mesh.discoverEdgeLoop )
					self._initialTargetCentre = mesh.getEdgeSegment( seedIndex ).getMidPoint()
				else:
					self._highlightEdgeRingOrLoopSegments = []
					self._initialTargetCentre = None
				self._view.postRedraw()

			targetComponentIndex = seedIndex
		elif target == MeshSurfaceTweakComponentTarget.FACES:
			faceIndex, intersectionPoint = mesh.pickFace( targetPick.toMPick() )

			if faceIndex != targetComponentIndex:
				if faceIndex != -1:
					# Highlight the face
					self._highlightFaceTriangles = mesh.getFaceTriangles( faceIndex )
					self._initialTargetCentre = mesh.getFaceCentroid( faceIndex )
				else:
					self._highlightFaceTriangles = None
					self._initialTargetCentre = None
				self._view.postRedraw()

			targetComponentIndex = faceIndex
		else:
			assert False, 'invalid target'

		return targetPick, targetComponentIndex



	def _p_getTweakFromUser(self, ringSeedToRing, loopSeedToLoop):
		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeRingOrLoopSegments = []
		self._highlightFaceTriangles = None
		self._initialTargetCentre = None
		self._tweakTarget = None
		self._mode = MeshSurfaceTweakComponentMode.SURFACE

		targetComponentIndex = -1
		targetPick = None
		displacementAlongSurface = Vector3()
		displacementAlongNormal = 0.0
		targetDistance = None
		ringSeedToRingSegs = {}
		loopSeedToLoopSegs = {}
		viewport = None
		viewVector = None

		if self._view.pointerViewport is not None:
			targetPick, targetComponentIndex = self._p_getTarget( self._view.pointerViewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs )

		event = self._coEventHandler.waitForNextEvent()

		# Get the target component
		while not ( event.eventType == MeshSurfaceTweakComponentsManipulator.EV_BUTTON_DOWN  and  targetComponentIndex != -1 ):
			if event.eventType == MeshSurfaceTweakComponentsManipulator.EV_RESET:
				self._view.postRedraw()
				raise MeshSurfaceTweakComponentsManipulator.ResetException

			if event.eventType == MeshSurfaceTweakComponentsManipulator.EV_TARGET  or  event.eventType == MeshSurfaceTweakComponentsManipulator.EV_OUTPUTCHANGED:
				self._highlightVertexPosition = None
				self._highlightEdgeSegment = None
				self._highlightEdgeRingOrLoopSegments = []
				self._highlightFaceTriangles = None
				targetComponentIndex = -1
				if event.eventType == MeshSurfaceTweakComponentsManipulator.EV_OUTPUTCHANGED:
					ringSeedToRingSegs = {}
					loopSeedToLoopSegs = {}

				if self._view.pointerViewport is not None:
					targetPick, targetComponentIndex = self._p_getTarget( self._view.pointerViewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs )
			else:
				viewport = event.viewport

				if event.eventType != MeshSurfaceTweakComponentsManipulator.EV_LEAVE  and  viewport is not None:
					targetPick, targetComponentIndex = self._p_getTarget( viewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs )
					viewVector = viewport.camera.cameraPosition.forwardAxis

			event = self._coEventHandler.waitForNextEvent()


		if viewport.pointerStatus.bShiftKey:
			self._mode = MeshSurfaceTweakComponentMode.NORMAL
		else:
			self._mode = MeshSurfaceTweakComponentMode.SURFACE
		self._tweakTarget = self._targetCell.value


		# We have a target component
		# Remove the highlight
		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeRingOrLoopSegments = []
		self._highlightFaceTriangles = None

		# If the selection is valid
		if self._initialTargetCentre is not None:
			targetDistance = event.viewport.camera.transformWorldToCameraZ( self._initialTargetCentre )
			startPoint = event.viewport.pointerStatus.positionF
			startPointWorld = event.viewport.camera.transformScreenToWorldCameraZ( startPoint, targetDistance )
			bScaledMotion = event.bScaledMotion

			# Add the tweak object to the tweak list
			self._tweaksCell.literalValue = self._tweaksCell.value + [ MeshSurfaceTweakComponent( deepcopy( targetPick ), self._targetCell.value, self._mode, not event.viewport.pointerStatus.bControlKey, viewVector, Vector3(), 0.0 ) ]
			self._view.forceRedraw()
		else:
			return


		event = self._coEventHandler.waitForNextEvent()

		# Now, track the displacement
		while not( event.eventType == MeshSurfaceTweakComponentsManipulator.EV_BUTTON_UP  and  event.bScaledMotion == bScaledMotion ):
			if event.eventType == MeshSurfaceTweakComponentsManipulator.EV_RESET:
				self._view.postRedraw()
				raise MeshSurfaceTweakComponentsManipulator.ResetException

			if not( event.eventType == MeshSurfaceTweakComponentsManipulator.EV_TARGET  or  event.eventType == MeshSurfaceTweakComponentsManipulator.EV_OUTPUTCHANGED ):
				endPoint = event.viewport.pointerStatus.positionF
				endPointWorld = event.viewport.camera.transformScreenToWorldCameraZ( endPoint, targetDistance )

				displacementAlongSurface = endPointWorld - startPointWorld
				displacementAlongNormal = event.viewport.camera.transformDistanceAcrossScreenToDistanceThruWorld( endPoint.x - startPoint.x, targetDistance )

				if bScaledMotion:
					displacementAlongSurface *= gSculptConfig.manipulatorFineTuneMotionScale
					displacementAlongNormal *= gSculptConfig.manipulatorFineTuneMotionScale

				if self._f_onAdjustTweak is not None:
					self._f_onAdjustTweak( self._tweakTarget, self._mode, viewVector, displacementAlongSurface, displacementAlongNormal )
				self._view.forceRedraw()

			event = self._coEventHandler.waitForNextEvent()


		assert len( self._tweaksCell.value ) > 0, 'no tweaks'
		topTweak = self._tweaksCell.value[-1]
		newTopTweak = MeshSurfaceTweakComponent( topTweak.pick, topTweak.target, topTweak.mode, topTweak.bClearSelection,					\
											topTweak.viewVector, displacementAlongSurface, displacementAlongNormal )
		tweaks = self._tweaksCell.value
		tweaks[-1] = newTopTweak
		self._tweaksCell.literalValue = tweaks
		self._p_commandHistoryBreak()



	def _p_tweakLoop(self):
		ringSeedToRing = {}
		loopSeedToLoop = {}
		while True:
			try:
				self._p_getTweakFromUser( ringSeedToRing, loopSeedToLoop )
			except MeshSurfaceTweakComponentsManipulator.ResetException:
				print 'MeshSurfaceTweakComponentsManipulator._p_tweakLoop: reset'
				ringSeedToRing = {}
				loopSeedToLoop = {}
				self._view.postRedraw()



	def evPrimaryButtonDown(self, viewport):
		super( MeshSurfaceTweakComponentsManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=False ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshSurfaceTweakComponentsManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=False ) )


	def evSecondaryButtonDown(self, viewport):
		super( MeshSurfaceTweakComponentsManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=True ) )

	def evSecondaryButtonUp(self, viewport):
		super( MeshSurfaceTweakComponentsManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=True ) )


	def evMotion(self, viewport, point):
		super( MeshSurfaceTweakComponentsManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_MOTION, viewport=viewport ) )

	def evEnter(self, viewport, point):
		super( MeshSurfaceTweakComponentsManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_MOTION, viewport=viewport ) )

	def evLeave(self, viewport, point):
		super( MeshSurfaceTweakComponentsManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_LEAVE, viewport=viewport ) )


	def evScroll(self, viewport, x, y):
		v = self._targetCell.value
		v -= y
		while v < MeshSurfaceTweakComponentTarget.VERTICES:
			v += ( MeshSurfaceTweakComponentTarget.FACES + 1 )
		while v > MeshSurfaceTweakComponentTarget.FACES:
			v -= ( MeshSurfaceTweakComponentTarget.FACES + 1 )
		self._targetCell.literalValue = MeshSurfaceTweakComponentTarget( v )




	def evPaint3d(self, viewport, paintLayer):
		# Paint the target highlight
		if self._highlightVertexPosition is not None:
			ModelDraw.drawVertexPickHighlight( self._highlightVertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
		if self._highlightEdgeSegment is not None:
			ModelDraw.drawEdgePickHighlight( self._highlightEdgeSegment, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
		for seg in self._highlightEdgeRingOrLoopSegments:
			ModelDraw.drawEdgePickHighlight( seg, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
		if self._highlightFaceTriangles is not None:
			ModelDraw.drawFacePickHighlight( self._highlightFaceTriangles, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )
		if self._f_onDebugDraw is not None:
			self._f_onDebugDraw( paintLayer )



	def _p_onTarget(self):
		self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_TARGET ) )

	def _p_onConstraints(self):
		self._view.postRedraw()


	def _p_onMeshCellValueChanged(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_RESET ) )

	def _p_onOutputMeshCellValueChanged(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshSurfaceTweakComponentsManipulator.Event( MeshSurfaceTweakComponentsManipulator.EV_OUTPUTCHANGED ) )

	def _p_getMesh(self):
		if self._meshCell is not None:
			return self._meshCell.getValueAsClass( GSProductMesh )
		else:
			return None





class MeshSurfaceTweakComponentsFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, tweaksField, targetField):
		super( MeshSurfaceTweakComponentsFieldManipulator, self ).__init__()
		self._tweaksField = tweaksField
		self._targetField = targetField


	def createElement(self, sheetEditContext):
		return MeshSurfaceTweakComponentsManipulator()

	def destroyElement(self, element, sheetEditContext):
		del sheetEditContext.sheetEditor.tweakManip

	def initialiseElement(self, element, sheetEditContext):
		tweaks = self._p_getCell( self._tweaksField, sheetEditContext )
		target = self._p_getCell( self._targetField, sheetEditContext )
		element.attachCells( tweaks, target )
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
		element.attachInputMeshCell( cell )

	def detachInputProductCell(self, element, cell):
		element.detachInputMeshCell( cell )

	def attachOutputProductCell(self, element, cell):
		element.attachMeshCell( cell )

	def detachOutputProductCell(self, element, cell):
		element.detachMeshCell()




class TweakComponentsAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshSurfaceTweakComponents

	def initInvoke(self):
		# Initialise with full pre-compute. This will remove the cost of the full pre-compute which would normally occur on each modification of the tweak list
		self._inputObject.preComputeFull()

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTweaks = CppMeshSurfaceTweakComponentList()
		pyMeshSurfaceTweakComponentList_to_CppMeshSurfaceTweakComponentList( [ tweak.toCppMeshSurfaceTweakComponent()   for tweak in self._proc.tweaks ], cppTweaks )
		self.mesh.surfaceTweakComponents( cppTweaks, False )
		self.surfaceAdjusters = None
		self.normalAdjusters = None


	def adjustTweak(self, tweakTarget, mode, viewVector, displacementAlongSurface, displacementAlongNormal):
		if mode == MeshSurfaceTweakComponentMode.SURFACE:
			if self.surfaceAdjusters is None:
				self.surfaceAdjusters = MeshSurfaceTweakAdjuster()
				if tweakTarget == MeshSurfaceTweakComponentTarget.VERTICES:
					self.mesh.surfaceTweakAdjustMarkedVertices( self.surfaceAdjusters, viewVector )
				elif tweakTarget == MeshSurfaceTweakComponentTarget.EDGES  or								\
						tweakTarget == MeshSurfaceTweakComponentTarget.EDGERINGS  or						\
						tweakTarget == MeshSurfaceTweakComponentTarget.EDGELOOPS:
					self.mesh.surfaceTweakAdjustMarkedEdges( self.surfaceAdjusters, viewVector )
				if tweakTarget == MeshSurfaceTweakComponentTarget.FACES:
					self.mesh.surfaceTweakAdjustMarkedFaces( self.surfaceAdjusters, viewVector )
			self.surfaceAdjusters.tweak( displacementAlongSurface )
		elif mode == MeshSurfaceTweakComponentMode.NORMAL:
			if self.normalAdjusters is None:
				self.normalAdjusters = MeshSurfaceTweakNormalAdjuster()
				if tweakTarget == MeshSurfaceTweakComponentTarget.VERTICES:
					self.mesh.surfaceTweakNormalAdjustMarkedVertices( self.normalAdjusters )
				elif tweakTarget == MeshSurfaceTweakComponentTarget.EDGES  or								\
						tweakTarget == MeshSurfaceTweakComponentTarget.EDGERINGS  or						\
						tweakTarget == MeshSurfaceTweakComponentTarget.EDGELOOPS:
					self.mesh.surfaceTweakNormalAdjustMarkedEdges( self.normalAdjusters )
				if tweakTarget == MeshSurfaceTweakComponentTarget.FACES:
					self.mesh.surfaceTweakNormalAdjustMarkedFaces( self.normalAdjusters )
			self.normalAdjusters.tweak( displacementAlongNormal )

	def debugDraw(self, paintLayer):
		if self.surfaceAdjusters is not None:
			if paintLayer == PaintLayer.PAINTLAYER_TRANSPARENCY:
				self.surfaceAdjusters.debugDraw()


	def getResult(self):
		return self.mesh






class ToolMeshSurfaceTweakComponents (GSProcedureTool):
	sheet = SheetRefField( ProcMeshSurfaceTweakComponents )

	functionClass = ProcMeshSurfaceTweakComponents
	adjustableInvoker = TweakComponentsAdjustableInvoker
	pageTitle = _( 'SurfTwk' )
	titleText = _( 'Surface Tweak Components' )


	tweakTarget = Field( MeshSurfaceTweakComponentTarget, MeshSurfaceTweakComponentTarget.VERTICES )


	tweakListLabel = FieldLabelWithFn( sheet.tweaks, _( 'Tweaks:' ) )
	tweakListEditor = TargetListFieldEdit( sheet.tweaks, targetLabelText=_( '%d tweak' ), pluralTargetLabelText=_( '%d tweaks' ), removeTargetButtonText=_( 'Remove tweak' ) )

	tweakManip = MeshSurfaceTweakComponentsFieldManipulator( sheet.tweaks, tweakTarget )
	tweakManip.enable()

	targetRadio = MeshSurfaceTweakComponentTargetFieldEditRadioGroup( tweakTarget )



	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click and drag to move along surface.\n'  'Hold <shift> to move along surface normal.\n'  'Selection is cleared on each click, unless <control> is pressed.\n'  'Use the scroll wheel to switch between component types.' ) )
	layout.row()
	layout.row()  <<  _( 'Component type:' )
	layout.row()  <<  targetRadio.vertices
	layout.row()  <<  targetRadio.edges
	layout.row()  <<  targetRadio.edgeRings
	layout.row()  <<  targetRadio.edgeLoops
	layout.row()  <<  targetRadio.faces
	layout.row()
	layout.row()  <<  tweakListLabel.label
	layout.row()  <<  tweakListEditor.numTargets
	layout.row()  <<  None  <<  ( tweakListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		super( ToolMeshSurfaceTweakComponents, self ).__init__( view, editorSettings, parentWindow, commandHistory, painterFactory )
		self.tweakManip._f_onAdjustTweak = self._onAdjustTweak
		#self.tweakManip._f_onDebugDraw = self._onDebugDraw


	def shutdownTool(self):
		self.tweakManip._f_onAdjustTweak = None
		#self.tweakManip._f_onDebugDraw = None
		super( ToolMeshSurfaceTweakComponents, self ).shutdownTool()



	def _onAdjustTweak(self, tweakTarget, mode, viewVector, displacementAlongSurface, displacementAlongNormal):
		self._adjustableInvoker.adjustTweak( tweakTarget, mode, viewVector, displacementAlongSurface, displacementAlongNormal )

	def _onDebugDraw(self, paintLayer):
		self._adjustableInvoker.debugDraw( paintLayer )




	def setTarget(self, target):
		self.tweakTarget = target


