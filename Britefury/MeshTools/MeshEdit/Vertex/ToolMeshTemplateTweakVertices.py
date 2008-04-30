##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy, deepcopy

from Britefury.GL.GL import *
from Britefury.GL.GLU import *

from Britefury.Math.Math import Point3

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegister

from Britefury.WorkArea.Viewport import ViewportEventHandler, ViewportScrollListener
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.gSculptConfig import gSculptConfig

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Util.CoEventHandler import CoEventHandler

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.FieldEditors import *

from Britefury.ProceduralCore.GSProcedure import *

from Britefury.ProceduralTool.GSProcedureAdjustableInvoker import GSProcedureAdjustableInvoker
from Britefury.ProceduralTool.GSProcedureTool import *

from Britefury.Manipulator.Manipulator import Manipulator
from Britefury.Manipulator.FieldManipulators import *

from Britefury.Model import ModelDraw
from Britefury.Model.TargetList import TargetList
from Britefury.Model.TargetListFieldEdit import TargetListFieldEdit
from Britefury.Model.Pick3d import Pick3d

from Britefury.Mesh.Mesh import GSProductMesh, CppMeshVertexReposition, CppMeshVertexRepositionList, pyMeshVertexRepositionList_to_CppMeshVertexRepositionList





#
# MeshTemplateTweakVertex class
#

class MeshTemplateTweakVertex (object):
	def __init__(self, pick=None, position=None):
		super( MeshTemplateTweakVertex, self ).__init__()
		if pick is None:
			pick = Pick3d()
		if position is None:
			position = Point3()
		self.pick = pick
		self.position = position


	def toCppMeshVertexReposition(self):
		return CppMeshVertexReposition( self.pick.toMPick(), self.position )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'pick' )  >>  self.pick
		xmlNode.getChild( 'position' )  >>  self.position

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'pick' )  <<  self.pick
		xmlNode.addChild( 'position' )  <<  self.position


	def __copy__(self):
		return MeshTemplateTweak( copy( self.pick ), copy( self.position ) )

ioObjectFactoryRegister( 'MeshTemplateTweakVertex', MeshTemplateTweakVertex )










class ProcMeshTemplateTweakVertices (GSProcedure):
	description = _( 'Template tweak vertices' )

	tweaks = Field( TargetList, TargetList() )

	inputClass = GSProductMesh



	def procedureInvoke(self, inputObject, errorHandler):
		cppTweaks = CppMeshVertexRepositionList()
		pyMeshVertexRepositionList_to_CppMeshVertexRepositionList( [ tweak.toCppMeshVertexReposition()   for tweak in self.tweaks ], cppTweaks )
		inputObject.repositionVertices( cppTweaks )
		return inputObject







class MeshTemplateTweakVerticesManipulator (Manipulator, ViewportEventHandler, ViewportScrollListener, Viewport3dPainter):
	EV_BUTTON_DOWN = 0
	EV_BUTTON_UP = 1
	EV_MOTION = 2
	EV_LEAVE = 3
	EV_RESET = 4
	EV_OUTPUTCHANGED = 5


	class Event (object):
		def __init__(self, eventType, **kwargs):
			self.eventType = eventType
			self.__dict__.update( kwargs )


	class ResetException:
		pass



	def __init__(self):
		super( MeshTemplateTweakVerticesManipulator, self ).__init__()

		self._tweaksCell = None

		self._coEventHandler = None

		self._highlightVertexPosition = None
		self._initialVertexPosition = None

		self._f_onSetTargetVertex = None
		self._f_onAdjustPosition = None

		self._meshCell = None

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		super( MeshTemplateTweakVerticesManipulator, self ).manipAttachToView( view )
		self._coEventHandler = CoEventHandler( self._p_tweakLoop )
		self._coEventHandler.start()
		self._view.addEventHandler( self, Viewport3d )
		self._view.addPainter( self, Viewport3d )
		self._view.postRedraw()

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._view.removeEventHandler( self, Viewport3d )
		self._view.removePainter( self, Viewport3d )
		self._view.postRedraw()
		self._coEventHandler.terminate()
		self._coEventHandler = None
		super( MeshTemplateTweakVerticesManipulator, self ).manipDetachFromView()



	def attachCell(self, tweaks):
		self._tweaksCell = tweaks

	def detachCell(self):
		self._tweaksCell = None



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



	def _p_getTarget(self, viewport, targetVertexIndex):
		mesh = self._p_getMesh()
		targetPick = viewport.createPick()

		# Target vertices
		vertexIndex, thruFaceIndex = mesh.pickVertex( targetPick.toMPick() )

		if vertexIndex != targetVertexIndex:
			if vertexIndex != -1:
				# Highlight the vertex
				self._highlightVertexPosition = mesh.getVertexPosition( vertexIndex )
				self._initialVertexPosition = self._highlightVertexPosition
			else:
				self._highlightVertexPosition = None
				self._initialVertexPosition = None
			self._view.postRedraw()

		targetVertexIndex = vertexIndex

		return targetPick, targetVertexIndex



	def _p_getTweakFromUser(self):
		self._highlightVertexPosition = None
		self._initialVertexPosition = None
		targetVertexIndex = -1
		targetPick = None
		position = Point3()
		targetDistance = None
		viewport = None
		startPoint = None
		startPointer = None

		if self._view.pointerViewport is not None:
			targetPick, targetVertexIndex = self._p_getTarget( self._view.pointerViewport, targetVertexIndex )

		event = self._coEventHandler.waitForNextEvent()

		# Get the target component
		while not ( event.eventType == MeshTemplateTweakVerticesManipulator.EV_BUTTON_DOWN  and  targetVertexIndex != -1 ):
			if event.eventType == MeshTemplateTweakVerticesManipulator.EV_RESET:
				self._view.postRedraw()
				raise MeshTemplateTweakVerticesManipulator.ResetException

			if event.eventType == MeshTemplateTweakVerticesManipulator.EV_OUTPUTCHANGED:
				self._highlightVertexPosition = None
				targetVertexIndex = -1
			else:
				viewport = event.viewport

			if event.eventType != MeshTemplateTweakVerticesManipulator.EV_LEAVE  and  viewport is not None:
				targetPick, targetVertexIndex = self._p_getTarget( viewport, targetVertexIndex )

			event = self._coEventHandler.waitForNextEvent()


		# We have a target component
		# Remove the highlight
		self._highlightVertexPosition = None

		# If the selection is valid
		if self._initialVertexPosition is not None:
			targetDistance = event.viewport.camera.transformWorldToCameraZ( self._initialVertexPosition )
			startPointer = event.viewport.pointerStatus.positionF
			startPoint = event.viewport.camera.transformScreenToWorldCameraZ( startPointer, targetDistance )
			bScaledMotion = event.bScaledMotion


			# Add the tweak object to the tweak list
			self._tweaksCell.literalValue = self._tweaksCell.value + [ MeshTemplateTweakVertex( deepcopy( targetPick ), deepcopy( self._initialVertexPosition ) ) ]
			position = self._initialVertexPosition

			if self._f_onSetTargetVertex is not None  and  targetVertexIndex != -1:
				self._f_onSetTargetVertex( targetVertexIndex, self._initialVertexPosition )
		else:
			return



		event = self._coEventHandler.waitForNextEvent()

		# Now, track the motion
		while not( event.eventType == MeshTemplateTweakVerticesManipulator.EV_BUTTON_UP  and  event.bScaledMotion == bScaledMotion ):
			if event.eventType == MeshTemplateTweakVerticesManipulator.EV_RESET:
				self._view.postRedraw()
				raise MeshTemplateTweakVerticesManipulator.ResetException

			if event.eventType != MeshTemplateTweakVerticesManipulator.EV_OUTPUTCHANGED:
				endPointer = event.viewport.pointerStatus.positionF
				endPoint = event.viewport.camera.transformScreenToWorldCameraZ( endPointer, targetDistance )

				screenDelta = endPointer - startPointer
				translation = endPoint - startPoint


				if bScaledMotion:
					translation *= gSculptConfig.manipulatorFineTuneMotionScale
					screenDelta *= gSculptConfig.manipulatorFineTuneMotionScale


				vertexScreen = event.viewport.transformWorldToScreen( self._initialVertexPosition ).toPoint2()
				vertexScreen += screenDelta
				newPosition = event.viewport.computeDrawPoint( screenPoint=vertexScreen, bNoneIfNoHit=True )
				if newPosition is not None:
					position = newPosition
				else:
					position = self._initialVertexPosition + translation


				if self._f_onAdjustPosition is not None:
					self._f_onAdjustPosition( position )
				self._view.forceRedraw()

			event = self._coEventHandler.waitForNextEvent()


		assert len( self._tweaksCell.value ) > 0, 'no tweaks'
		topTweak = self._tweaksCell.value[-1]
		newTopTweak = MeshTemplateTweakVertex( topTweak.pick, position )
		tweaks = self._tweaksCell.value
		tweaks[-1] = newTopTweak
		self._tweaksCell.literalValue = tweaks
		self._p_commandHistoryBreak()



	def _p_tweakLoop(self):
		while True:
			try:
				self._p_getTweakFromUser()
			except MeshTemplateTweakVerticesManipulator.ResetException:
				self._view.postRedraw()



	def evPrimaryButtonDown(self, viewport):
		super( MeshTemplateTweakVerticesManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=False ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshTemplateTweakVerticesManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=False ) )


	def evSecondaryButtonDown(self, viewport):
		super( MeshTemplateTweakVerticesManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=True ) )

	def evSecondaryButtonUp(self, viewport):
		super( MeshTemplateTweakVerticesManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=True ) )


	def evMotion(self, viewport, point):
		super( MeshTemplateTweakVerticesManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_MOTION, viewport=viewport ) )

	def evEnter(self, viewport, point):
		super( MeshTemplateTweakVerticesManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_MOTION, viewport=viewport ) )

	def evLeave(self, viewport, point):
		super( MeshTemplateTweakVerticesManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_LEAVE, viewport=viewport ) )


	def evPaint3d(self, viewport, paintLayer):
		# Paint the target highlight
		if self._highlightVertexPosition is not None:
			ModelDraw.drawVertexPickHighlight( self._highlightVertexPosition, ModelDraw.TARGET_HIGHLIGHT_COLOUR, paintLayer )


	def _o_reset(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_RESET ) )


	def _p_onMeshCellValueChanged(self):
		self._o_reset()

	def _p_onOutputMeshCellValueChanged(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshTemplateTweakVerticesManipulator.Event( MeshTemplateTweakVerticesManipulator.EV_OUTPUTCHANGED ) )

	def _p_getMesh(self):
		if self._meshCell is not None:
			return self._meshCell.getValueAsClass( GSProductMesh )
		else:
			return None






class MeshTemplateTweakVerticesFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, tweaksField):
		super( MeshTemplateTweakVerticesFieldManipulator, self ).__init__()
		self._tweaksField = tweaksField
		self._o_addDependency( self._tweaksField )


	def createElement(self, sheetEditContext):
		return MeshTemplateTweakVerticesManipulator()

	def initialiseElement(self, element, sheetEditContext):
		element.attachCell( self._p_getCell( self._tweaksField, sheetEditContext ) )
		if sheetEditContext.commandHistory is not None:
			element.commandHistory = sheetEditContext.commandHistory

	def shutdownElement(self, element, sheetEditContext):
		element.detachCell()
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




class TemplateTweakVerticesAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshTemplateTweakVertices


	def __init__(self, function, inputObject, errorHandler):
		super( TemplateTweakVerticesAdjustableInvoker, self ).__init__( function, inputObject, errorHandler )
		self._vertexIndex = None
		self._initialVertexPosition = None



	def initInvoke(self):
		# Initialise with full pre-compute. This will remove the cost of the full pre-compute which would normally occur on each modification of the tweak list
		self._inputObject.preComputeFull()

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		cppTweaks = CppMeshVertexRepositionList()
		pyMeshVertexRepositionList_to_CppMeshVertexRepositionList( [ tweak.toCppMeshVertexReposition()   for tweak in self._proc.tweaks ], cppTweaks )
		self.mesh.repositionVertices( cppTweaks )



	def setTargetVertex(self, vertexIndex, initialPosition):
		self._vertexIndex = vertexIndex
		self._initialVertexPosition = initialPosition


	def adjustPosition(self, position):
		if self._vertexIndex is not None:
			self.mesh.setVertexPosition( self._vertexIndex, position )


	def getResult(self):
		return self.mesh






class ToolMeshTemplateTweakVertices (GSProcedureTool):
	sheet = SheetRefField( ProcMeshTemplateTweakVertices )

	functionClass = ProcMeshTemplateTweakVertices
	adjustableInvoker = TemplateTweakVerticesAdjustableInvoker
	pageTitle = _( 'TmpTwk' )
	titleText = _( 'Tempate Tweak' )


	tweakListLabel = FieldLabelWithFn( sheet.tweaks, _( 'Tweak targets:' ) )
	tweakListEditor = TargetListFieldEdit( sheet.tweaks, targetLabelText=_( '%d tweak' ), pluralTargetLabelText=_( '%d tweaks' ), removeTargetButtonText=_( 'Remove tweak' ) )

	tweakManip = MeshTemplateTweakVerticesFieldManipulator( sheet.tweaks )
	tweakManip.enable()


	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click and drag to move.\n'  'Vertices will be constrained to the surfaces of any drawing templates that are enabled.' ) )
	layout.row()
	layout.row()  <<  tweakListLabel.label
	layout.row()  <<  tweakListEditor.numTargets
	layout.row()  <<  None  <<  ( tweakListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		super( ToolMeshTemplateTweakVertices, self ).__init__( view, editorSettings, parentWindow, commandHistory, painterFactory )
		self.tweakManip._f_onSetTargetVertex = self._onSetTargetVertex
		self.tweakManip._f_onAdjustPosition = self._onAdjustPosition


	def shutdownTool(self):
		self.tweakManip._f_onSetTargetVertex = None
		self.tweakManip._f_onAdjustPosition = None
		super( ToolMeshTemplateTweakVertices, self ).shutdownTool()



	def _onSetTargetVertex(self, vertexIndex, initialPosition):
		self._adjustableInvoker.setTargetVertex( vertexIndex, initialPosition )

	def _onAdjustPosition(self, position):
		self._adjustableInvoker.adjustPosition( position )




