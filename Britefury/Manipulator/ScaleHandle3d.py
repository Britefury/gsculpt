##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from OpenGL.GL import *

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math import lerp
from Britefury.Math.Math import Colour3f, Vector2, Vector3, Point3, Triangle3, Segment3, Matrix4, Axes3, Plane

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import glColor4Colour3fA, glVertex3Point3

from Britefury.Manipulator.Handle3d import *



XFORMAXISHANDLE_ALPHA = 0.5
HANDLE_UNHIGHLIGHTEDBRIGHTNESS = 0.5
XFORMAXISHANDLE_SIZE = 0.1


SCALE_RATE							= 0.005

AXIS_ARROW_WIDTH						= 0.1
AXIS_ARROW_START						= 0.45
RADIAL_BAND_WIDTH					= 0.2

AXIS_ARROW_START_SIDE				= ( AXIS_ARROW_START - AXIS_ARROW_WIDTH )
RADIAL_BAND_START					= ( AXIS_ARROW_START - RADIAL_BAND_WIDTH )






class Scale3dDragHandleComponent (SingleHandleComponent3d):
	def __init__(self, colour, scaleVector, dragAxis, constraints):
		super( Scale3dDragHandleComponent, self ).__init__( colour )
		self.dragAxis = dragAxis
		self._scaleVector = scaleVector
		self._screenDragAxis = None
		self._initalPoint = None
		self._constraints = constraints


	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._screenDragAxis = ( self.dragAxis * localToCameraMatrix ).getNormalised().toVector2()
		if self._screenDragAxis.sqrLength()  <=  1.0e-6:
			self._screenDragAxis = Vector2( 1.0, 0.0 )
		self._initialPoint = copy( pointerPosition )
		return True

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		delta = pointerPosition - self._initialPoint
		scaleFactor = 1.0  +  delta.dot( self._screenDragAxis ) * motionScale * SCALE_RATE
		return Vector3( lerp( 1.0, scaleFactor, self._scaleVector.x ),
					  lerp( 1.0, scaleFactor, self._scaleVector.y ),
					  lerp( 1.0, scaleFactor, self._scaleVector.z ) ),  self._constraints

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._screenDragAxis = None
		self._initalPoint = None






class Scale3dAxisHandleComponent (Scale3dDragHandleComponent):
	def __init__(self, colour, scaleVector, shaft = Vector3( 0.0, 0.0, 1.0 ), u = Vector3( 1.0, 0.0, 0.0 ), v = Vector3( 0.0, 1.0, 0.0 )):
		super( Scale3dAxisHandleComponent, self ).__init__( colour, scaleVector, shaft, scaleVector )
		self.shaftAxis = shaft
		self.uAxis = u
		self.vAxis = v


	def isInsideFrustum(self, frustum):
		return frustum.contains( Point3() )  and  frustum.contains( Point3( self.shaftAxis ) )


	def _p_paint3d(self, viewport, bHighlighted):
		if bHighlighted:
			brightness = 1.0
		else:
			brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS

		glBegin( GL_TRIANGLES )

		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.shaftAxis * AXIS_ARROW_START_SIDE  +  self.uAxis * AXIS_ARROW_WIDTH ) )
		glColor4Colour3fA( self._colour * brightness, XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.shaftAxis * AXIS_ARROW_START ) )
		glVertex3Point3( Point3( self.shaftAxis ) )

		glVertex3Point3( Point3( self.shaftAxis * AXIS_ARROW_START ) )
		glVertex3Point3( Point3( self.shaftAxis ) )
		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.shaftAxis * AXIS_ARROW_START_SIDE  +  self.vAxis * AXIS_ARROW_WIDTH ) )

		glEnd()


	def _p_raytrace(self, raySegment):
		bSuccess, t, u, v, intersection = Triangle3( Point3( self.shaftAxis * AXIS_ARROW_START_SIDE  +  self.uAxis * AXIS_ARROW_WIDTH ),
											Point3( self.shaftAxis * AXIS_ARROW_START ),
											Point3( self.shaftAxis ) ).raytrace( raySegment )

		if bSuccess:
			raySegment = copy( raySegment )
			raySegment.b = intersection

		bSuccess2, t2, u, v, intersection2 = Triangle3( Point3( self.shaftAxis * AXIS_ARROW_START_SIDE  +  self.vAxis * AXIS_ARROW_WIDTH ),
											Point3( self.shaftAxis * AXIS_ARROW_START ),
											Point3( self.shaftAxis ) ).raytrace( raySegment )

		if bSuccess2:
			return True, intersection2
		elif bSuccess:
			return True, intersection
		else:
			return False, None





class Scale3dRadialHandleComponent (Scale3dDragHandleComponent):
	def __init__(self, colour, scaleVector, u = Vector3( 1.0, 0.0, 0.0 ), v = Vector3( 0.0, 1.0, 0.0 )):
		super( Scale3dRadialHandleComponent, self ).__init__( colour, scaleVector, u + v, [ u, v ] )
		self.uAxis = u
		self.vAxis = v


	def isInsideFrustum(self, frustum):
		return frustum.contains( Point3( self.uAxis * AXIS_ARROW_START ) )   and			\
				frustum.contains( Point3( self.uAxis * RADIAL_BAND_START ) )   and			\
				frustum.contains( Point3( self.vAxis * AXIS_ARROW_START ) )   and			\
				frustum.contains( Point3( self.vAxis * RADIAL_BAND_START ) )


	def _p_paint3d(self, viewport, bHighlighted):
		if bHighlighted:
			brightness = 1.0
		else:
			brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS

		glBegin( GL_TRIANGLES )

		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.uAxis * AXIS_ARROW_START ) )
		glColor4Colour3fA( self._colour * brightness, XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.uAxis * RADIAL_BAND_START ) )
		glVertex3Point3( Point3( self.vAxis * RADIAL_BAND_START ) )

		glVertex3Point3( Point3( self.vAxis * RADIAL_BAND_START ) )
		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.vAxis * AXIS_ARROW_START ) )
		glVertex3Point3( Point3( self.uAxis * AXIS_ARROW_START ) )

		glEnd()


	def _p_raytrace(self, raySegment):
		bSuccess, t, u, v, intersection = Triangle3( Point3( self.uAxis * AXIS_ARROW_START ),
											Point3( self.uAxis * RADIAL_BAND_START ),
											Point3( self.vAxis * RADIAL_BAND_START ) ).raytrace( raySegment )
		if bSuccess:
			return True, intersection

		bSuccess, t, u, v, intersection = Triangle3( Point3( self.vAxis * RADIAL_BAND_START ),
											Point3( self.vAxis * AXIS_ARROW_START ),
											Point3( self.uAxis * AXIS_ARROW_START ) ).raytrace( raySegment )
		if bSuccess:
			return True, intersection

		return False, None






class Scale3dUniformHandleComponent (SingleHandleComponent3d):
	def __init__(self, colour, u = Vector3( 1.0, 0.0, 0.0 ), v = Vector3( 0.0, 1.0, 0.0 ), w = Vector3( 0.0, 0.0, 1.0 )):
		super( Scale3dUniformHandleComponent, self ).__init__( colour )
		self.uAxis = u
		self.vAxis = v
		self.wAxis = w
		self._initialPoint = None


	def isInsideFrustum(self, frustum):
		return True


	def _p_paint3d(self, viewport, bHighlighted):
		if bHighlighted:
			brightness = 1.0
		else:
			brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS

		glBegin( GL_TRIANGLE_FAN )

		glColor4Colour3fA( self._colour * brightness, XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3() )
		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.uAxis * RADIAL_BAND_START ) )
		glVertex3Point3( Point3( self.vAxis * RADIAL_BAND_START ) )
		glVertex3Point3( Point3( self.wAxis * RADIAL_BAND_START ) )
		glVertex3Point3( Point3( self.uAxis * RADIAL_BAND_START ) )

		glEnd()


	def _p_raytrace(self, raySegment):
		return False, None


	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._initialPoint = copy( pointerPosition )
		return True

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		delta = pointerPosition.x - self._initialPoint.x
		scaleFactor = 1.0  +   delta * motionScale * SCALE_RATE
		return Vector3( scaleFactor, scaleFactor, scaleFactor ),  None

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._dragPlane = None
		self._initalPoint = None





class Scale2Handle3d (Handle3d):
	class Scale2Handle3dComponents (CompositeHandleComponent3d):
		def __init__(self):
			self.xHandle = Scale3dAxisHandleComponent( Colour3f( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.yHandle = Scale3dAxisHandleComponent( Colour3f( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.xyHandle = Scale3dUniformHandleComponent( Colour3f( 1.0, 1.0, 1.0 ) )
			self._handles = [ self.xHandle, self.yHandle, self.xyHandle ]


		def isInsideFrustum(self, frustum):
			return frustum.contains( Point3( -0.25, -0.25, -0.25) )   and   frustum.contains( Point3( 1.25, 0.0, 0.0 ) )   and			\
					frustum.contains( Point3( 0.0, 1.25, 0.0 ) )   and   frustum.contains( Point3( 0.0, 0.0, 1.25 ) )



	startSignal = ClassSignal()
	scaleSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		base = Scale2Handle3d.Scale2Handle3dComponents()
		super( Scale2Handle3d, self ).__init__( base, base.xyHandle )



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, scaleAndConstraints):
		scale, constraints = scaleAndConstraints
		if constraints is None:
			self.scaleSignal.emit( self, viewport, Vector2( scale.x, scale.y ), Vector2( 0.0, 0.0 ) )
		else:
			self.scaleSignal.emit( self, viewport, Vector2( scale.x, scale.y ), Vector2( constraints.x, constraints.y ) )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )






class Scale3Handle3d (Handle3d):
	class Scale3Handle3dComponents (CompositeHandleComponent3d):
		def __init__(self):
			self.xHandle = Scale3dAxisHandleComponent( Colour3f( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.yHandle = Scale3dAxisHandleComponent( Colour3f( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.zHandle = Scale3dAxisHandleComponent( Colour3f( 0.125, 0.375, 1.0 ), Vector3( 0.0, 0.0, 1.0 ), Vector3( 0.0, 0.0, 1.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ) )
			self.xyHandle = Scale3dRadialHandleComponent( Colour3f( 1.0, 1.0, 0.0 ), Vector3( 1.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ) )
			self.xzHandle = Scale3dRadialHandleComponent( Colour3f( 1.0, 0.0, 1.0 ), Vector3( 1.0, 0.0, 1.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.yzHandle = Scale3dRadialHandleComponent( Colour3f( 0.0, 1.0, 1.0 ), Vector3( 0.0, 1.0, 1.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.xyzHandle = Scale3dUniformHandleComponent( Colour3f( 1.0, 1.0, 1.0 ) )
			self._handles = [ self.xHandle, self.yHandle, self.zHandle, self.xyHandle, self.xzHandle, self.yzHandle, self.xyzHandle ]


		def isInsideFrustum(self, frustum):
			return frustum.contains( Point3( -0.25, -0.25, -0.25) )   and   frustum.contains( Point3( 1.25, 0.0, 0.0 ) )   and			\
					frustum.contains( Point3( 0.0, 1.25, 0.0 ) )   and   frustum.contains( Point3( 0.0, 0.0, 1.25 ) )



	startSignal = ClassSignal()
	scaleSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		base = Scale3Handle3d.Scale3Handle3dComponents()
		super( Scale3Handle3d, self ).__init__( base, base.xyzHandle )



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, scaleAndConstraints):
		self.scaleSignal.emit( self, viewport, *scaleAndConstraints )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )

