##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy, deepcopy

import math

from Britefury.GL.GL import *
from Britefury.GL.GLU import *

from Britefury.Math.Math import Point3

from Britefury.FileIO.IOXml import IOXmlEnum, ioObjectFactoryRegister

from Britefury.Brush.Brush import CubicSCurveBrush, FlatBrush, GaussianBrush, LinearBrush, SphericalBrush

from Britefury.WorkArea.Viewport import ViewportEventHandler, ViewportScrollListener
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Britefury.View.View import PaintLayer

from Britefury.gSculptConfig import gSculptConfig

from Britefury.UI.FormLayout import FormLayoutFactory

from Britefury.Util.CoEventHandler import CoEventHandler

from Britefury.CellEdit.ChoiceCellEditRadioGroup import *
from Britefury.CellEdit.ChoiceCellEditToggleGroup import *

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

from Britefury.Mesh.Mesh import GSProductMesh, MeshProportionalAdjuster




BRUSH_GAUSSIAN_MINIMUM = 1.0 / 255.0

RADIUS_SCROLL_SCALE_FACTOR = 1.25
RADIUS_SCROLL_FINE_SCALE_FACTOR = RADIUS_SCROLL_SCALE_FACTOR ** ( 1.0 / 15.0 )

PROPORTIONAL_SCALE_RATE = 1.0 / 200.0
PROPORTIONAL_ROTATION_RATE = math.radians( 0.5 )
PROPORTIONAL_RELAX_RATE = 1.0 / 100.0





class TweakSphereQuadric (object):
	s_globalQuadric = None

	def __init__(self):
		self._sphere = GLUquadric()
		gluQuadricDrawStyle( self._sphere, GLU_FILL )
		gluQuadricNormals( self._sphere, GLU_SMOOTH )


	@classmethod
	def getSphere(cls):
		if cls.s_globalQuadric is None  or  cls.s_globalQuadric() is None:
			sphere = TweakSphereQuadric()
			cls.s_globalQuadric = weakref.ref( sphere )
			return sphere._sphere
		else:
			sphere = cls.s_globalQuadric()
			return sphere._sphere




class ProportionalTweakOperationClass (type):
	pass


#
# MeshProportionalTweak
#

class MeshProportionalTweak (object):
	__metaclass__ = ProportionalTweakOperationClass

	def __init__(self, brushPosition=None, brush=None):
		super( MeshProportionalTweak, self ).__init__()
		if brushPosition is None:
			brushPosition = Point3()
		if brush is None:
			brush = GaussianBrush( 0.0, BRUSH_GAUSSIAN_MINIMUM )
		self.brushPosition = brushPosition
		self.brush = brush


	def initProportionalAdjuster(self, mesh, adjuster):
		mesh.proportionalAdjusterInit( self.brushPosition, self.brush, adjuster )


	def __readxml__(self, xmlNode):
		xmlNode.getChild( 'brush_position' )  >>  self.brushPosition
		self.brush = xmlNode.getChild( 'brush' ).readObject()

	def __writexml__(self, xmlNode):
		xmlNode.addChild( 'brush_position' )  <<  self.brushPosition
		xmlNode.addChild( 'brush' ).writeObject( self.brush )


	def __copy__(self):
		return MeshProportionalTweak( copy( self.brushPosition ), copy( self.brush ) )


ioObjectFactoryRegister( 'MeshProportionalTweak', MeshProportionalTweak )





#
# MeshProportionalMove
#

class MeshProportionalMove (MeshProportionalTweak):
	def __init__(self, brushPosition=None, brush=None, translation=None):
		super( MeshProportionalMove, self ).__init__( brushPosition, brush )
		if translation is None:
			translation = Vector3()
		self.translation = translation


	def proportionalTweak(self, mesh, adjuster=None):
		mesh.proportionalMove( self.brushPosition, self.brush, self.translation, adjuster )

	def proportionalAdjust(self, adjuster):
		adjuster.move( self.translation )

	def computeTweakParameters(self, viewport, focalDistance, startPointerPosition, endPointerPosition, bScaledMotion):
		start = viewport.camera.transformScreenToWorldCameraZ( startPointerPosition, focalDistance )
		end = viewport.camera.transformScreenToWorldCameraZ( endPointerPosition, focalDistance )
		self.translation = end - start
		if bScaledMotion:
			self.translation *= gSculptConfig.manipulatorFineTuneMotionScale


	def __readxml__(self, xmlNode):
		super( MeshProportionalMove, self ).__readxml__( xmlNode)
		xmlNode.getChild( 'translation' )  >>  self.translation

	def __writexml__(self, xmlNode):
		super( MeshProportionalMove, self ).__writexml__( xmlNode )
		xmlNode.addChild( 'translation' )  <<  self.translation


	def __copy__(self):
		return MeshProportionalMove( copy( self.brushPosition ), copy( self.brush ), copy( self.translation ) )


ioObjectFactoryRegister( 'MeshProportionalMove', MeshProportionalMove )





#
# MeshProportionalMoveNormal
#

class MeshProportionalMoveNormal (MeshProportionalTweak):
	def __init__(self, brushPosition=None, brush=None, distance=0.0):
		super( MeshProportionalMoveNormal, self ).__init__( brushPosition, brush )
		self.distance = distance


	def proportionalTweak(self, mesh, adjuster=None):
		mesh.proportionalMoveNormal( self.brushPosition, self.brush, self.distance, adjuster )

	def proportionalAdjust(self, adjuster):
		adjuster.moveNormal( self.distance )

	def computeTweakParameters(self, viewport, focalDistance, startPointerPosition, endPointerPosition, bScaledMotion):
		self.distance = viewport.camera.transformDistanceAcrossScreenToDistanceThruWorld( endPointerPosition.x - startPointerPosition.x, focalDistance )
		if bScaledMotion:
			self.distance *= gSculptConfig.manipulatorFineTuneMotionScale


	def __readxml__(self, xmlNode):
		super( MeshProportionalMoveNormal, self ).__readxml__( xmlNode)
		self.distance = ioXmlReadFloatNode( xmlNode.getChild( 'distance' ), self.distance )

	def __writexml__(self, xmlNode):
		super( MeshProportionalMoveNormal, self ).__writexml__( xmlNode )
		ioXmlWriteFloatNode( xmlNode.addChild( 'distance' ), self.distance )


	def __copy__(self):
		return MeshProportionalMoveNormal( copy( self.brushPosition ), copy( self.brush ), self.distance )


ioObjectFactoryRegister( 'MeshProportionalMoveNormal', MeshProportionalMoveNormal )





#
# MeshProportionalScale
#

class MeshProportionalScale (MeshProportionalTweak):
	def __init__(self, brushPosition=None, brush=None, scaleFactor=1.0):
		super( MeshProportionalScale, self ).__init__( brushPosition, brush )
		self.scaleFactor = scaleFactor


	def proportionalTweak(self, mesh, adjuster=None):
		mesh.proportionalScale( self.brushPosition, self.brush, self.scaleFactor, adjuster )

	def proportionalAdjust(self, adjuster):
		adjuster.scale( self.brushPosition, self.scaleFactor )

	def computeTweakParameters(self, viewport, focalDistance, startPointerPosition, endPointerPosition, bScaledMotion):
		scaleDelta = ( endPointerPosition.x - startPointerPosition.x ) * PROPORTIONAL_SCALE_RATE
		if bScaledMotion:
			scaleDelta *= gSculptConfig.manipulatorFineTuneMotionScale
		self.scaleFactor = 1.0 + scaleDelta


	def __readxml__(self, xmlNode):
		super( MeshProportionalScale, self ).__readxml__( xmlNode)
		self.scaleFactor = ioXmlReadFloatNode( xmlNode.getChild( 'scale_factor' ), self.scaleFactor )

	def __writexml__(self, xmlNode):
		super( MeshProportionalScale, self ).__writexml__( xmlNode )
		ioXmlWriteFloatNode( xmlNode.addChild( 'scale_factor' ), self.scaleFactor )


	def __copy__(self):
		return MeshProportionalScale( copy( self.brushPosition ), copy( self.brush ), self.scaleFactor )


ioObjectFactoryRegister( 'MeshProportionalScale', MeshProportionalScale )





#
# MeshProportionalRotate
#

class MeshProportionalRotate (MeshProportionalTweak):
	def __init__(self, brushPosition=None, brush=None, rotationAxis=None, rotation=0.0):
		super( MeshProportionalRotate, self ).__init__( brushPosition, brush )
		if rotationAxis is None:
			rotationAxis = Vector3( 0.0, 0.0, 1.0 )
		self.rotationAxis = rotationAxis
		self.rotation = rotation


	def proportionalTweak(self, mesh, adjuster=None):
		mesh.proportionalRotate( self.brushPosition, self.brush, self.rotationAxis, self.rotation, adjuster )

	def proportionalAdjust(self, adjuster):
		adjuster.rotate( self.brushPosition, self.rotationAxis, self.rotation )

	def computeTweakParameters(self, viewport, focalDistance, startPointerPosition, endPointerPosition, bScaledMotion):
		self.rotationAxis = copy( viewport.camera.cameraPosition.forwardAxis )
		self.rotation = ( endPointerPosition.x - startPointerPosition.x ) * PROPORTIONAL_ROTATION_RATE
		if bScaledMotion:
			self.rotation *= gSculptConfig.manipulatorFineTuneMotionScale


	def __readxml__(self, xmlNode):
		super( MeshProportionalRotate, self ).__readxml__( xmlNode)
		xmlNode.getChild( 'rotation_axis' )  >>  self.rotationAxis
		self.rotation = ioXmlReadFloatNode( xmlNode.getChild( 'rotation' ), self.rotation )

	def __writexml__(self, xmlNode):
		super( MeshProportionalRotate, self ).__writexml__( xmlNode )
		xmlNode.addChild( 'rotation_axis' )  <<  self.rotationAxis
		ioXmlWriteFloatNode( xmlNode.addChild( 'rotation' ), self.rotation )


	def __copy__(self):
		return MeshProportionalRotate( copy( self.brushPosition ), copy( self.brush ), copy( self.rotationAxis ), self.rotation )


ioObjectFactoryRegister( 'MeshProportionalRotate', MeshProportionalRotate )




#
# MeshProportionalRelax
#

class MeshProportionalRelax (MeshProportionalTweak):
	def __init__(self, brushPosition=None, brush=None, relaxAmount=0.0):
		super( MeshProportionalRelax, self ).__init__( brushPosition, brush )
		self.relaxAmount = relaxAmount


	def proportionalTweak(self, mesh, adjuster=None):
		mesh.proportionalRelax( self.brushPosition, self.brush, self.relaxAmount, adjuster )

	def proportionalAdjust(self, adjuster):
		adjuster.relax( self.relaxAmount )

	def computeTweakParameters(self, viewport, focalDistance, startPointerPosition, endPointerPosition, bScaledMotion):
		self.relaxAmount = ( endPointerPosition.x - startPointerPosition.x ) * PROPORTIONAL_RELAX_RATE
		if bScaledMotion:
			self.relaxAmount *= gSculptConfig.manipulatorFineTuneMotionScale
		self.relaxAmount = max( self.relaxAmount, 0.0 )


	def __readxml__(self, xmlNode):
		super( MeshProportionalRelax, self ).__readxml__( xmlNode)
		self.relaxAmount = ioXmlReadFloatNode( xmlNode.getChild( 'relax_amount' ), self.relaxAmount )

	def __writexml__(self, xmlNode):
		super( MeshProportionalRelax, self ).__writexml__( xmlNode )
		ioXmlWriteFloatNode( xmlNode.addChild( 'relax_amount' ), self.relaxAmount )


	def __copy__(self):
		return MeshProportionalRelax( copy( self.brushPosition ), copy( self.brush ), self.relaxAmount )


ioObjectFactoryRegister( 'MeshProportionalRelax', MeshProportionalRelax )








_proportionalTweakOperationClasses = [ MeshProportionalMove, MeshProportionalMoveNormal, MeshProportionalScale, MeshProportionalRotate, MeshProportionalRelax ]


# Cell class for ProportionalTweakOperationClass
class ProportionalTweakOperationClassCell (RefCell):
	valueClass = ProportionalTweakOperationClass
	bAllowNone = False

registerCellType( ProportionalTweakOperationClass, ProportionalTweakOperationClassCell )



# Cell editor for ProportionalTweakOperationClass
class ProportionalTweakOperationClassCellEditRadioGroup (ChoiceCellEditRadioGroup):
	def __init__(self):
		super( ProportionalTweakOperationClassCellEditRadioGroup, self ).__init__()
		self.move = self.addChoice( MeshProportionalMove, _( 'Move' ) )
		self.moveNormal = self.addChoice( MeshProportionalMoveNormal, _( 'MoveNml' ) )
		self.scale = self.addChoice( MeshProportionalScale, _( 'Scale' ) )
		self.rotate = self.addChoice( MeshProportionalRotate, _( 'Rotate' ) )
		self.relax = self.addChoice( MeshProportionalRelax, _( 'Relax' ) )



# Field editor for ProportionalTweakOperationClass
class ProportionalTweakOperationClassFieldEditRadioGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'move', 'moveNormal', 'scale', 'rotate', 'relax' ]


	def createElement(self, rowEditContext):
		return ProportionalTweakOperationClassCellEditRadioGroup()









def _cubicSCurveBrushFactory(radius):
	return CubicSCurveBrush( radius )

def _flatBrushFactory(radius):
	return FlatBrush( radius )

def _gaussianBrushFactory(radius):
	return GaussianBrush( radius, BRUSH_GAUSSIAN_MINIMUM )

def _linearBrushFactory(radius):
	return LinearBrush( radius )

def _sphericalBrushFactory(radius):
	return SphericalBrush( radius )



class BrushFactory (object):
	def __init__(self, factoryFunction):
		self._factory = factoryFunction

	def makeBrush(self, radius):
		return self._factory( radius )


cubicSCurveBrushFactory = BrushFactory( _cubicSCurveBrushFactory )
flatBrushFactory = BrushFactory( _flatBrushFactory )
gaussianBrushFactory = BrushFactory( _gaussianBrushFactory )
linearBrushFactory = BrushFactory( _linearBrushFactory )
sphericalBrushFactory = BrushFactory( _sphericalBrushFactory )





# Cell class for ProportionalTweakBrushClass
class ProportionalTweakBrushFactoryCell (RefCell):
	valueClass = BrushFactory
	bAllowNone = False

registerCellType( BrushFactory, ProportionalTweakBrushFactoryCell )



# Cell editor for ProportionalTweakBrushFactory
class ProportionalTweakBrushFactoryCellEditToggleGroup (ChoiceCellEditToggleGroup):
	def __init__(self):
		super( ProportionalTweakBrushFactoryCellEditToggleGroup, self ).__init__()
		self.cubicSCurve = self.addChoice( cubicSCurveBrushFactory, gsImageFilename='falloff_cubicscurve.png' )
		self.flat = self.addChoice( flatBrushFactory, gsImageFilename='falloff_flat.png' )
		self.gaussian = self.addChoice( gaussianBrushFactory, gsImageFilename='falloff_gaussian.png' )
		self.linear = self.addChoice( linearBrushFactory, gsImageFilename='falloff_linear.png' )
		self.spherical = self.addChoice( sphericalBrushFactory, gsImageFilename='falloff_spherical.png' )



# Field editor for ProportionalTweakBrushFactory
class ProportionalTweakBrushFactoryFieldEditToggleGroup (BasicSingleFieldWidgetEditor):
	__componentnames__ = [ 'cubicSCurve', 'flat', 'gaussian', 'linear', 'spherical' ]


	def createElement(self, rowEditContext):
		return ProportionalTweakBrushFactoryCellEditToggleGroup()











class ProcMeshProportionalTweak (GSProcedure):
	description = _( 'Proportional tweak' )

	tweaks = Field( TargetList, TargetList() )

	inputClass = GSProductMesh



	def procedureInvoke(self, inputObject, errorHandler):
		adjuster = MeshProportionalAdjuster()
		for tweak in self.tweaks:
			tweak.proportionalTweak( inputObject, adjuster )
		return inputObject







class MeshProportionalTweakManipulator (Manipulator, ViewportEventHandler, ViewportScrollListener, Viewport3dPainter):
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
		super( MeshProportionalTweakManipulator, self ).__init__()

		self._tweaksCell = None
		self._operationCell = None
		self._brushFactoryCell = None
		self._brushRadiusCell = None

		self._coEventHandler = None

		self._brushPosition = None

		self._f_initAdjustableTweak = None
		self._f_adjustTweak = None
		self._f_finishAdjustableTweak = None

		self._meshCell = None

		self._o_setUISensitivity( False )




	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		super( MeshProportionalTweakManipulator, self ).manipAttachToView( view )
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
		super( MeshProportionalTweakManipulator, self ).manipDetachFromView()



	def attachCells(self, tweaks, operation, brushFactory, brushRadius):
		self._tweaksCell = tweaks
		self._operationCell = operation
		self._brushFactoryCell = brushFactory
		self._brushRadiusCell = brushRadius

	def detachCells(self):
		self._tweaksCell = None
		self._operationCell = None
		self._brushFactoryCell = None
		self._brushRadiusCell = None



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




	def _p_getBrushPosition(self, viewport):
		mesh = self._p_getMesh()
		pick = viewport.createPick()


		# Try hitting a face, not outside the bounds of the mesh
		pick.bAllowPickOutsideBounds = False
		faceIndex, intersectionPoint = mesh.pickFace( pick.toMPick() )

		if faceIndex != -1:
			focalDist = viewport.camera.transformWorldToCameraZ( intersectionPoint )
			return intersectionPoint, focalDist
		else:
			# Could not get a face outside of the bounds of the mesh
			pick.bAllowPickOutsideBounds = True
			edgeIndex, focalPoint, thruFaceIndex = mesh.pickEdge( pick.toMPick() )
			if edgeIndex != -1:
				focalDist = viewport.camera.transformWorldToCameraZ( focalPoint )
				return viewport.camera.transformScreenToWorldCameraZ( viewport.pointerStatus.positionF, focalDist ), focalDist
			else:
				return None, None



	def _p_getTweakFromUser(self):
		self._brushPosition = None
		focalDist = None

		if self._view.pointerViewport is not None:
			self._brushPosition, focalDist = self._p_getBrushPosition( self._view.pointerViewport )

		event = self._coEventHandler.waitForNextEvent()

		# Get the brush position
		while not ( event.eventType == self.EV_BUTTON_DOWN  and  self._brushPosition is not None ):
			if event.eventType == self.EV_RESET:
				self._view.postRedraw()
				raise self.ResetException

			if event.eventType == self.EV_OUTPUTCHANGED:
				self._brushPosition = None
				viewport = None
			else:
				viewport = event.viewport

			if event.eventType == self.EV_LEAVE:
				self._brushPosition = None
				self._view.postRedraw()

			if event.eventType != self.EV_LEAVE  and  viewport is not None:
				brushPosition, focalDist = self._p_getBrushPosition( viewport )
				if brushPosition != self._brushPosition:
					self._view.forceRedraw()
				self._brushPosition = brushPosition

			event = self._coEventHandler.waitForNextEvent()


		# Record the initial pointer position, and the scaled motion flag
		startPointerPosition = copy( viewport.pointerStatus.positionF )
		bScaledMotion = event.bScaledMotion


		# Add the tweak to the tweak list
		operationClass = self._operationCell.getValue()
		brushRadius = self._brushRadiusCell.getValue()
		brush = self._brushFactoryCell.getValue().makeBrush( brushRadius )
		tweak = operationClass( copy( self._brushPosition ), brush )
		#self._tweaksCell.literalValue = self._tweaksCell.getValue()  +  [ tweak ]


		if self._f_initAdjustableTweak is not None:
			self._f_initAdjustableTweak( tweak )



		self._brushPosition = None


		# Now, track the motion
		event = self._coEventHandler.waitForNextEvent()

		while not( event.eventType == self.EV_BUTTON_UP  and  event.bScaledMotion == bScaledMotion ):
			if event.eventType == self.EV_RESET:
				self._view.postRedraw()
				raise self.ResetException

			if event.eventType != self.EV_OUTPUTCHANGED:
				endPointerPosition = event.viewport.pointerStatus.positionF

				tweak.computeTweakParameters( viewport, focalDist, startPointerPosition, endPointerPosition, bScaledMotion )

				if self._f_adjustTweak is not None:
					self._f_adjustTweak( tweak )

				self._view.forceRedraw()

			event = self._coEventHandler.waitForNextEvent()


		if self._f_finishAdjustableTweak is not None:
			self._f_finishAdjustableTweak()
		# Add the final tweak to the tweak list
		#assert len( self._tweaksCell.value ) > 0, 'no tweaks'
		#tweaks = self._tweaksCell.getValue()
		#tweaks[-1] = tweak
		#self._tweaksCell.literalValue = tweaks
		self._tweaksCell.literalValue = self._tweaksCell.getValue()  +  [ tweak ]
		self._p_commandHistoryBreak()



	def _p_tweakLoop(self):
		while True:
			try:
				self._brushPosition = None
				self._p_getTweakFromUser()
			except MeshProportionalTweakManipulator.ResetException:
				self._brushPosition = None
				self._view.postRedraw()



	def evPrimaryButtonDown(self, viewport):
		super( MeshProportionalTweakManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=False ) )

	def evPrimaryButtonUp(self, viewport):
		super( MeshProportionalTweakManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=False ) )


	def evSecondaryButtonDown(self, viewport):
		super( MeshProportionalTweakManipulator, self ).evPrimaryButtonDown( viewport )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_BUTTON_DOWN, viewport=viewport, bScaledMotion=True ) )

	def evSecondaryButtonUp(self, viewport):
		super( MeshProportionalTweakManipulator, self ).evPrimaryButtonUp( viewport )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_BUTTON_UP, viewport=viewport, bScaledMotion=True ) )


	def evMotion(self, viewport, point):
		super( MeshProportionalTweakManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_MOTION, viewport=viewport ) )

	def evEnter(self, viewport, point):
		super( MeshProportionalTweakManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_MOTION, viewport=viewport ) )

	def evLeave(self, viewport, point):
		super( MeshProportionalTweakManipulator, self ).evMotion( viewport, point )
		self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_LEAVE, viewport=viewport ) )


	def evPaint3d(self, viewport, paintLayer):
		# Paint sphere
		if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH:
			if self._brushPosition is not None:
				brushRadius = self._brushRadiusCell.getValue()

				glColor4f( 0.06, 0.1, 0.3, 0.7 )
				glCullFace( GL_BACK )
				glEnable( GL_CULL_FACE )

				glPushMatrix()
				glTranslated( self._brushPosition.x, self._brushPosition.y, self._brushPosition.z )
				glScaled( brushRadius, brushRadius, brushRadius )

				gluSphere( TweakSphereQuadric.getSphere(), 1.0, 36, 18 )

				glPopMatrix()

				glDisable( GL_CULL_FACE )



	def evScroll(self, viewport, x, y):
		if viewport.pointerStatus.bControlKey:
			v = self._operationCell.value
			n = _proportionalTweakOperationClasses.index( v )
			n -= y
			while n < 0:
				n += len( _proportionalTweakOperationClasses )
			while n >= len( _proportionalTweakOperationClasses ):
				n -= len( _proportionalTweakOperationClasses )
			self._operationCell.literalValue = _proportionalTweakOperationClasses[n]
		else:
			v = self._brushRadiusCell.getValue()
			if v == 0.0:
				if y > 0:
					self._brushRadiusCell.literalValue = 1.0
			else:
				if viewport.pointerStatus.bShiftKey:
					radiusScaleFactor = RADIUS_SCROLL_FINE_SCALE_FACTOR ** y
				else:
					radiusScaleFactor = RADIUS_SCROLL_SCALE_FACTOR ** y
				self._brushRadiusCell.literalValue = v * radiusScaleFactor
			self._view.postRedraw()



	def _o_reset(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( self.Event( self.EVENT_RESET ) )


	def _p_onMeshCellValueChanged(self):
		self._o_reset()

	def _p_onOutputMeshCellValueChanged(self):
		if self._coEventHandler is not None:
			self._coEventHandler.sendEvent( MeshProportionalTweakManipulator.Event( MeshProportionalTweakManipulator.EV_OUTPUTCHANGED ) )

	def _p_getMesh(self):
		if self._meshCell is not None:
			return self._meshCell.getValueAsClass( GSProductMesh )
		else:
			return None






class MeshProportionalTweakFieldManipulator (GSProcedureToolFieldManipulator):
	def __init__(self, tweaksField, operationField, brushFactoryField, brushRadiusField):
		super( MeshProportionalTweakFieldManipulator, self ).__init__()
		self._tweaksField = tweaksField
		self._operationField = operationField
		self._brushFactoryField = brushFactoryField
		self._brushRadiusField = brushRadiusField
		self._o_addDependency( self._tweaksField )
		self._o_addDependency( self._operationField )
		self._o_addDependency( self._brushFactoryField )
		self._o_addDependency( self._brushRadiusField )


	def createElement(self, sheetEditContext):
		return MeshProportionalTweakManipulator()

	def initialiseElement(self, element, sheetEditContext):
		tweaks = self._p_getCell( self._tweaksField, sheetEditContext )
		operation = self._p_getCell( self._operationField, sheetEditContext )
		brushFactory = self._p_getCell( self._brushFactoryField, sheetEditContext )
		brushRadius = self._p_getCell( self._brushRadiusField, sheetEditContext )
		element.attachCells( tweaks, operation, brushFactory, brushRadius )
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




class ProportionalTweakAdjustableInvoker (GSProcedureAdjustableInvoker):
	functionClass = ProcMeshProportionalTweak


	def initInvoke(self):
		# Initialise with full pre-compute. This will remove the cost of the full pre-compute which would normally occur on each modification of the tweak list
		self._inputObject.preComputeFull()
		self.tweak = None

	def main(self):
		self.mesh = deepcopy( self._inputObject )
		adjuster = MeshProportionalAdjuster()
		for tweak in self._proc.tweaks:
			tweak.proportionalTweak( self.mesh, adjuster )

		if self.tweak is not None:
			self.adjuster = MeshProportionalAdjuster()
			tweak.initProportionalAdjuster( self.mesh, self.adjuster )
			self.adjuster.save()


	def initAdjustableTweak(self, tweak):
		self.tweak = tweak
		self.adjuster = MeshProportionalAdjuster()
		tweak.initProportionalAdjuster( self.mesh, self.adjuster )
		self.adjuster.save()


	def adjustTweak(self, tweak):
		if self.tweak is not None:
			self.adjuster.restore()
			tweak.proportionalAdjust( self.adjuster )


	def finishAdjustableTweak(self):
		self.adjuster = None
		self.tweak = None


	def getResult(self):
		return self.mesh





class ToolMeshProportionalTweak (GSProcedureTool):
	sheet = SheetRefField( ProcMeshProportionalTweak )

	functionClass = ProcMeshProportionalTweak
	adjustableInvoker = ProportionalTweakAdjustableInvoker
	pageTitle = _( 'PropTwk' )
	titleText = _( 'Proportional Tweak' )


	operation = Field( ProportionalTweakOperationClass, MeshProportionalMove )
	falloff = Field( BrushFactory, gaussianBrushFactory )
	brushRadius = Field( float, 10.0 )


	operationRadio = ProportionalTweakOperationClassFieldEditRadioGroup( operation )
	falloffToggles = ProportionalTweakBrushFactoryFieldEditToggleGroup( falloff )
	brushRadiusEntry = FloatFieldEditEntry( brushRadius, 0.0, 1.0e18, 0.1, 10.0, 4 )


	tweakListLabel = FieldLabelWithFn( sheet.tweaks, _( 'Tweaks:' ) )
	tweakListEditor = TargetListFieldEdit( sheet.tweaks, targetLabelText=_( '%d tweak' ), pluralTargetLabelText=_( '%d tweaks' ), removeTargetButtonText=_( 'Remove tweak' ) )


	tweakManip = MeshProportionalTweakFieldManipulator( sheet.tweaks, operation, falloff, brushRadius )
	tweakManip.enable()



	layout = FormLayoutFactory()
	layout.textBlock( _( 'Click and drag to tweak.\n'  'Use the scroll wheel to alter the brush radius.\n'  'Use <shift>-scroll to alter the brush radius in smaller increments.\n'  'Use <control>-scroll to change the operation.\n' ) )
	layout.row()
	layout.row()  <<  _( 'Mode:' )
	layout.row()  <<  operationRadio.move  <<  operationRadio.moveNormal
	layout.row()  <<  operationRadio.scale  <<  operationRadio.rotate
	layout.row()  <<  operationRadio.relax  <<  None
	layout.row()
	layout.row()  <<  _( 'Brush falloff:' )
	layout.section( xAttachOptions=0 )
	layout.row()  <<  falloffToggles.gaussian  <<  falloffToggles.spherical  <<  falloffToggles.cubicSCurve  <<  falloffToggles.linear  <<  falloffToggles.flat
	layout.section()
	layout.row()
	layout.row()  <<  _( 'Brush radius:' )  <<  brushRadiusEntry.entry
	layout.row()
	layout.row()  <<  tweakListLabel.label
	layout.row()  <<  tweakListEditor.numTargets
	layout.row()  <<  None  <<  ( tweakListEditor.removeTarget, 2 )  <<  None
	layout.row()
	layout.finalise()



	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		super( ToolMeshProportionalTweak, self ).__init__( view, editorSettings, parentWindow, commandHistory, painterFactory )
		self.tweakManip._f_initAdjustableTweak = self._initAdjustableTweak
		self.tweakManip._f_adjustTweak = self._adjustTweak
		self.tweakManip._f_finishAdjustableTweak = self._finishAdjustableTweak
		if self._storeOperation is not None:
			self.operation = self._storeOperation
		if self._storeFalloff is not None:
			self.falloff = self._storeFalloff
		if self._storeBrushRadius is not None:
			self.brushRadius = self._storeBrushRadius


	def shutdownTool(self):
		self.tweakManip._f_initAdjustableTweak = None
		self.tweakManip._f_adjustTweak = None
		self.tweakManip._f_finishAdjustableTweak = None
		ToolMeshProportionalTweak._storeOperation = self.operation
		ToolMeshProportionalTweak._storeFalloff = self.falloff
		ToolMeshProportionalTweak._storeBrushRadius = self.brushRadius
		super( ToolMeshProportionalTweak, self ).shutdownTool()



	def _initAdjustableTweak(self, tweak):
		self._adjustableInvoker.initAdjustableTweak( tweak )

	def _adjustTweak(self, tweak):
		self._adjustableInvoker.adjustTweak( tweak )

	def _finishAdjustableTweak(self):
		self._adjustableInvoker.finishAdjustableTweak()




	_storeOperation = None
	_storeFalloff = None
	_storeBrushRadius = None