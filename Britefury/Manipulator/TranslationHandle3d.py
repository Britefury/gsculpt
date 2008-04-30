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
from Britefury.Math.Math import Colour3f, Vector2, Vector3, Point3, Triangle3, Segment3, Matrix4, Axes3, Plane

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import glColor4Colour3fA, glVertex3Point3

from Britefury.Manipulator.Handle3d import *



XFORMAXISHANDLE_ALPHA = 0.5
HANDLE_UNHIGHLIGHTEDBRIGHTNESS = 0.5
XFORMAXISHANDLE_SIZE = 0.1





class Translation3dAxisHandleComponent (SingleHandleComponent3d):
	def __init__(self, colour, shaft = Vector3( 0.0, 0.0, 1.0 ), u = Vector3( 1.0, 0.0, 0.0 ), v = Vector3( 0.0, 1.0, 0.0 )):
		super( Translation3dAxisHandleComponent, self ).__init__( colour )
		self.shaftAxis = shaft
		self.uAxis = u
		self.vAxis = v
		self._dragPlane = None
		self._initalPoint = None


	def isInsideFrustum(self, frustum):
		return frustum.contains( Point3() )  and  frustum.contains( Point3( self.shaftAxis ) )


	def _p_paint3d(self, viewport, bHighlighted):
		if bHighlighted:
			brightness = 1.0
		else:
			brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS

		glBegin( GL_TRIANGLES )

		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( ( self.uAxis + self.shaftAxis )  *  0.1 ) )
		glColor4Colour3fA( self._colour * brightness, XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3() )
		glVertex3Point3( Point3( self.shaftAxis ) )

		glVertex3Point3( Point3() )
		glVertex3Point3( Point3( self.shaftAxis ) )
		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( ( self.vAxis + self.shaftAxis )  *  0.1 ) )

		glEnd()


	def _p_raytrace(self, raySegment):
		bSuccess, t, u, v, intersection = Triangle3( Point3( ( self.uAxis + self.shaftAxis ) * 0.1 ), Point3( self.shaftAxis ), Point3() ).raytrace( raySegment )

		if bSuccess:
			raySegment = copy( raySegment )
			raySegment.b = intersection

		bSuccess2, t2, u, v, intersection2 = Triangle3( Point3( ( self.vAxis + self.shaftAxis ) * 0.1 ), Point3( self.shaftAxis ), Point3() ).raytrace( raySegment )

		if bSuccess2:
			return True, intersection2
		elif bSuccess:
			return True, intersection
		else:
			return False, None



	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		q = self.shaftAxis.cross( camForward )
		if abs( q.sqrLength() )  <  1.0e-6:
			return False
		normal = self.shaftAxis.cross( q )
		self._dragPlane = Plane( normal, Point3() )
		bSuccess, t, hitPoint = raySegment.intersect( self._dragPlane )
		if bSuccess:
			self._initialPoint = hitPoint
		return bSuccess

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		bSuccess, t, hitPoint = raySegment.intersect( self._dragPlane )
		if bSuccess:
			delta = hitPoint - self._initialPoint
			# Rescale @delta to compensate for the scale factor caused by any transformation wrappers. The scale factor can be deduced from @localToCameraMatrix.
			delta = delta.projectOntoUnitVector( self.shaftAxis )
			if delta.sqrLength()  <  ( hitPoint.toVector3().sqrLength()  *  1.0e-12 ):
				# No motion
				return Vector3(),  self.shaftAxis
			else:
				deltaLength = ( delta * localToCameraMatrix ).length()
				delta.normaliseToLength( deltaLength )
				return delta * motionScale,  self.shaftAxis
		else:
			return None

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._dragPlane = None
		self._initalPoint = None





class Translation3dPlaneHandleComponent (SingleHandleComponent3d):
	def __init__(self, colour, u = Vector3( 1.0, 0.0, 0.0 ), v = Vector3( 0.0, 1.0, 0.0 )):
		super( Translation3dPlaneHandleComponent, self ).__init__( colour )
		self.uAxis = u
		self.vAxis = v
		self._dragPlane = Plane( Point3(), u, v )
		self._initialPoint = None


	def isInsideFrustum(self, frustum):
		return frustum.contains( Point3( self.uAxis * 0.2  +  self.vAxis * 0.2 ) )   and			\
				frustum.contains( Point3( self.uAxis * 0.6  +  self.vAxis * 0.2 ) )   and			\
				frustum.contains( Point3( self.uAxis * 0.2  +  self.vAxis * 0.6 ) )


	def _p_paint3d(self, viewport, bHighlighted):
		if bHighlighted:
			brightness = 1.0
		else:
			brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS

		glBegin( GL_TRIANGLES )

		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.uAxis * 0.6  +  self.vAxis * 0.2 ) )
		glColor4Colour3fA( self._colour * brightness, XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.uAxis * 0.2  +  self.vAxis * 0.2 ) )
		glVertex3Point3( Point3( self.uAxis * 0.3  +  self.vAxis * 0.3 ) )

		glVertex3Point3( Point3( self.uAxis * 0.2  +  self.vAxis * 0.2 ) )
		glVertex3Point3( Point3( self.uAxis * 0.3  +  self.vAxis * 0.3 ) )
		glColor4Colour3fA( Colour3f(), XFORMAXISHANDLE_ALPHA )
		glVertex3Point3( Point3( self.uAxis * 0.2  +  self.vAxis * 0.6 ) )

		glEnd()


	def _p_raytrace(self, raySegment):
		bSuccess, t, u, v, intersection = Triangle3( Point3( self.uAxis * 0.6 + self.vAxis * 0.2 ),
											Point3( self.uAxis * 0.2 + self.vAxis * 0.2 ),
											Point3( self.uAxis * 0.3 + self.vAxis * 0.3 ) ).raytrace( raySegment )
		if bSuccess:
			return True, intersection

		bSuccess, t, u, v, intersection = Triangle3( Point3( self.uAxis * 0.2 + self.vAxis * 0.2 ),
											Point3( self.uAxis * 0.3 + self.vAxis * 0.3 ),
											Point3( self.uAxis * 0.2 + self.vAxis * 0.6 ) ).raytrace( raySegment )
		if bSuccess:
			return True, intersection

		return False, None



	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		bSuccess, t, hitPoint = raySegment.intersect( self._dragPlane )
		if bSuccess:
			self._initialPoint = hitPoint
		return bSuccess

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		bSuccess, t, hitPoint = raySegment.intersect( self._dragPlane )
		if bSuccess:
			delta = hitPoint - self._initialPoint
			# Rescale @delta to compensate for the scale factor caused by any transformation wrappers. The scale factor can be deduced from @localToCameraMatrix.
			if delta.sqrLength()  <  ( hitPoint.toVector3().sqrLength()  *  1.0e-12 ):
				# No motion
				return Vector3(),  [ self.uAxis, self.vAxis ]
			else:
				deltaLength = ( delta * localToCameraMatrix ).length()
				delta.normaliseToLength( deltaLength )
				return delta * motionScale,  [ self.uAxis, self.vAxis ]
		else:
			return None

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._initalPoint = None





class Translation3dFreeHandleComponent (SingleHandleComponent3d):
	def __init__(self):
		super( Translation3dFreeHandleComponent, self ).__init__( Colour3f() )
		self._dragPlane = None
		self._initialPoint = None


	def isInsideFrustum(self, frustum):
		return True


	def _p_paint3d(self, viewport, bHighlighted):
		pass


	def _p_raytrace(self, raySegment):
		return False, None



	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._dragPlane = Plane( -camForward, Point3() )
		bSuccess, t, hitPoint = raySegment.intersect( self._dragPlane )
		if bSuccess:
			self._initialPoint = hitPoint
		return bSuccess

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		bSuccess, t, hitPoint = raySegment.intersect( self._dragPlane )
		if bSuccess:
			delta = hitPoint - self._initialPoint
			# Rescale @delta to compensate for the scale factor caused by any transformation wrappers. The scale factor can be deduced from @localToCameraMatrix.
			if delta.sqrLength()  <  ( hitPoint.toVector3().sqrLength()  *  1.0e-12 ):
				# No motion
				return Vector3(),  None
			else:
				deltaLength = ( delta * localToCameraMatrix ).length()
				delta.normaliseToLength( deltaLength )
				return delta * motionScale,  None
		else:
			return None

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._dragPlane = None
		self._initalPoint = None






class Translation2Handle3d (Handle3d):
	class TranslationHandle3dComponents (CompositeHandleComponent3d):
		def __init__(self):
			self.xHandle = Translation3dAxisHandleComponent( Colour3f( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.yHandle = Translation3dAxisHandleComponent( Colour3f( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.xyHandle = Translation3dFreeHandleComponent()
			self._handles = [ self.xHandle, self.yHandle, self.xyHandle ]


		def isInsideFrustum(self, frustum):
			return frustum.contains( Point3( -0.25, -0.25, -0.25) )   and   frustum.contains( Point3( 1.25, 0.0, 0.0 ) )   and			\
					frustum.contains( Point3( 0.0, 1.25, 0.0 ) )   and   frustum.contains( Point3( 0.0, 0.0, 1.25 ) )



	startSignal = ClassSignal()
	translateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		base = Translation2Handle3d.TranslationHandle3dComponents()
		super( Translation2Handle3d, self ).__init__( base, base.xyHandle )



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, translationAndConstraints):
		translation, constraints = translationAndConstraints
		if constraints is None:
			self.translateSignal.emit( self, viewport, Vector2( translation.x, translation.y ), Vector2( 0.0, 0.0 ) )
		else:
			self.translateSignal.emit( self, viewport, Vector2( translation.x, translation.y ), Vector2( constraints.x, constraints.y ) )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )






class Translation3Handle3d (Handle3d):
	class TranslationHandle3dComponents (CompositeHandleComponent3d):
		def __init__(self):
			self.xHandle = Translation3dAxisHandleComponent( Colour3f( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.yHandle = Translation3dAxisHandleComponent( Colour3f( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.zHandle = Translation3dAxisHandleComponent( Colour3f( 0.125, 0.375, 1.0 ), Vector3( 0.0, 0.0, 1.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ) )
			self.xyHandle = Translation3dPlaneHandleComponent( Colour3f( 1.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ) )
			self.xzHandle = Translation3dPlaneHandleComponent( Colour3f( 1.0, 0.0, 1.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.yzHandle = Translation3dPlaneHandleComponent( Colour3f( 0.0, 1.0, 1.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.xyzHandle = Translation3dFreeHandleComponent()
			self._handles = [ self.xHandle, self.yHandle, self.zHandle, self.xyHandle, self.xzHandle, self.yzHandle, self.xyzHandle ]


		def isInsideFrustum(self, frustum):
			return frustum.contains( Point3( -0.25, -0.25, -0.25) )   and   frustum.contains( Point3( 1.25, 0.0, 0.0 ) )   and			\
					frustum.contains( Point3( 0.0, 1.25, 0.0 ) )   and   frustum.contains( Point3( 0.0, 0.0, 1.25 ) )



	startSignal = ClassSignal()
	translateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self):
		base = Translation3Handle3d.TranslationHandle3dComponents()
		super( Translation3Handle3d, self ).__init__( base, base.xyzHandle )



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, translationAndConstraints):
		self.translateSignal.emit( self, viewport, *translationAndConstraints )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )


