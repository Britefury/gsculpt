##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy
import weakref

import pygtk
pygtk.require('2.0')
import gtk
import gtk.gtkgl
import gobject
import pango

from Britefury.GL.GL import *
from Britefury.GL.GLU import *


from Britefury.Math import Math
from Britefury.Math.Math import Vector2i, Point2i

from Britefury.Util.SignalSlot import ClassSignal
from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import setDrawingFont, disableDrawingFont
from Britefury.WorkArea import PointerStatus






FONT_LIST_SIZE = 128
FONT_STRING = 'Sans 12'



class DrawingAreaListener (object):
	"""Drawing area listener interface"""

	def evPaint(self, drawingArea):
		"""Paint event
		@drawingArea: drawing area that sent the event"""
		pass

	def evButtonDown(self, drawingArea, button):
		"""Button down event
		@drawingArea: drawing area that sent the event
		@button: index of the button that was pressed"""
		pass

	def evButtonDown2(self, drawingArea, button):
		"""Button double click event
		@drawingArea: drawing area that sent the event
		@button: index of the button that was pressed"""
		pass

	def evButtonDown3(self, drawingArea, button):
		"""Button triple click event
		@drawingArea: drawing area that sent the event
		@button: index of the button that was pressed"""
		pass

	def evButtonUp(self, drawingArea, button):
		"""Button up event
		@drawingArea: drawing area that sent the event
		@button: index of the button that was released"""
		pass

	def evMotion(self, drawingArea, position):
		"""Motion event
		@drawingArea: drawing area that sent the event
		@position: (Point2i) position of the pointer"""
		pass

	def evEnter(self, drawingArea, position):
		"""Enter event
		@drawingArea: drawing area that sent the event
		@position: (Point2i) position of the pointer"""
		pass

	def evLeave(self, drawingArea, position):
		"""Leave event
		@drawingArea: drawing area that sent the event
		@position: (Point2i) position of the pointer"""
		pass

	def evLeaveUngrab(self, drawingArea):
		"""Leave-ungrab event
		@drawingArea: drawing area that sent the event"""
		pass

	def evScroll(self, drawingArea, x, y):
		"""Scroll event
		@drawingArea: drawing area that sent the event
		@x: scroll in the x-axis
		@y: scroll in the y-axis"""
		pass

	def evConfig(self, drawingArea, size):
		"""Configure (resize) event
		@drawingArea: drawing area that sent the event
		@size: (Vector2i) the new size of the drawing area"""
		pass

	def evRealise(self, drawingArea):
		"""Realise
		@drawingArea: drawing area that sent the event"""
		pass

	def evUnrealise(self, drawingArea):
		"""Unrealise
		@drawingArea: drawing area that sent the event"""
		pass




class DrawingAreaSharingManager (object):
	"""Drawing area resource sharing manager"""
	class Resources (object):
		def __init__(self):
			self.bInitialised = False
			self.fontBase = None
			self.fontHeight = None


	initialiseGLResourcesSignal = ClassSignal()
	shutdownGLResourcesSignal = ClassSignal()


	def __init__(self):
		"""Initialise sharing manager"""
		self._resources = DrawingAreaSharingManager.Resources()
		self._contexts = []



	def _p_groupMemberRealised(self, drawingArea, context, drawable):
		bContextsEmpty = len( self._contexts ) == 0
		self._contexts.append( context )
		if bContextsEmpty:
			self._p_initialiseResources( drawingArea, context, drawable )

	def _p_groupMemberUnrealised(self, drawingArea, context, drawable):
		if context is not None:
			self._contexts.remove( context )
		if len( self._contexts ) == 0:
			self._p_shutdownResources( drawingArea, context, drawable )


	def _p_initialiseResources(self, drawingArea, context, drawable):
		global FONT_LIST_SIZE, FONT_STRING
		assert not self._resources.bInitialised, 'resources already initialised'

		drawable.gl_begin( context )

		# Generate the OpenGL display lists for font rendering
		self._resources.fontBase = glGenLists( FONT_LIST_SIZE )

		# Get the Pango font description
		fontDescription = pango.FontDescription( FONT_STRING )

		# Use it for OpenGL rendering
		font = gtk.gdkgl.font_use_pango_font( fontDescription, 0, FONT_LIST_SIZE, self._resources.fontBase )

		# Get the font metrics
		fontMetrics = font.get_metrics()

		# Compute the height
		fontHeight = fontMetrics.get_ascent() + fontMetrics.get_descent()
		self._resources.fontHeight = pango.PIXELS( fontHeight )

		# Done
		self.initialiseGLResourcesSignal.emit( self )
		drawable.gl_end()
		self._resources.bInitialised = True

	def _p_shutdownResources(self, drawingArea, context, drawable):
		global FONT_LIST_SIZE, FONT_STRING
		assert self._resources.bInitialised, 'resources not initialised'

		drawable.gl_begin( context )

		# Destroy the font display list
		glDeleteLists( self._resources.fontBase, FONT_LIST_SIZE )
		self.shutdownGLResourcesSignal.emit( self )
		drawable.gl_end()
		self._resources.bInitialised = False


	def _p_getSharedContext(self):
		if len( self._contexts ) == 0:
			return None
		else:
			return self._contexts[0]

	def _p_getFontBase(self):
		assert self._resources.bInitialised, 'resources not initialised'
		return self._resources.fontBase

	def _p_getFontHeight(self):
		assert self._resources.bInitialised, 'resources not initialised'
		return self._resources.fontHeight





class DrawingArea (object):
	"""OpenGL based drawing area widget"""
	s_redrawHandle = 0
	s_redrawList = []
	s_glConfig = None

	def __init__(self, sharingManager):
		"""Initialise the drawing area
		@sharingManager: a DrawingAreaSharingManager, resource sharing manager"""
		assert sharingManager is not None, 'no sharing manager'
		self._sharingManager = sharingManager

		self._pointerStatus = PointerStatus.PointerStatus()
		self._size = Vector2i()

		self._bRealised = False
		self._bFrozen = False
		self._bRedrawPending = False

		self._listener = None

		# Create the Gtk drawing area widget
		self._drawingArea = gtk.DrawingArea()
		gtk.gtkgl.widget_set_gl_capability( self._drawingArea, DrawingArea.ps_getGLConfig() )
		self._context = None

		# Connect signals
		self._drawingArea.connect_after( 'configure-event', self._p_onConfigure )
		self._drawingArea.connect( 'expose-event', self._p_onExpose )
		self._drawingArea.connect( 'button-press-event', self._p_onButtonPress )
		self._drawingArea.connect( 'button-release-event', self._p_onButtonRelease )
		self._drawingArea.connect( 'motion-notify-event', self._p_onMotionNotify )
		self._drawingArea.connect( 'enter-notify-event', self._p_onEnterNotify )
		self._drawingArea.connect( 'leave-notify-event', self._p_onLeaveNotify )
		self._drawingArea.connect( 'scroll-event', self._p_onScroll )
		self._drawingArea.connect_after( 'realize', self._p_onRealise )
		self._drawingArea.connect( 'unrealize', self._p_onUnrealise )

		# Tell the widget to send these events
		self._drawingArea.add_events( gtk.gdk.EXPOSURE_MASK |
									gtk.gdk.BUTTON_PRESS_MASK |
									gtk.gdk.BUTTON_RELEASE_MASK |
									gtk.gdk.POINTER_MOTION_MASK |
									gtk.gdk.POINTER_MOTION_HINT_MASK |
									gtk.gdk.ENTER_NOTIFY_MASK |
									gtk.gdk.LEAVE_NOTIFY_MASK |
									gtk.gdk.SCROLL_MASK )


	def setListener(self, listener):
		"""Set the event listener
		@listener: event listener"""
		self._listener = listener



	def beginGL(self):
		"""Begin OpenGL drawing"""
		assert self._bRealised, 'Drawing area should be realised'
		drawable = gtk.gtkgl.widget_get_gl_drawable( self._drawingArea )
		drawable.gl_begin( self._context )
		setDrawingFont( self._sharingManager._p_getFontBase(), self._sharingManager._p_getFontHeight() )

	def endGL(self):
		"""End OpenGL drawing"""
		assert self._bRealised, 'Drawing area should be realised'
		drawable = gtk.gtkgl.widget_get_gl_drawable( self._drawingArea )
		drawable.gl_end()
		disableDrawingFont()

	def swapBuffers(self):
		"""Swap buffers"""
		drawable = gtk.gtkgl.widget_get_gl_drawable( self._drawingArea )
		drawable.swap_buffers()


	def freeze(self):
		self._bFrozen = True


	def thaw(self):
		self._bFrozen = False


	def getWidget(self):
		"""Get the drawing area widget"""
		return self._drawingArea


	def postRedraw(self):
		"""Post a redraw"""
		if self._bRealised  and  not self._bFrozen:
			# If there is not already a pending redraw
			if not self._bRedrawPending:
				DrawingArea.s_redrawList.append( weakref.ref( self ) )

				if DrawingArea.s_redrawHandle == 0:
					DrawingArea.s_redrawHandle = gobject.idle_add( DrawingArea.s_onRedraw, priority = gobject.PRIORITY_HIGH_IDLE + 20 )
				self._bRedrawPending = True

	def forceRedraw(self):
		"""Force a redraw"""
		if self._bRealised  and  not self._bFrozen:
			self._p_forceRedraw()

	def forcePendingRedraw(self):
		"""Force a pending redraw"""
		if self._bRealised  and  not self._bFrozen:
			# Force only if a redraw is pending
			if self._bRedrawPending:
				# Stop the pending redraw
				DrawingArea.s_redrawList = [ item   for item in DrawingArea.s_redrawList   if item() is not self ]
				self._bRedrawPending = False
				self._p_repaint()



	def _p_forceRedraw(self):
		# Remove any pending redraw
		if self._bRedrawPending:
			DrawingArea.s_redrawList = [ item   for item in DrawingArea.s_redrawList   if item() is not self ]
			self._bRedrawPending = False
		self._p_repaint()


	def _p_repaint(self):
		"""Repaint"""
		if self._bRealised:
			self.beginGL()

			if self._listener is not None:
				self._listener.evPaint( self )

			self.swapBuffers()

			self.endGL()




	def _p_onConfigure(self, widget, event):
		width = widget.allocation.width
		height = widget.allocation.height
		self._size = Vector2i( width, height )

		if self._bRealised:
			if self._listener is not None:
				self._listener.evConfig( self, self._size )

			# Initialise GL viewport and repaint
			self.beginGL()
			glViewport( 0, 0, width, height )
			self.endGL()
			self._p_forceRedraw()


	def _p_onExpose(self, widget, event):
		self._p_forceRedraw()


	def _p_onButtonPress(self, widget, event):
		x, y, state = event.window.get_pointer()
		self._pointerStatus._p_setState( state )


		if event.type == gtk.gdk.BUTTON_PRESS:
			if self._listener is not None:
				self._listener.evButtonDown( self, event.button )
		elif event.type == gtk.gdk._2BUTTON_PRESS:
			if self._listener is not None:
				self._listener.evButtonDown2( self, event.button )
		elif event.type == gtk.gdk._3BUTTON_PRESS:
			if self._listener is not None:
				self._listener.evButtonDown3( self, event.button )

		return False


	def _p_onButtonRelease(self, widget, event):
		x, y, state = event.window.get_pointer()
		self._pointerStatus._p_setState( state )

		if self._listener is not None:
			self._listener.evButtonUp( self, event.button )

		return False


	def _p_onMotionNotify(self, widget, event):
		if event.is_hint:
			x, y, state = event.window.get_pointer()
			self._pointerStatus._p_setPosition( self._p_windowCoordsToScreenCoords( Point2i( x, y ) ) )
			self._pointerStatus._p_setState( state )
		else:
			self._pointerStatus._p_setPosition( self._p_windowCoordsToScreenCoords( Point2i( int( event.x ), int( event.y ) ) ) )
			self._pointerStatus._p_setState( event.state )

		if self._listener is not None:
			self._listener.evMotion( self, self._pointerStatus.position )

		return False


	def _p_onEnterNotify(self, widget, event):
		x, y, state = event.window.get_pointer()

		self._pointerStatus._p_setPosition( self._p_windowCoordsToScreenCoords( Point2i( x, y ) ) )
		self._pointerStatus._p_setInsideBounds( True )
		self._pointerStatus._p_setState( state )

		# Only want normal events
		if event.mode == gtk.gdk.CROSSING_NORMAL:
			if self._listener is not None:
				self._listener.evEnter( self, self._pointerStatus.position )

		return False


	def _p_onLeaveNotify(self, widget, event):
		x, y, state = event.window.get_pointer()

		self._pointerStatus._p_setPosition( self._p_windowCoordsToScreenCoords( Point2i( x, y ) ) )
		self._pointerStatus._p_setInsideBounds( False )
		self._pointerStatus._p_setState( state )

		if event.mode == gtk.gdk.CROSSING_NORMAL:
			# Normal crossing event
			if self._listener is not None:
				self._listener.evLeave( self, self._pointerStatus.position )
		elif event.mode == gtk.gdk.CROSSING_UNGRAB:
			if self._listener is not None:
				self._listener.evLeaveUngrab( self )

		return False


	def _p_onScroll(self, widget, event):
		pointerX, pointerY, state = event.window.get_pointer()
		self._pointerStatus._p_setState( state )

		x = 0
		y = 0

		if event.direction == gtk.gdk.SCROLL_UP:
			y = 1
		elif event.direction == gtk.gdk.SCROLL_DOWN:
			y = -1
		if event.direction == gtk.gdk.SCROLL_LEFT:
			x = -1
		elif event.direction == gtk.gdk.SCROLL_RIGHT:
			x = 1

		if self._listener is not None:
			self._listener.evScroll( self, x, y )

		return False


	def _p_onRealise(self, widget):
		self._bRealised = True

		shareContext = self._sharingManager._p_getSharedContext()
		self._context = gtk.gtkgl.widget_create_gl_context( self._drawingArea, shareContext )
		drawable = gtk.gtkgl.widget_get_gl_drawable( self._drawingArea )
		self._sharingManager._p_groupMemberRealised( self, self._context, drawable )

		if self._listener is not None:
			self._listener.evRealise( self )

		# Send a configure event
		if self._listener is not None:
			self._listener.evConfig( self, self._size )

		# Initialise viewport and repaint
		self.beginGL()
		glViewport( 0, 0, self._size.x, self._size.y )
		self.endGL()
		self._p_forceRedraw()


	def _p_onUnrealise(self, widget):
		drawable = gtk.gtkgl.widget_get_gl_drawable( self._drawingArea )
		self._sharingManager._p_groupMemberUnrealised( self, self._context, drawable )
		self._context.destroy()
		self._context = None

		self._bRealised = False
		self._pointerStatus._p_reset()

		if self._listener is not None:
			self._listener.evUnrealise( self )



	@staticmethod
	def ps_getGLConfig():
		if DrawingArea.s_glConfig is None:
			displayMode = gtk.gdkgl.MODE_RGB | gtk.gdkgl.MODE_DEPTH | gtk.gdkgl.MODE_DOUBLE
			DrawingArea.s_glConfig = gtk.gdkgl.Config( mode=displayMode )

		return DrawingArea.s_glConfig


	@staticmethod
	def s_onRedraw():
		gobject.source_remove( DrawingArea.s_redrawHandle )
		DrawingArea.s_redrawHandle = 0

		for drawingAreaRef in DrawingArea.s_redrawList :
			drawingArea = drawingAreaRef()
			drawingArea._bRedrawPending = False
			drawingArea._p_repaint()

		DrawingArea.s_redrawList = []

		return False


	def _p_windowCoordsToScreenCoords(self, windowPoint):
		point = copy( windowPoint )
		point.y = self._size.y - point.y - 1
		return point



	def _p_getSize(self):
		"""Private - Get the drawing area size"""
		return self._size

	def _p_getPointerStatus(self):
		"""Private - Get the pointer status"""
		return self._pointerStatus

	def _p_isRealised(self):
		"""Private - Check if the drawing area is realised"""
		return self._bRealised


	size = property( _p_getSize )
	pointerStatus = property( _p_getPointerStatus )
	bIsRealised = property( _p_isRealised )





if __name__ == '__main__':
	from Britefury.GL.GLU import *
	from Britefury.Math import Math
	from Britefury.Math.Math import Point2
	from Britefury.Graphics import Graphics
	from Britefury.Graphics.Graphics import glDrawString2

	class HelloWorld (DrawingAreaListener):
		def delete_event(self, widget, event, data=None):
			print( "delete_event" )
			return gtk.FALSE

		def destroy(self, widget, data=None):
			gtk.main_quit()

		def onValue(self,value):
			pass

		def evPaint(self, drawingArea):
			glMatrixMode( GL_MODELVIEW )
			glLoadIdentity()

			glMatrixMode( GL_PROJECTION )
			glLoadIdentity()
			gluOrtho2D( -1.0, 1.0, -1.0, 1.0 )

			glClearColor( 0.0, 0.0, 0.0, 0.0 )
			glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT )

			glEnable( GL_DEPTH_TEST )
			glDepthFunc( GL_LEQUAL )

			glEnable( GL_NORMALIZE )

			glPixelStorei( GL_UNPACK_ALIGNMENT, 1 )

			s = drawingArea.size
			glColor3d( 1.0, 0.0, 0.0 )
			glDrawString2( Point2( 0.0, 0.0 ), 'Hello world' )


		def evButtonDown(self, drawingArea, button):
			print 'evButtonDown %d' % ( button, )

		def evButtonUp(self, drawingArea, button):
			print 'evButtonDown %d' % ( button, )

		def evMotion(self, drawingArea, position):
			print 'evMotion %d,%d' % ( position.x, position.y )

		def evEnter(self, drawingArea, position):
			print 'evEnter %d,%d' % ( position.x, position.y )

		def evLeave(self, drawingArea, position):
			print 'evLeave %d,%d' % ( position.x, position.y )

		def evScroll(self, drawingArea, x, y):
			print 'evScroll %d, %d' % ( x, y )


		def __init__(self):
			sharingManager = DrawingAreaSharingManager()
			self.drawingArea = DrawingArea( sharingManager )
			self.drawingArea.setListener( self )

			self.window = gtk.Window( gtk.WINDOW_TOPLEVEL );
			self.window.connect( 'delete-event', self.delete_event )
			self.window.connect( 'destroy', self.destroy )
			self.window.set_border_width( 10 )

			self.drawingArea.getWidget().set_size_request( 300, 300 )
			self.drawingArea.getWidget().show()

			self.window.add( self.drawingArea.getWidget() );
			self.window.show();


		def main(self):
			gtk.main()

	hello = HelloWorld()
	hello.main()




