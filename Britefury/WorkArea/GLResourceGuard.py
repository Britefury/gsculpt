##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.GL.GL import *

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Math.Math import Colour3f

from Britefury.Graphics.Graphics import glColor3Colour3f

from Britefury.WorkArea.DrawingArea import DrawingArea, DrawingAreaListener



class GLResourceGuard (DrawingAreaListener):
	def __init__(self, sharingManager):
		super( GLResourceGuard, self ).__init__()
		self._drawingArea = DrawingArea( sharingManager )
		self._drawingArea.setListener( self )
		self._backgroundColour = Colour3f()


	def getWidget(self):
		return self._drawingArea.getWidget()

	def setSizeRequest(self, width, height):
		self._drawingArea.getWidget().set_size_request( width, height )


	def evPaint(self, drawingArea):
		super( GLResourceGuard, self ).evPaint( drawingArea )
		glClearColor( self._backgroundColour.r, self._backgroundColour.g, self._backgroundColour.b, 0.0 )
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

		glEnable( GL_DEPTH_TEST )
		glDepthFunc( GL_LEQUAL )

		glEnable( GL_NORMALIZE )

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 )


		# Set projection matrix
		glMatrixMode( GL_PROJECTION )
		glLoadIdentity()
		glOrtho( -1.0, 1.0, -1.0, 1.0, -1.0, 1.0 )



		# Render a small graphic
		glBegin( GL_TRIANGLE_FAN )
		glColor3d( 0.4157, 0.4667, 0.4 )
		glVertex2d( 0.0, 0.0 )
		glColor3Colour3f( self._backgroundColour )
		glVertex2d( 1.0, 0.0 )
		glVertex2d( 0.707, 0.707 )
		glVertex2d( 0.0, 1.0 )
		glVertex2d( -0.707, 0.707 )
		glVertex2d( -1.0, 0.0 )
		glVertex2d( -0.707, -0.707 )
		glVertex2d( 0.0, -1.0 )
		glVertex2d( 0.707, -0.707 )
		glVertex2d( 1.0, 0.0 )
		glEnd()


	def evRealise(self, drawingArea):
		super( GLResourceGuard, self ).evRealise( drawingArea )
		style = self.getWidget().get_style()
		gdkColour = style.bg[gtk.STATE_NORMAL]
		self._backgroundColour = Colour3f( float(gdkColour.red) / 65535.0, float(gdkColour.green) / 65535.0, float(gdkColour.blue) / 65535.0 )
