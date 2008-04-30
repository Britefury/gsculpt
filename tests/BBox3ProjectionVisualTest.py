import sys
import math
import random
from copy import copy

from Britefury.GL.GL import *
from Britefury.GL.GLU import *

from Britefury.Math.Math import Vector3, Point3, Matrix4, BBox3, BBox2, Vector2, Point2, EulerRotation


from Britefury.WorkArea.Viewport3d import Viewport3dPainter, Viewport3d



def randomVector3():
	return Vector3( random.random(), random.random(), random.random() )



class BBox3ProjectionTestPainter (Viewport3dPainter):
	def __init__(self):
		self._boxPos = Point3()
		self._halfBoxSize = randomVector3()
		self._box = BBox3( self._boxPos - self._halfBoxSize, self._boxPos + self._halfBoxSize )

	def addToView(self, view):
		view.addPainter( self, Viewport3d )

	def drawBBox3(self, box):
		l = box.getLower()
		u = box.getUpper()

		glBegin( GL_LINES )

		glColor3d( 1.0, 0.0, 0.0 )

		glVertex3d( l.x, l.y, l.z )
		glVertex3d( u.x, l.y, l.z )

		glVertex3d( l.x, u.y, l.z )
		glVertex3d( u.x, u.y, l.z )

		glVertex3d( l.x, l.y, u.z )
		glVertex3d( u.x, l.y, u.z )

		glVertex3d( l.x, u.y, u.z )
		glVertex3d( u.x, u.y, u.z )


		glVertex3d( l.x, l.y, l.z )
		glVertex3d( l.x, u.y, l.z )

		glVertex3d( u.x, l.y, l.z )
		glVertex3d( u.x, u.y, l.z )

		glVertex3d( l.x, l.y, u.z )
		glVertex3d( l.x, u.y, u.z )

		glVertex3d( u.x, l.y, u.z )
		glVertex3d( u.x, u.y, u.z )


		glVertex3d( l.x, l.y, l.z )
		glVertex3d( l.x, l.y, u.z )

		glVertex3d( u.x, l.y, l.z )
		glVertex3d( u.x, l.y, u.z )

		glVertex3d( l.x, u.y, l.z )
		glVertex3d( l.x, u.y, u.z )

		glVertex3d( u.x, u.y, l.z )
		glVertex3d( u.x, u.y, u.z )

		glEnd()


	def drawBBox2(self, box):
		l = box.getLower()
		u = box.getUpper()

		glBegin( GL_LINE_LOOP )

		glColor3d( 0.0, 1.0, 0.0 )

		glVertex2d( l.x, l.y )
		glVertex2d( u.x, l.y )
		glVertex2d( u.x, u.y )
		glVertex2d( l.x, u.y )

		glEnd()



	def paintExtreme3(self, v):
		extreme = self._box.getCentre() + v
		glVertex3d( extreme.x, extreme.y, extreme.z )



	def evPaint3d(self, viewport, paintLayer):
		self.drawBBox3( self._box )

		screenToWorldMatrix = viewport.camera.projection.screenToWorldMatrix
		worldToScreenMatrix = viewport.camera.projection.worldToScreenMatrix


		box2 = BBox2()
		for i in xrange( 0, 8 ):
			c = self._box.getPoint( i )
			box2.addPoint( worldToScreenMatrix.transformHomogeneous2d( c ) )


		glMatrixMode( GL_PROJECTION )
		glPushMatrix()
		viewport.glProjectionIdentity()

		glMatrixMode( GL_MODELVIEW )
		glPushMatrix()
		viewport.glModelViewIdentity()

		self.drawBBox2( box2 )

		glPopMatrix()
		glMatrixMode( GL_PROJECTION )
		glPopMatrix()
		glMatrixMode( GL_MODELVIEW )



	def getBoxPos(self):
		return self._boxPos

	def setBoxPos(self, p):
		self._boxPos = p
		self._box = BBox3( self._boxPos - self._halfBoxSize, self._boxPos + self._halfBoxSize )


	def getHalfBoxSize(self):
		return self._halfBoxSize

	def setHalfBoxSize(self, s):
		self._halfBoxSize = s
		self._box = BBox3( self._boxPos - self._halfBoxSize, self._boxPos + self._halfBoxSize )


	boxPos = property( getBoxPos, setBoxPos )
	halfBoxSize = property( getHalfBoxSize, setHalfBoxSize )



# To execute:
BBox3ProjectionTestPainter().addToView( gsc.app._view )
