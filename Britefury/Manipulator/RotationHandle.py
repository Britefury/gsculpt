##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import weakref

from copy import copy

import math

from OpenGL.GL import *
from OpenGL.GLU import *

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math import lerp
from Britefury.Math import Math
from Britefury.Math.Math import Colour3f, Vector3, Point3, Triangle3, Segment3, Matrix4, Axes3, Plane, solveQuadratic, Vector2

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import glColor3Colour3f, glColor4Colour3fA, glVertex3Point3, glDrawString3


from Britefury.Cell.CellInterface import *

from Britefury.Manipulator.Handle3d import *




ROTATIONHANDLE_ALPHA				= 0.5
ROTATIONHANDLE_SEGMENTS			= 36
ROTATIONHANDLE_LOCK_SIZE			= 0.05

ROTATIONHANDLE_ARC_WIDTH			= 0.6

RING_WIDTH								= 0.06
RING_INNER_RADIUS						= ( 1.0 - RING_WIDTH )
RING_OUTER_RADIUS					= ( 1.0 + RING_WIDTH )
RING_SQR_INNER_RADIUS				= ( RING_INNER_RADIUS * RING_INNER_RADIUS )
RING_SQR_OUTER_RADIUS				= ( RING_OUTER_RADIUS * RING_OUTER_RADIUS )



PLANE_ROTATION_RATE					= math.radians( 0.25 )


HANDLE_UNHIGHLIGHTEDBRIGHTNESS	= 0.5






class RotationHandleQuadric (object):
	s_globalQuadric = None

	def __init__(self):
		self._sphere = gluNewQuadric()
		gluQuadricDrawStyle( self._sphere, GLU_FILL )
		gluQuadricNormals( self._sphere, GLU_SMOOTH )


	@classmethod
	def getSphere(cls):
		if cls.s_globalQuadric is None  or  cls.s_globalQuadric() is None:
			sphere = RotationHandleQuadric()
			cls.s_globalQuadric = weakref.ref( sphere )
			return sphere._sphere
		else:
			sphere = cls.s_globalQuadric()
			return sphere._sphere




def lineUnitSphereIntersect(pointOnLine, lineVector):
	p = pointOnLine.toVector3()
	q = lineVector
	a = q.dot( q )				# term t^2
	b = p.dot( q ) * 2.0			# term t
	c = p.dot( p )  -  1.0			# term 1

	roots = solveQuadratic( a, b, c )

	if roots.hasRoots():
		return True, roots.getMinimumRoot(), roots.getMaximumRoot()
	else:
		return False, None, None

def raytraceUnitSphere(raySegment):
	return lineUnitSphereIntersect( raySegment.a, raySegment.b - raySegment.a )








class RotationHandleComponent (SingleHandleComponent3d):
	def __init__(self, owner, colour):
		super( RotationHandleComponent, self ).__init__( colour )
		self._owner = owner


	def _o_snapRotation(self, theta, pointerStatus):
		if self._owner.snapping is not None:
			theta = math.degrees( theta )
			if isinstance( self._owner.snapping, CellInterface ):
				theta = self._owner.snapping.getValue().snap( theta, pointerStatus.bControlKey, pointerStatus.bShiftKey )
			else:
				theta = self._owner.snapping.snap( theta, pointerStatus.bControlKey, pointerStatus.bShiftKey )
			theta = math.radians( theta )
		return theta


	def _o_postRedraw(self):
		self._owner._view.postRedraw()





class RotationRingHandleComponent (RotationHandleComponent):
	def __init__(self, owner, colour, rotationVector, planeU, planeV, planeNormal):
		super( RotationRingHandleComponent, self ).__init__( owner, colour )
		self._rotationVector = rotationVector
		self.planeU = planeU
		self.planeV = planeV
		self.planeNormal = planeNormal
		self._ringPlane = Plane( planeNormal, 0.0 )
		self._initialPoint = None
		self._currentRotation = None
		self._dragVector = None
		self._rotationPointOnRing = None



	def isInsideFrustum(self, frustum):
		return frustum.contains( Point3( self.planeU * 1.25 ) )  and  frustum.contains( Point3( -self.planeU * 1.25 ) )  and		\
				frustum.contains( Point3( self.planeV * 1.25 ) )  and  frustum.contains( Point3( -self.planeV * 1.25 ) )


	def _p_paint3d(self, viewport, bHighlighted):
		# Paint the ring
		if bHighlighted:
			brightness = 1.0
			thickness = 4.0
		else:
			brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS
			thickness = 3.0

		glLineWidth( thickness )
		glBegin( GL_LINE_STRIP )

		glColor4Colour3fA( self._colour * brightness, ROTATIONHANDLE_ALPHA )

		deltaTheta = math.pi * 2.0  /  float( ROTATIONHANDLE_SEGMENTS )
		theta = 0.0
		for i in xrange( 0, ROTATIONHANDLE_SEGMENTS + 1 ):
			glVertex3Point3( Point3( self.planeU * math.cos( theta )  +  self.planeV * math.sin( theta ) ) )
			theta += deltaTheta

		glEnd()
		glLineWidth( 1.0 )


		if self._currentRotation is not None:
			# Paint the rotation
			glBegin( GL_TRIANGLE_STRIP )

			deltaTheta = -self._currentRotation / float( ROTATIONHANDLE_SEGMENTS )
			theta = 0.0
			for i in xrange( 0, ROTATIONHANDLE_SEGMENTS + 1 ):
				v = self._rotationPointOnRing  *  Matrix4.rotate( self.planeNormal, theta )
				glColor4d( 0.0, 0.0, 0.0, 1.0 )
				glVertex3Point3( Point3( v * ROTATIONHANDLE_ARC_WIDTH ) )
				glColor4Colour3fA( self._colour, ROTATIONHANDLE_ALPHA )
				glVertex3Point3( Point3( v ) )
				theta += deltaTheta

			glEnd()

			glColor3Colour3f( self._colour )
			glDrawString3( Point3( self._rotationPointOnRing * 1.1 ), str( math.degrees( -self._currentRotation ) ) )




	def _p_checkRingSphereIntersectionValidity(self, point):
		u = abs( point.dot( self.planeU ) )
		v = abs( point.dot( self.planeV ) )
		n = abs( point.dot( self.planeNormal ) )

		return n < u  and  n < v  and  n < ROTATIONHANDLE_LOCK_SIZE



	def _p_raytraceRingPlane(self, raySegment):
		bSuccess, t, intersection = raySegment.intersect( self._ringPlane )
		if bSuccess:
			i = intersection.dot( self.planeU )
			j = intersection.dot( self.planeV )
			sqrRadius = i * i  +  j * j
			if sqrRadius >= RING_SQR_INNER_RADIUS  and  sqrRadius <= RING_SQR_OUTER_RADIUS:
				return True, intersection
			else:
				return False, None
		else:
			return False, None

	def _p_raytraceRingSphere(self, raySegment):
		bSuccess, tNear, tFar = raytraceUnitSphere( raySegment )

		if bSuccess:
			pNear = raySegment.getPoint( tNear )
			pFar = raySegment.getPoint( tFar )

			if self._p_checkRingSphereIntersectionValidity( pNear ):
				return True, pNear
			elif self._p_checkRingSphereIntersectionValidity( pFar ):
				return True, pFar
			else:
				return False, None
		else:
			return False, None




	def _p_raytraceRing(self, raySegment):
		bSuccess, intersection = self._p_raytraceRingPlane( raySegment )

		if bSuccess:
			raySegment = copy( raySegment )
			raySegment.b = intersection

		bSuccess2, intersection2 = self._p_raytraceRingSphere( raySegment )

		if bSuccess2:
			return True, intersection2
		elif bSuccess:
			return True, intersection
		else:
			return False, None


	def _p_raytrace(self, raySegment):
		return self._p_raytraceRing( raySegment )


	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		bSuccess, intersection = self._p_raytraceRing( raySegment )

		if bSuccess:
			uv = Vector2( intersection.dot( self.planeU ), intersection.dot( self.planeV ) )
			uv.normalise()
			self._rotationPointOnRing = self.planeU * uv.x  +  self.planeV * uv.y
			dragVector3 = self.planeU * uv.y  +  self.planeV * -uv.x
			self._dragVector = ( dragVector3 * localToCameraMatrix ).toVector2()
			if self._dragVector.sqrLength()  >  1.0e-12:
				self._dragVector.normalise()
			else:
				self._dragVector = Vector2( 1.0, 0.0 )
		else:
			self._dragVector = Vector2( 1.0, 0.0 )
		self._initialPoint = copy( pointerPosition )
		self._currentRotation = 0.0
		return True

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		delta = ( pointerPosition - self._initialPoint ).dot( self._dragVector )
		self._currentRotation = self._o_snapRotation( -delta * motionScale * PLANE_ROTATION_RATE, pointerStatus )
		return self._rotationVector, self._currentRotation

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._initalPoint = None
		self._currentRotation = None
		self._dragVector = None
		self._rotationPointOnRing = None
		self._o_postRedraw()






class RotationSphereHandleComponent (RotationHandleComponent):
	def __init__(self, owner, colour, u = Vector3( 1.0, 0.0, 0.0 ), v = Vector3( 0.0, 1.0, 0.0 ), w = Vector3( 0.0, 0.0, 1.0 )):
		super( RotationSphereHandleComponent, self ).__init__( owner, colour )
		self.uAxis = u
		self.vAxis = v
		self.wAxis = w
		self._rotationStart = None
		self._rotationTheta = None
		self._rotationAxis = None
		self._bSphereRotation = False
		self._initialPointer = None


	def isInsideFrustum(self, frustum):
		return frustum.contains( Point3( self.uAxis * 1.25 ) )  and  frustum.contains( Point3( -self.uAxis * 1.25 ) )  and		\
				frustum.contains( Point3( self.vAxis * 1.25 ) )  and  frustum.contains( Point3( -self.vAxis * 1.25 ) )  and		\
				frustum.contains( Point3( self.wAxis * 1.25 ) )  and  frustum.contains( Point3( -self.wAxis * 1.25 ) )


	def _p_paint3d(self, viewport, brightness):
		# Paint sphere
		mat_diffuse = [ 0.05, 0.1, 0.3, 0.7 ]
		mat_specular = [ 0.3, 0.3, 0.3, 1.0 ]
		mat_shininess = [ 50.0 ]

		glMaterialfv( GL_FRONT, GL_DIFFUSE, mat_diffuse )
		glMaterialfv( GL_FRONT, GL_SPECULAR, mat_specular )
		glMaterialfv( GL_FRONT, GL_SHININESS, mat_shininess )
		glEnable( GL_LIGHTING )

		glCullFace( GL_BACK )
		glEnable( GL_CULL_FACE )

		gluSphere( RotationHandleQuadric.getSphere(), 1.0, 36, 18 )

		glDisable( GL_CULL_FACE )

		glDisable( GL_LIGHTING )


		if self._rotationAxis is not None:
			glBegin( GL_TRIANGLE_STRIP )

			deltaTheta = -self._rotationTheta / float( ROTATIONHANDLE_SEGMENTS )
			theta = 0.0
			for i in xrange( 0, ROTATIONHANDLE_SEGMENTS + 1 ):
				v = self._rotationStart * Matrix4.rotate( self._rotationAxis, theta )
				glColor4d( 0.0, 0.0, 0.0, 1.0 )
				glVertex3Point3( Point3( v * ROTATIONHANDLE_ARC_WIDTH ) )
				glColor4d( 1.0, 1.0, 0.0, ROTATIONHANDLE_ALPHA )
				glVertex3Point3( Point3( v ) )
				theta += deltaTheta
			glEnd()

			glColor3d( 1.0, 1.0, 0.0 )
			glDrawString3( Point3( self._rotationStart * 1.1 ), str( math.degrees( -self._rotationTheta ) ) )


	def _p_raytrace(self, raySegment):
		# The sphere component is the default component that gets selected, so do not detect intersections, so that the ring handles always take precedence
		return False, None


	def _p_dragStart3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._initialPointer = copy( pointerPosition )
		bSuccess, tNear, tFar = raytraceUnitSphere( raySegment )
		if bSuccess:
			self._rotationStart = raySegment.getPoint( tNear ).toVector3()
			self._rotationTheta = 0.0
		else:
			self._rotationStart = -camForward
		self._bSphereRotation = bSuccess
		return True

	def _p_dragMotion3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		if self._bSphereRotation:
			bSuccess, tNear, tFar = raytraceUnitSphere( raySegment )
			if bSuccess:
				u = self._rotationStart
				v = raySegment.getPoint( tNear ).toVector3()

				self._rotationAxis = -u.cross( v ).getNormalised()
				cosTheta = u.dot( v )
				theta = -math.acos( min( max( cosTheta, -1.0 ), 1.0 ) )
				self._rotationTheta = self._o_snapRotation( theta * motionScale, pointerStatus )
				return self._rotationAxis, self._rotationTheta
			else:
				return None, None
		else:
			m = localToCameraMatrix.rigidBodyInverse()
			u = Vector3( 1.0, 0.0, 0.0 ) * m
			v = Vector3( 0.0, 1.0, 0.0 ) * m
			delta = pointerPosition - self._initialPointer
			if delta.sqrLength()  >  0.0:
				self._rotationAxis = u * -delta.y  +  v * delta.x
				self._rotationAxis.normalise()
				self._rotationTheta = self._o_snapRotation( delta.length() * motionScale * PLANE_ROTATION_RATE, pointerStatus )
				return self._rotationAxis, self._rotationTheta
			else:
				return None, None

	def _p_dragFinish3d(self, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._rotationStart = None
		self._rotationAxis = None
		self._rotationTheta = None
		self._bSphereRotation = False
		self._initialPointer = None
		self._o_postRedraw()





class RotationHandle (Handle3d):
	class RotationHandleComponents (CompositeHandleComponent3d):
		def __init__(self, owner):
			self.xHandle = RotationRingHandleComponent( owner, Colour3f( 1.0, 0.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, -1.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ) )
			self.yHandle = RotationRingHandleComponent( owner, Colour3f( 0.0, 1.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 0.0, -1.0 ), Vector3( 0.0, 1.0, 0.0 ) )
			self.zHandle = RotationRingHandleComponent( owner, Colour3f( 0.125, 0.375, 1.0 ), Vector3( 0.0, 0.0, 1.0 ), Vector3( 1.0, 0.0, 0.0 ), Vector3( 0.0, 1.0, 0.0 ), Vector3( 0.0, 0.0, 1.0 ) )
			self.sphereHandle = RotationSphereHandleComponent( owner, Colour3f( 1.0, 1.0, 1.0 ) )
			self._handles = [ self.xHandle, self.yHandle, self.zHandle, self.sphereHandle ]


		def isInsideFrustum(self, frustum):
			return frustum.contains( Point3( -1.25, 0.0, 0.0 ) )  and  frustum.contains( Point3( 1.25, 0.0, 0.0 ) )  and		\
					frustum.contains( Point3( 0.0, -1.25, 0.0 ) )  and  frustum.contains( Point3( 0.0, 1.25, 0.0 ) )  and		\
					frustum.contains( Point3( 0.0, 0.0, -1.25 ) )  and  frustum.contains( Point3( 0.0, 0.0, 1.25 ) )



	startSignal = ClassSignal()
	rotateSignal = ClassSignal()
	finishSignal = ClassSignal()


	def __init__(self, snapping=None):
		base = RotationHandle.RotationHandleComponents( self )
		super( RotationHandle, self ).__init__( base, base.sphereHandle )

		self.snapping = snapping



	def _p_onDragStart(self, viewport):
		self.startSignal.emit( self, viewport )

	def _p_onDragMotion(self, viewport, rotation):
		axis, angle = rotation
		if axis is not None  and  angle is not None:
			self.rotateSignal.emit( self, viewport, axis, angle )

	def _p_onDragFinish(self, viewport):
		self.finishSignal.emit( self, viewport )

