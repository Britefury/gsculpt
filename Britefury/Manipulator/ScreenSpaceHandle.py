##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from Britefury.GL.GL import *

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math import Math
from Britefury.Math.Math import Colour3f, Vector2, Point2, Triangle2, Segment2, Matrix4

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import glColor4Colour3fA, glVertex2Point2

from Britefury.Manipulator.HandleScreen import *



XFORMAXISHANDLE_ALPHA = 0.5
HANDLE_UNHIGHLIGHTEDBRIGHTNESS = 0.5
XFORMAXISHANDLE_SIZE = 0.1





class ScreenSpaceHandleAxisHandleComponent (SingleHandleComponentScreen):
	def __init__(self, colour, shaft = Vector2( 1.0, 0.0 ), u = Vector2( 0.0, 1.0 )):
		super( ScreenSpaceHandleAxisHandleComponent, self ).__init__( colour )
		self.shaftAxis = shaft
		self.uAxis = u
		self._initalPoint = None


	def isWithinBounds(self, boundsPolygon):
		return boundsPolygon.contains( Point2() )  and  boundsPolygon.contains( Point2( self.shaftAxis ) )


	def _p_paintScreen(self, viewport, brightness):
		glBegin( GL_TRIANGLES )

		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex2Point2( Point2( ( self.uAxis + self.shaftAxis )  *  0.1 ) )
		glColor4Colour3fA( self._colour * brightness, XFORMAXISHANDLE_ALPHA )
		glVertex2Point2( Point2() )
		glVertex2Point2( Point2( self.shaftAxis ) )

		glEnd()


	def _p_pick(self, pointerPosition, localPointerPosition):
		return Triangle2( Point2( ( self.uAxis + self.shaftAxis ) * 0.1 ), Point2( self.shaftAxis ), Point2() ).contains( localPointerPosition )



	def _p_dragStartScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		self._initialPoint = copy( pointerPosition )
		return True

	def _p_dragMotionScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		return ( pointerPosition - self._initialPoint ).projectOntoUnitVector( self.shaftAxis )  *  motionScale,  self.shaftAxis

	def _p_dragFinishScreen(self, ppointerStatus, ointerPosition, localPointerPosition, motionScale):
		self._initalPoint = None





class ScreenSpaceHandleFreeHandleComponent (SingleHandleComponentScreen):
	def __init__(self):
		super( ScreenSpaceHandleFreeHandleComponent, self ).__init__( Colour3f() )
		self._initialPoint = None


	def isWithinBounds(self, boundsPolygon):
		return True


	def _p_paintScreen(self, viewport, brightness):
		pass


	def _p_pick(self, pointerPosition, localPointerPosition):
		return False



	def _p_dragStartScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		self._initialPoint = copy( pointerPosition )
		return True

	def _p_dragMotionScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		return ( pointerPosition - self._initialPoint )  *  motionScale,  None

	def _p_dragFinishScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		self._initalPoint = None





class ScreenSpace1Handle (HandleScreen):
	class ScreenSpaceHandle1Components (CompositeHandleComponentScreen):
		def __init__(self):
			self.freeHandle = ScreenSpaceHandleFreeHandleComponent()
			self._handles = [ self.freeHandle ]


		def isWithinBounds(self, boundsPolygon):
			return boundsPolygon.contains( Point2( -0.25, -0.25 ) )   and   boundsPolygon.contains( Point2( 1.25, 0.0 ) )   and  boundsPolygon.contains( Point2( 0.0, 1.25 ) )



	startSignal = ClassSignal()
	dragSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		b = ScreenSpace1Handle.ScreenSpaceHandle1Components()
		super( ScreenSpace1Handle, self ).__init__( b, b.freeHandle )



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, deltaAndConstraints):
		delta, constraints = deltaAndConstraints
		self.dragSignal.emit( self, viewport, delta.x )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )







class ScreenSpace2Handle (HandleScreen):
	class ScreenSpaceHandle2Components (CompositeHandleComponentScreen):
		def __init__(self):
			self.xHandle = ScreenSpaceHandleAxisHandleComponent( Colour3f( 1.0, 0.0, 0.0 ), Vector2( 1.0, 0.0 ), Vector2( 0.0, 1.0 ) )
			self.yHandle = ScreenSpaceHandleAxisHandleComponent( Colour3f( 0.0, 1.0, 0.0 ), Vector2( 0.0, 1.0 ), Vector2( 1.0, 0.0 ) )
			self.freeHandle = ScreenSpaceHandleFreeHandleComponent()
			self._handles = [ self.xHandle, self.yHandle, self.freeHandle ]


		def isWithinBounds(self, boundsPolygon):
			return boundsPolygon.contains( Point2( -0.25, -0.25 ) )   and   boundsPolygon.contains( Point2( 1.25, 0.0 ) )   and  boundsPolygon.contains( Point2( 0.0, 1.25 ) )



	startSignal = ClassSignal()
	dragSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		b = ScreenSpace2Handle.ScreenSpaceHandle2Components()
		super( ScreenSpace2Handle, self ).__init__( b, b.freeHandle )



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, deltaAndConstraints):
		delta, constraints = deltaAndConstraints
		if constraints is not None:
			self.dragSignal.emit( self, viewport, delta.x, delta.y, constraints.x, constraints.y )
		else:
			self.dragSignal.emit( self, viewport, delta.x, delta.y, None, None )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )



