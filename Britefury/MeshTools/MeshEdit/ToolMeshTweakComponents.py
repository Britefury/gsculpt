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

from Britefury.FileIO.IOXml import IOXmlEnum, ioXmlReadBoolNode, ioXmlWriteBoolNode, ioObjectFactoryRegister

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

from Britefury.Mesh.Mesh import GSProductMesh, CppMeshTweakComponent, CppMeshTweakComponentList, pyMeshTweakComponentList_to_CppMeshTweakComponentList, MeshTweakComponentTarget

from Britefury.MeshTools.MeshEditComponent import MeshEditComponent






# XML IO for MeshTweakComponentTarget

targetIOXmlEnum = IOXmlEnum( MeshTweakComponentTarget, 'MeshTweakComponentTarget', MeshTweakComponentTarget.VERTICES )
targetIOXmlEnum.value( MeshTweakComponentTarget.VERTICES, 'vertices' )
targetIOXmlEnum.value( MeshTweakComponentTarget.EDGES, 'edges' )
targetIOXmlEnum.value( MeshTweakComponentTarget.EDGERINGS, 'edge_rings' )
targetIOXmlEnum.value( MeshTweakComponentTarget.EDGELOOPS, 'edge_loops' )
targetIOXmlEnum.value( MeshTweakComponentTarget.FACES, 'faces' )

ioXmlReadMeshTweakComponentTargetProp = targetIOXmlEnum.propReader
ioXmlReadMeshTweakComponentTargetNode = targetIOXmlEnum.nodeReader
ioXmlWriteMeshTweakComponentTargetProp = targetIOXmlEnum.propWriter
ioXmlWriteMeshTweakComponentTargetNode = targetIOXmlEnum.nodeWriter



# Cell for MeshTweakComponentTarget

class MeshTweakComponentTargetCell (Cell):
	valueClass = MeshTweakComponentTarget
	bAllowNone = False

registerCellType( MeshTweakComponentTarget, MeshTweakComponentTargetCell )


# Cell editor for MeshTweakComponentTarget

class MeshTweakComponentTargetCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( MeshTweakComponentTargetCellEditRadioGroup, self ).__init__()
		self.vertices = self.addChoice( MeshTweakComponentTarget.VERTICES, _( 'Vertices' ) )
		self.edges = self.addChoice( MeshTweakComponentTarget.EDGES, _( 'Edges' ) )
		self.edgeRings = self.addChoice( MeshTweakComponentTarget.EDGERINGS, _( 'Edge rings' ) )
		self.edgeLoops = self.addChoice( MeshTweakComponentTarget.EDGELOOPS, _( 'Edge loops' ) )
		self.faces = self.addChoice( MeshTweakComponentTarget.FACES, _( 'Faces' ) )




# Field editor for MeshTweakComponentTarget

class MeshTweakComponentTargetFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'vertices', 'edges', 'edgeRings', 'edgeLoops', 'faces' ]
	def createElement(self, sheetEditContext):
		return MeshTweakComponentTargetCellEditRadioGroup()








#
# MeshTweakComponent class
#

class MeshTweakComponent (object):
	def __init__(self, pick=None, target=MeshTweakComponentTarget.VERTICES, bClearSelection=True, translation=None):
		super( MeshTweakComponent, self ).__init__()
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.target = target
		self.bClearSelection = bClearSelection
		if translation is None:
			translation = Vector3()
		self.translation = translation


	def toCppMeshTweakComponent(self):
		return CppMeshTweakComponent( self.pick.toMPick(), self.target, self.bClearSelection, self.translation )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.target = ioXmlReadMeshTweakComponentTargetNode( xmlNode.getChild( 'target' ), self.target )
		self.bClearSelection = ioXmlReadBoolNode( xmlNode.getChild( 'b_clear_selection' ), self.bClearSelection )
		xmlNode.getChild( 'translation' )  >>  self.translation

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteMeshTweakComponentTargetNode( xmlNode.addChild( 'target' ), self.target )
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_clear_selection' ), self.bClearSelection )
		xmlNode.addChild( 'translation' )  <<  self.translation


	def __copy__(self):
		return MeshTweakComponent( copy( self.pick ), self.target, self.bClearSelection, copy( self.translation ) )

ioObjectFactoryRegister( 'MeshTweakComponent', MeshTweakComponent )










class ProcMeshTweakComponents (GSProcedure):
	description = _( 'Tweak components' )

	tweaks = Field( TargetList, TargetList() )
	bRestoreSelection = Field( bool, True )

	inputClass = GSProductMesh



	def procedureInvoke(self, inputObject, errorHandler):
		cppTweaks = CppMeshTweakComponentList()
		pyMeshTweakComponentList_to_CppMeshTweakComponentList( [ tweak.toCppMeshTweakComponent()   for tweak in self.tweaks ], cppTweaks )
		inputObject.tweakComponents( cppTweaks, self.bRestoreSelection )
		return inputObject




# Constraints enumeration
class TweakConstraints (Enum):
	NONE = 0
	X_PLANE = 1
	Y_PLANE = 2
	Z_PLANE = 3
	X_AXIS = 4
	Y_AXIS = 5
	Z_AXIS = 6


# Cell class for TweakConstraints
class TweakConstraintsCell (Cell):
	valueClass = TweakConstraints
	bAllowNone = False

registerCellType( TweakConstraints, TweakConstraintsCell )



# Cell editor for TweakConstraints

class TweakConstraintsCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( TweakConstraintsCellEditRadioGroup, self ).__init__()
		self.none = self.addChoice( TweakConstraints.NONE, 'XYZ' )
		self.xPlane = self.addChoice( TweakConstraints.X_PLANE, 'YZ' )
		self.yPlane = self.addChoice( TweakConstraints.Y_PLANE, 'XZ' )
		self.zPlane = self.addChoice( TweakConstraints.Z_PLANE, 'XY' )
		self.xAxis = self.addChoice( TweakConstraints.X_AXIS, 'X' )
		self.yAxis = self.addChoice( TweakConstraints.Y_AXIS, 'Y' )
		self.zAxis = self.addChoice( TweakConstraints.Z_AXIS, 'Z' )



# Field editor for TweakConstraints

class TweakConstraintsFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'none', 'xPlane', 'yPlane', 'zPlane', 'xAxis', 'yAxis', 'zAxis' ]


	def createElement(self, rowEditContext):
		return TweakConstraintsCellEditRadioGroup()




class MeshTweakComponentsManipulator (Manipulator, ViewportEventHandler, ViewportScrollListener, Viewport3dPainter):
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
		super( MeshTweakComponentsManipulator, self ).__init__()

		self._tweaksCell = None
		self._targetCell = None
		self._constraintsCell = None

		self._coEventHandler = None

		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeRingOrLoopSegments = []
		self._highlightFaceTriangles = None
		self._initialTargetCentre = None
		self._targetCentre = None

		self._f_onAdjustTranslation = None

		self._meshCell = None

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		super( MeshTweakComponentsManipulator, self ).manipAttachToView( view )
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
		super( MeshTweakComponentsManipulator, self ).manipDetachFromView()



	def attachCells(self, tweaks, target, constraints):
		self._tweaksCell = tweaks
		self._targetCell = target
		self._constraintsCell = constraints
		self._targetCell.changedSignal.connect( self._p_onTarget )
		self._constraintsCell.changedSignal.connect( self._p_onConstraints )

	def detachCells(self):
		self._targetCell.changedSignal.disconnect( self._p_onTarget )
		self._constraintsCell.changedSignal.disconnect( self._p_onConstraints )
		self._tweaksCell = None
		self._targetCell = None
		self._constraintsCell = None


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
		if target == MeshTweakComponentTarget.VERTICES:
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
		elif target == MeshTweakComponentTarget.EDGES:
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
		elif target == MeshTweakComponentTarget.EDGERINGS  or  target == MeshTweakComponentTarget.EDGELOOPS:
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
					if target == MeshTweakComponentTarget.EDGERINGS:
						self._highlightEdgeRingOrLoopSegments = getHighlight( ringSeedToRing, ringSeedToRingSegs, mesh.discoverEdgeRing )
					elif target == MeshTweakComponentTarget.EDGELOOPS:
						self._highlightEdgeRingOrLoopSegments = getHighlight( loopSeedToLoop, loopSeedToLoopSegs, mesh.discoverEdgeLoop )
					self._initialTargetCentre = mesh.getEdgeSegment( seedIndex ).getMidPoint()
				else:
					self._highlightEdgeRingOrLoopSegments = []
					self._initialTargetCentre = None
				self._view.postRedraw()

			targetComponentIndex = seedIndex
		elif target == MeshTweakComponentTarget.FACES:
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
		self._targetCentre = None
		targetComponentIndex = -1
		targetPick = None
		translation = Vector3()
		targetDistance = None
		ringSeedToRingSegs = {}
		loopSeedToLoopSegs = {}
		viewport = None

		if self._view.pointerViewport is not None:
			targetPick, targetComponentIndex = self._p_getTarget( self._view.pointerViewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs )

		event = self._coEventHandler.waitForNextEvent()

		# Get the target component
		while not ( event.eventType == MeshTweakComponentsManipulator.EV_BUTTON_DOWN  and  targetComponentIndex != -1 ):
			if event.eventType == MeshTweakComponentsManipulator.EV_RESET:
				self._view.postRedraw()
				raise MeshTweakComponentsManipulator.ResetException

			if event.eventType == MeshTweakComponentsManipulator.EV_TARGET  or  event.eventType == MeshTweakComponentsManipulator.EV_OUTPUTCHANGED:
				self._highlightVertexPosition = None
				self._highlightEdgeSegment = None
				self._highlightEdgeRingOrLoopSegments = []
				self._highlightFaceTriangles = None
				targetComponentIndex = -1
				if event.eventType == MeshTweakComponentsManipulator.EV_OUTPUTCHANGED:
					ringSeedToRingSegs = {}
					loopSeedToLoopSegs = {}

				if self._view.pointerViewport is not None:
					targetPick, targetComponentIndex = self._p_getTarget( self._view.pointerViewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs )
			else:
				viewport = event.viewport

			if event.eventType != MeshTweakComponentsManipulator.EV_LEAVE  and  viewport is not None:
				targetPick, targetComponentIndex = self._p_getTarget( viewport, targetComponentIndex, ringSeedToRing, loopSeedToLoop, ringSeedToRingSegs, loopSeedToLoopSegs )

			self._targetCentre = self._initialTargetCentre

			event = self._coEventHandler.waitForNextEvent()


		# We have a target component
		# Remove the highlight
		self._highlightVertexPosition = None
		self._highlightEdgeSegment = None
		self._highlightEdgeRingOrLoopSegments = []
		self._highlightFaceTriangles = None

		# If the selection is valid
		if self._initialTargetCentre is not None:
			self._targetCentre = self._initialTargetCentre
			targetDistance = event.viewport.camera.transformWorldToCameraZ( self._initialTargetCentre )
			startPoint = event.viewport.camera.transformScreenToWorldCameraZ( event.viewport.pointerStatus.positionF, targetDistance )
			bScaledMotion = event.bScaledMotion

			# Handle constraints
			constraints = self._constraintsCell.value
			constraintPlane = None
			if constraints == TweakConstraints.X_PLANE:
				constraintPlane = Plane( Vector3( 1.0, 0.0, 0.0 ), self._initialTargetCentre )
			elif constraints == TweakConstraints.Y_PLANE:
				constraintPlane = Plane( Vector3( 0.0, 1.0, 0.0 ), self._initialTargetCentre )
			elif constraints == TweakConstraints.Z_PLANE:
				constraintPlane = Plane( Vector3( 0.0, 0.0, 1.0 ), self._initialTargetCentre )
			elif constraints == TweakConstraints.X_AXIS  or  constraints == TweakConstraints.Y_AXIS  or  constraints == TweakConstraints.Z_AXIS:
				if constraints == TweakConstraints.X_AXIS:
					shaftAxis = Vector3( 1.0, 0.0, 0.0 )
				elif constraints == TweakConstraints.Y_AXIS:
					shaftAxis = Vector3( 0.0, 1.0, 0.0 )
				elif constraints == TweakConstraints.Z_AXIS:
					shaftAxis = Vector3( 0.0, 0.0, 1.0 )
				q = shaftAxis.cross( event.viewport.camera.cameraPosition.forwardAxis )
				if abs( q.sqrLength() )  >=  1.0e-6:
					normal = shaftAxis.cross( q )
					constraintPlane = Plane( normal, self._initialTargetCentre )

			startPointOnPlane = None
			if constraintPlane is not None:
				raySegment = event.viewport.camera.computeRay( event.viewport.pointerStatus.positionF )
				bSuccess, t, hitPoint = raySegment.intersect( constraintPlane )
				if bSuccess:
					startPointOnPlane = hitPoint


			# Add the tweak object to the tweak list
			self._tweaksCell.literalValue = self._tweaksCell.value + [ MeshTweakComponent( deepcopy( targetPick ), self._targetCell.value, not event.viewport.pointerStatus.bControlKey, Vector3() ) ]
		else:
			return


		event = self._coEventHandler.waitForNextEvent()

		# Now, track the translation
		while not( event.eventType == MeshTweakComponentsManipulator.EV_BUTTON_UP  and  event.bScaledMotion == bScaledMotion ):
			if event.eventType == MeshTweakComponentsManipulator.EV_RESET:
				self._view.postRedraw()
				raise MeshTweakComponentsManipulator.ResetException

			if not( event.eventType == MeshTweakComponentsManipulator.EV_TARGET  or  event.eventType == MeshTweakComponentsManipulator.EV_OUTPUTCHANGED ):
				endPoint = event.viewport.camera.transformScreenToWorldCameraZ( event.viewport.pointerStatus.positionF, targetDistance )
				endPointOnPlane = None

				if startPointOnPlane is not None  and  constraintPlane is not None:
					raySegment = event.viewport.camera.computeRay( event.viewport.pointerStatus.positionF )
					bSuccess, t, hitPoint = raySegment.intersect( constraintPlane )
					if bSuccess:
						endPointOnPlane = hitPoint

				if endPointOnPlane is not None:
					translation = endPointOnPlane - startPointOnPlane
				else:
					translation = endPoint - startPoint

					if constraints == TweakConstraints.X_PLANE:
						translation.projectOntoPlane( Vector3( 1.0, 0.0, 0.0 ) )
					elif constraints == TweakConstraints.Y_PLANE:
						translation.projectOntoPlane( Vector3( 0.0, 1.0, 0.0 ) )
					elif constraints == TweakConstraints.Z_PLANE:
						translation.projectOntoPlane( Vector3( 0.0, 0.0, 1.0 ) )


				if constraints == TweakConstraints.X_AXIS  or  constraints == TweakConstraints.Y_AXIS  or  constraints == TweakConstraints.Z_AXIS:
					if constraints == TweakConstraints.X_AXIS:
						shaftAxis = Vector3( 1.0, 0.0, 0.0 )
					elif constraints == TweakConstraints.Y_AXIS:
						shaftAxis = Vector3( 0.0, 1.0, 0.0 )
					elif constraints == TweakConstraints.Z_AXIS:
						shaftAxis = Vector3( 0.0, 0.0, 1.0 )
					translation = translation.projectOntoUnitVector( shaftAxis )


				if bScaledMotion:
					translation *= gSculptConfig.manipulatorFineTuneMotionScale
				self._targetCentre = self._initialTargetCentre + translation
				if self._f_onAdjustTranslation is not None:
					self._f_onAdjustTranslation( translation )
				self._view.forceRedraw()

			event = self._coEventHandler.waitForNextEvent()


		assert len( self._tweaksCell.value ) > 0, 'no tweaks'
		topTweak = self._tweaksCell.value[-1]
		newTopTweak = MeshTweakComponent( topTweak.pick, topTweak.target, topTweak.bClearSelection, translation )
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
			except MeshTweakComponentsManipulator.ResetException:
				ringSeedToRing = {}
				loopSeedToLoop = {}
				self._view.postRedraw()



	def evPrimaryButtonDown(self, viewport):
		super( MeshTweakComponentsManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=False ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshTweakComponentsManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=False ) )


	def evSecondaryButtonDown(self, viewport):
		super( MeshTweakComponentsManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=True ) )

	def evSecondaryButtonUp(self, viewport):
		super( MeshTweakComponentsManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=True ) )


	def evMotion(self, viewport, point):
		super( MeshTweakComponentsManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_MOTION, viewport=viewport ) )

	def evEnter(self, viewport, point):
		super( MeshTweakComponentsManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_MOTION, viewport=viewport ) )

	def evLeave(self, viewport, point):
		super( MeshTweakComponentsManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_LEAVE, viewport=viewport ) )


	def evScroll(self, viewport, x, y):
		if viewport.pointerStatus.bShiftKey:
			v = self._constraintsCell.value
			v -= y
			while v < TweakConstraints.NONE:
				v += ( TweakConstraints.Z_AXIS + 1 )
			while v > TweakConstraints.Z_AXIS:
				v -= ( TweakConstraints.Z_AXIS + 1 )
			self._constraintsCell.literalValue = TweakConstraints( v )
		else:
			v = self._targetCell.value
			v -= y
			while v < MeshTweakComponentTarget.VERTICES:
				v += ( MeshTweakComponentTarget.FACES + 1 )
			while v > MeshTweakComponentTarget.FACES:
				v -= ( MeshTweakComponentTarget.FACES + 1 )
			self._targetCell.literalValue = MeshTweakComponentTarget( v )




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
		if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH:
			if self._targetCentre is not None:
				constraints = self._constraintsCell.value
				if constraints != TweakConstraints.NONE:
					targetDistance = viewport.camera.transformWorldToCameraZ( self._targetCentre )
					mag = viewport.camera.transformDistanceAcrossScreenToDistanceThruWorld( ModelDraw.TWEAK_CONSTRAINT_WIDGET_SIZE * viewport.size.y, targetDistance )

					glLineWidth( 4.0 )

					if constraints == TweakConstraints.X_AXIS  or  constraints == TweakConstraints.Y_PLANE  or  constraints == TweakConstraints.Z_PLANE:
						glColor4Colour3fA( Colour3f( 1.0, 0.0, 0.0 ) * ModelDraw.TWEAK_CONSTRAINT_WIDGET_BRIGHTNESS, ModelDraw.TWEAK_CONSTRAINT_WIDGET_ALPHA )
						glBegin( GL_LINES )
						glVertex3Point3( self._targetCentre - Vector3( mag, 0.0, 0.0 ) )
						glVertex3Point3( self._targetCentre + Vector3( mag, 0.0, 0.0 ) )
						glEnd()

					if constraints == TweakConstraints.Y_AXIS  or  constraints == TweakConstraints.X_PLANE  or  constraints == TweakConstraints.Z_PLANE:
						glColor4Colour3fA( Colour3f( 0.0, 1.0, 0.0 ) * ModelDraw.TWEAK_CONSTRAINT_WIDGET_BRIGHTNESS, ModelDraw.TWEAK_CONSTRAINT_WIDGET_ALPHA )
						glBegin( GL_LINES )
						glVertex3Point3( self._targetCentre - Vector3( 0.0, mag, 0.0 ) )
						glVertex3Point3( self._targetCentre + Vector3( 0.0, mag, 0.0 ) )
						glEnd()

					if constraints == TweakConstraints.Z_AXIS  or  constraints == TweakConstraints.X_PLANE  or  constraints == TweakConstraints.Y_PLANE:
						glColor4Colour3fA( Colour3f( 0.125, 0.375, 1.0 ) * ModelDraw.TWEAK_CONSTRAINT_WIDGET_BRIGHTNESS, ModelDraw.TWEAK_CONSTRAINT_WIDGET_ALPHA )
						glBegin( GL_LINES )
						glVertex3Point3( self._targetCentre - Vector3( 0.0, 0.0, mag ) )
						glVertex3Point3( self._targetCentre + Vector3( 0.0, 0.0, mag ) )
						glEnd()

					glLineWidth( 1.0 )




	def _p_onTarget(self):
		self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_TARGET ) )

	def _p_onConstraints(self):
		self._view.postRedraw()



	def _p_onMeshCellValueChanged(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_RESET ) )

	def _p_onOutputMeshCellValueChanged(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshTweakComponentsManipulator.Event( MeshTweakComponentsManipulator.EV_OUTPUTCHANGED ) )

	def _p_getMesh(self):
		if self._meshCell is not None:
			return self._meshCell.getValueAsClass( GSProductMesh )
		else:
			return None







class MeshTweakComponentsFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, tweaksField, targetField, constraintsField):
		super( MeshTweakComponentsFieldManipulator, self ).__init__()
		self._tweaksField = tweaksField
		self._targetField = targetField
		self._constraintsField = constraintsField


	def createElement(self, sheetEditContext):
		return MeshTweakComponentsManipulator()

	def destroyElement(self, element, sheetEditContext):
		del sheetEditContext.sheetEditor.tweakManip

	def initialiseElement(self, element, sheetEditContext):
		tweaks = self._p_getCell( self._tweaksField, sheetEditContext )
		target = self._p_getCell( self._targetField, sheetEditContext )
		constraints = self._p_getCell( self._constraintsField, sheetEditContext )
		element.attachCells( tweaks, target, constraints )
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
	functionClass = ProcMeshTweakComponents

	def initInvoke(self):
		# Initialise with full pre-compute. This will remove the cost of the full pre-compute which would normally occur on each modification of the tweak list
		self._inputObject.preComputeFull()

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTweaks = CppMeshTweakComponentList()
		pyMeshTweakComponentList_to_CppMeshTweakComponentList( [ tweak.toCppMeshTweakComponent()   for tweak in self._proc.tweaks ], cppTweaks )
		self.mesh.tweakComponents( cppTweaks, False )


	def save(self):
		self.mesh.saveMarkedComponentPositions()

	def restore(self):
		self.mesh.restoreMarkedComponentPositions()


	def adjustTranslation(self, translation):
		self.mesh.restoreMarkedComponentPositions()
		self.mesh.transformMarkedComponents( Matrix4.translate( translation ) )


	def getResult(self):
		return self.mesh






class ToolMeshTweakComponents (GSProcedureTool):
	sheet = SheetRefField( ProcMeshTweakComponents )

	functionClass = ProcMeshTweakComponents
	adjustableInvoker = TweakComponentsAdjustableInvoker
	pageTitle = _( 'Tweak' )
	titleText = _( 'Tweak Components' )


	tweakTarget = Field( MeshTweakComponentTarget, MeshTweakComponentTarget.VERTICES )
	tweakConstraints = Field( TweakConstraints, TweakConstraints.NONE )


	tweakListLabel = FieldLabelWithFn( sheet.tweaks, _( 'Tweaks:' ) )
	tweakListEditor = TargetListFieldEdit( sheet.tweaks, targetLabelText=_( '%d tweak' ), pluralTargetLabelText=_( '%d tweaks' ), removeTargetButtonText=_( 'Remove tweak' ) )

	tweakManip = MeshTweakComponentsFieldManipulator( sheet.tweaks, tweakTarget, tweakConstraints )
	tweakManip.enable()

	targetRadio = MeshTweakComponentTargetFieldEditRadioGroup( tweakTarget )

	constraintsRadio = TweakConstraintsFieldEditRadioGroup( tweakConstraints )



	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click and drag to move. Selection is cleared on each click, unless <control> is pressed.\n'  'Use the scroll wheel to switch between component types.\n'  'Use the scroll wheel, with <shift> held to switch between constraints.' ) )
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
	layout.row()  <<  _( 'Constraints:' )
	layout.row()  <<  None  <<  constraintsRadio.none  <<  None
	layout.row()  <<  constraintsRadio.xPlane  <<  constraintsRadio.yPlane  <<  constraintsRadio.zPlane
	layout.row()  <<  constraintsRadio.xAxis  <<  constraintsRadio.yAxis  <<  constraintsRadio.zAxis
	layout.row()
	layout.finalise()



	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		super( ToolMeshTweakComponents, self ).__init__( view, editorSettings, parentWindow, commandHistory, painterFactory )
		self.tweakManip._f_onAdjustTranslation = self._onAdjustTranslation


	def shutdownTool(self):
		self.tweakManip._f_onAdjustTranslation = None
		super( ToolMeshTweakComponents, self ).shutdownTool()



	def _onAdjustTranslation(self, translation):
		self._adjustableInvoker.adjustTranslation( translation )




	def setTarget(self, target):
		self.tweakTarget = target






#
#
# OLD VERSION OF TWEAK
#
#

class _MeshTweak (object):
	def __init__(self, pick=None, bClearSelection=True, translation=None):
		super( _MeshTweak, self ).__init__()
		if pick is None:
			pick = Pick3d()
		self.pick = pick
		self.bClearSelection = bClearSelection
		if translation is None:
			translation = Vector3()
		self.translation = translation


	def toMeshTweakComponent(self, target):
		return MeshTweakComponent( self.pick, target, self.bClearSelection, self.translation )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		self.bClearSelection = ioXmlReadBoolNode( xmlNode.getChild( 'b_clear_selection' ), self.bClearSelection )
		xmlNode.getChild( 'translation' )  >>  self.translation

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		ioXmlWriteBoolNode( xmlNode.addChild( 'b_clear_selection' ), self.bClearSelection )
		xmlNode.addChild( 'translation' )  <<  self.translation


	def __copy__(self):
		return _MeshTweak( copy( self.pick ), self.bClearSelection, copy( self.translation ) )

ioObjectFactoryRegister( 'MeshTweak', _MeshTweak )







class _ProcMeshTweak (GSProcedure):
	__iotypename__ = 'ProcMeshTweak'

	description = '**DEPRACATED**'

	targetComponent = Field( MeshEditComponent, MeshEditComponent.VERTICES )
	tweaks = Field( TargetList, TargetList() )
	bRestoreSelection = Field( bool, True )

	inputClass = GSProductMesh


	# Handle old version of 'ProcMeshTweak', which did not have the @bRestoreSelection field.
	# It should default to False. Any newer version will override this value if present.
	def __readxml__(self, xmlNode):
		self.bRestoreSelection = False
		super( _ProcMeshTweak, self ).__readxml__( xmlNode )


	def __ioreplacement__(self):
		target = MeshTweakComponentTarget.VERTICES

		inTarget = self.targetComponent
		if inTarget == MeshEditComponent.VERTICES:
			target = MeshTweakComponentTarget.VERTICES
		elif inTarget == MeshEditComponent.EDGES:
			target = MeshTweakComponentTarget.EDGES
		elif inTarget == MeshEditComponent.FACES:
			target = MeshTweakComponentTarget.FACES


		proc = ProcMeshTweakComponents()
		proc.bRestoreSelection = self.bRestoreSelection
		tweaks = TargetList()
		for tweak in self.tweaks:
			tweaks.append( tweak.toMeshTweakComponent( target ) )
		proc.cells.tweaks.literalValue = tweaks
		return proc

