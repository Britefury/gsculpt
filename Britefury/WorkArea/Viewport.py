##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk
import gtk.gdk

from Britefury.GL.GL import *
from Britefury.GL.GLU import *

from Britefury.Util.PriorityList import PriorityList
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Math.Math import Colour3f

from Britefury.WorkArea.DrawingArea import DrawingArea, DrawingAreaSharingManager

from Britefury.View import View
from Britefury.View.View import PaintLayer


VIEWPORT_BORDER_SIZE = 2
PREV_PAGE_BUTTON = 1
NEXT_PAGE_BUTTON = 3
PRIMARY_BUTTON = 1
SECONDARY_BUTTON = 2
CONTEXT_BUTTON = 3



CLEAR_COLOUR = Colour3f( 0.0, 0.0, 0.0 )



class ViewportEventHandler (object):
	"""Viewport event handler"""


	def evPrimaryButtonDown(self, viewport):
		"""Primary button pressed
		@viewport: viewport that sent the event"""
		pass

	def evPrimaryButtonDown2(self, viewport):
		"""Primary button double clicked
		@viewport: viewport that sent the event"""
		pass

	def evPrimaryButtonDown3(self, viewport):
		"""Primary button triple clicked
		@viewport: viewport that sent the event"""
		pass

	def evPrimaryButtonUp(self, viewport):
		"""Primary button released
		@viewport: viewport that sent the event"""
		pass

	def evSecondaryButtonDown(self, viewport):
		"""Secondary button pressed
		@viewport: viewport that sent the event"""
		pass

	def evSecondaryButtonDown2(self, viewport):
		"""Secondary button double clicked
		@viewport: viewport that sent the event"""
		pass

	def evSecondaryButtonDown3(self, viewport):
		"""Secondary button triple clicked
		@viewport: viewport that sent the event"""
		pass

	def evSecondaryButtonUp(self, viewport):
		"""Secondary button released
		@viewport: viewport that sent the event"""
		pass


	def evMotion(self, viewport, position):
		"""Motion event
		@viewport: viewport that sent the event
		@position: (Point2i) position of the pointer"""
		pass

	def evEnter(self, viewport, position):
		"""Enter event
		@viewport: viewport that sent the event
		@position: (Point2i) position of the pointer"""
		pass

	def evLeave(self, viewport, position):
		"""Leave event
		@viewport: viewport that sent the event
		@position: (Point2i) position of the pointer"""
		pass

	def evLeaveUngrab(self, viewport):
		"""Leave-ungrab event
		@viewport: viewport that sent the event"""
		pass


	def evConfig(self, viewport, size):
		"""Configure (resize) event
		@viewport: viewport that sent the event
		@size: (Vector2i) the new size of the drawing area"""
		pass


	def evRealise(self, viewport):
		"""Realise
		@viewport: viewport that sent the event"""
		pass

	def evUnrealise(self, viewport):
		"""Unrealise
		@viewport: viewport that sent the event"""
		pass


	def evWorldToScreenChanged(self, viewport):
		"""World to screen transformation changed
		@viewport: viewport that sent the event"""
		pass



	def eventHandlerAttachedToViewport(self, viewport):
		"""Attached to a viewport
		@viewport: viewport to attach to"""
		if viewport.pointerStatus.bInsideBounds:
			self.evEnter( viewport, viewport.pointerStatus.position )

		# Don't emit button events if buttons are held down as this can cause problems


	def eventHandlerDetachedFromViewport(self, viewport):
		"""Detached from a viewport
		@viewport: viewport to detach from"""
		pointerStatus = viewport.pointerStatus

		if pointerStatus.bInsideBounds:
			self.evLeave( viewport, pointerStatus.position )

		# Emit button up events if buttons are pressed
		if pointerStatus.isButtonPressed( 1 ):
			self.evPrimaryButtonUp( viewport )

		if pointerStatus.isButtonPressed( 2 ):
			self.evSecondaryButtonUp( viewport )




class ViewportNavigationListener (object):
	"""Viewport navigation listener"""
	def evButtonDown(self, viewport, button):
		"""Button pressed
		@viewport: viewport that sent the event
		@button: button index"""
		assert False, 'abstract'

	def evButtonUp(self, viewport, button):
		"""Button released
		@viewport: viewport that sent the event
		@button: button index"""
		assert False, 'abstract'

	def evMotion(self, viewport, position):
		"""Motion event
		@viewport: viewport that sent the event
		@position: position of the pointer"""
		assert False, 'abstract'

	def evLeaveUngrab(self, viewport):
		"""Leave ungrab event
		@viewport: viewport that sent the event"""
		assert False, 'abstract'




class ViewportPageSwitchListener (object):
	"""Viewport page switch listener"""
	def evPrevPage(self, viewport):
		"""Previous page event
		@viewport: viewport that sent the event"""
		assert False, 'abstract'

	def evNextPage(self, viewport):
		"""Next page event
		@viewport: viewport that sent the event"""
		assert False, 'abstract'




class ViewportScrollListener (object):
	"""Viewport scroll listener"""
	def evScroll(self, viewport, x, y):
		"""Scroll event
		@viewport: viewport that sent the event
		@x: scroll-x
		@y: scroll-y"""
		assert False, 'abstract'




class ViewportContextButtonListener (object):
	"""Viewport context button listener"""
	def evContextButton(self, viewport):
		"""Context button pressed
		@viewport: viewport that sent the event"""
		assert False, 'abstract'




class ViewportPainter (object):
	"""Viewport painter"""
	def painterAttachedToViewport(self, viewport):
		"""Attached to viewport notify
		@viewport: viewport that this painter has been attached to"""
		pass

	def painterDetachedFromViewport(self, viewport):
		"""Detached from viewport - notify
		@viewport: viewport that this painter has been detached from"""
		pass



class Viewport (object):
	enterSignal = ClassSignal()
	leaveSignal = ClassSignal()
	leaveUngrabSignal = ClassSignal()
	buttonDownSignal = ClassSignal()
	scrollSignal = ClassSignal()


	"""Viewport"""
	def __init__(self, sharingManager):
		"""Initialise viewport
		@sharingManager: a @DrawingAreaSharingManager; the resource sharing manager"""
		global VIEWPORT_BORDER_SIZE


		# Options panel
		self.leftOptionsPanel = gtk.HBox()
		self.leftOptionsPanel.show()

		self.centreOptionsPanel = gtk.HBox()
		self.centreOptionsPanel.show()

		self.rightOptionsPanel = gtk.HBox()
		self.rightOptionsPanel.show()

		optionsPanel = gtk.HBox()
		optionsPanel.pack_start( self.leftOptionsPanel, expand=False, fill=False )
		optionsPanel.pack_start( self.centreOptionsPanel, expand=False, fill=False )
		optionsPanel.pack_start( self.rightOptionsPanel, expand=True, fill=True )
		optionsPanel.show()


		# Drawing area
		self._drawingArea = DrawingArea( sharingManager )
		self._drawingArea.getWidget().show()
		self._drawingArea.setListener( self )


		# Viewport box
		viewportBox = gtk.VBox()
		viewportBox.pack_start( optionsPanel, expand=False, fill=False )
		viewportBox.pack_start( self._drawingArea.getWidget(), expand=True, fill=True )
		viewportBox.show()


		# Viewport box wrapper
		viewportBoxWrapper = gtk.EventBox()
		viewportBoxWrapper.set_border_width( VIEWPORT_BORDER_SIZE )
		viewportBoxWrapper.add( viewportBox )
		viewportBoxWrapper.show()


		# Viewport event box
		self._viewportEventBox = gtk.EventBox()
		self._viewportEventBox.add( viewportBoxWrapper )
		self._viewportEventBox.set_app_paintable( True )
		self._viewportEventBox.connect( 'realize', self._p_onEventBoxRealise )
		self._viewportEventBox.connect( 'unrealize', self._p_onEventBoxUnrealise )
		self._viewportEventBox.connect( 'expose-event', self._p_onEventBoxExpose )



		self._view = None
		self._bEventBoxRealised = False
		self._bHighlighted = False

		self.navigationListener = None
		self.pageSwitchListener = None
		self._painters = []
		self._drawingTemplates = set()
		self._eventHandlers = PriorityList()
		self._scrollListeners = PriorityList()
		self._contextListeners = PriorityList()


		self._highlightColour = None
		self._highlightGC = None


		self.name = ''




	def addPainter(self, painter):
		"""Add a painter
		@painter: the painter to add"""
		bInList = painter in self._painters
		if bInList:
			self._painters.remove( painter )

		self._painters.append( painter )

		if not bInList:
			painter.painterAttachedToViewport( self )
		self.postRedraw()

	def removePainter(self, painter):
		"""Remove a painter
		@painter: the painter to remove"""
		if painter in self._painters:
			self._painters.remove( painter )
			painter.painterDetachedFromViewport( self )
			self.postRedraw()



	def addEventHandler(self, handler, priority):
		"""Add an event handler
		@handler: the handler to add
		@priority: the priority"""
		oldHandler = self._eventHandlers.getTop()
		self._eventHandlers.add( handler, priority )
		newHandler = self._eventHandlers.getTop()

		if oldHandler is not newHandler:
			if oldHandler is not None:
				oldHandler.eventHandlerDetachedFromViewport( self )
			if newHandler is not None:
				newHandler.eventHandlerAttachedToViewport( self )


	def removeEventHandler(self, handler):
		"""Remove an event handler
		@handler: handler to remove"""
		oldHandler = self._eventHandlers.getTop()
		self._eventHandlers.remove( handler )
		newHandler = self._eventHandlers.getTop()

		if oldHandler is not newHandler:
			if oldHandler is not None:
				oldHandler.eventHandlerDetachedFromViewport( self )
			if newHandler is not None:
				newHandler.eventHandlerAttachedToViewport( self )



	def addScrollListener(self, listener, priority):
		"""Add a scroll listener
		@listener: listener to add
		@priority: the priority"""
		self._scrollListeners.add( listener, priority )


	def removeScrollListener(self, listener):
		"""Remove a scroll listener
		@listener: listener to remove"""
		self._scrollListeners.remove( listener )



	def addContextButtonListener(self, listener, priority):
		"""Add a context button listener
		@listener: listener to add
		@priority: the priority"""
		self._contextListeners.add( listener, priority )


	def removeContextButtonListener(self, listener):
		"""Remove a context button listener
		@listener: listener to remove"""
		self._contextListeners.remove( listener )



	def addDrawingTemplate(self, drawingTemplate):
		"""Add a painter
		@painter: the painter to add"""
		self._drawingTemplates.add( drawingTemplate )

	def removeDrawingTemplate(self, drawingTemplate):
		"""Remove a painter
		@painter: the painter to remove"""
		try:
			self._drawingTemplates.remove( drawingTemplate )
		except KeyError:
			pass




	def getWidget(self):
		"""Get the overall widget"""
		return self._viewportEventBox

	def getViewportWidget(self):
		"""Get the viewport widget"""
		return self._drawingArea.getWidget()



	def freeze(self):
		self._drawingArea.freeze()

	def thaw(self):
		self._drawingArea.thaw()


	def beginGL(self):
		"""Begin OpenGL drawing"""
		self._drawingArea.beginGL()

	def endGL(self):
		"""End OpenGL drawing"""
		self._drawingArea.endGL()


	def glProjectionIdentity(self):
		"""Load an identity projection matrix"""
		size = self._drawingArea.size
		glLoadIdentity()
		gluOrtho2D( 0.0, size.x, 0.0, size.y )

	def glModelViewIdentity(self):
		"""Load an identity model-view matrix"""
		glLoadIdentity()



	def highlight(self):
		"""Enable highlight"""
		self._bHighlighted = True
		self._p_eventBoxRedraw()

	def unhighlight(self):
		"""Disable highlight"""
		self._bHighlighted = False
		self._p_eventBoxRedraw()



	def createPick(self, screenPoint = None):
		"""Create a pick for the given screen point (a Point2)"""
		if screenPoint is None:
			screenPoint = self._drawingArea.pointerStatus.position.toPoint2()
		return self._p_createPick( screenPoint )


	def _p_createPick(self, screenPoint):
		"""Private - Create a pick for the given screen point (a Point2)"""
		assert False, 'abstract'



	def postRedraw(self):
		"""Post a redraw request"""
		self._drawingArea.postRedraw()

	def forceRedraw(self):
		"""Force a redraw"""
		self._drawingArea.forceRedraw()

	def forcePendingRedraw(self):
		"""Force any pending redraw"""
		self._drawingArea.forcePendingRedraw()




	def getLeftOptionsPanel(self):
		"""Get the left options panel"""
		return self._leftOptionsPanel

	def _p_getCentreOptionsPanel(self):
		"""Get the centre options panel"""
		return self._centreOptionsPanel

	def getRightOptionsPanel(self):
		"""Get the right options panel"""
		return self._rightOptionsPanel






	def evPaint(self, drawingArea):
		glMatrixMode( GL_MODELVIEW );
		glLoadIdentity();

		glClearColor( CLEAR_COLOUR.r, CLEAR_COLOUR.g, CLEAR_COLOUR.b, 0.0 );
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glEnable( GL_NORMALIZE );

		glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );


		self.evPaintPreInit()
		self._p_initGLMatrices()
		self.evPaintPostInit()


	def evPaintPreInit(self):
		pass

	def evPaintPostInit(self):
		pass



	def evButtonDown(self, drawingArea, button):
		global PREV_PAGE_BUTTON, NEXT_PAGE_BUTTON, PRIMARY_BUTTON, SECONDARY_BUTTON, CONTEXT_BUTTON
		self.buttonDownSignal.emit( self, button )
		pstat = self.pointerStatus

		if pstat.bAltKey:
			if pstat.bShiftKey:
				if self.pageSwitchListener is not None:
					if button == PREV_PAGE_BUTTON:
						self.pageSwitchListener.evPrevPage( self )
					elif button == NEXT_PAGE_BUTTON:
						self.pageSwitchListener.evNextPage( self )
			else:
				if self.navigationListener is not None:
					self.navigationListener.evButtonDown( self, button )
		else:
			if button == PRIMARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evPrimaryButtonDown( self )
			elif button == SECONDARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evSecondaryButtonDown( self )
			elif button == CONTEXT_BUTTON:
				listener = self._contextListeners.getTop()
				if listener is not None:
					listener.evContextButton( self )


	def evButtonDown2(self, drawingArea, button):
		global PRIMARY_BUTTON, SECONDARY_BUTTON
		pstat = self.pointerStatus

		if not pstat.bAltKey:
			if button == PRIMARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evPrimaryButtonDown2( self )
			elif button == SECONDARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evSecondaryButtonDown2( self )


	def evButtonDown3(self, drawingArea, button):
		global PRIMARY_BUTTON, SECONDARY_BUTTON
		pstat = self.pointerStatus

		if not pstat.bAltKey:
			if button == PRIMARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evPrimaryButtonDown3( self )
			elif button == SECONDARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evSecondaryButtonDown3( self )


	def evButtonUp(self, drawingArea, button):
		global PRIMARY_BUTTON, SECONDARY_BUTTON
		pstat = self.pointerStatus

		if self.navigationListener is not None:
			self.navigationListener.evButtonUp( self, button )

		if not pstat.bAltKey:
			if button == PRIMARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evPrimaryButtonUp( self )
			elif button == SECONDARY_BUTTON:
				handler = self._eventHandlers.getTop()
				if handler is not None:
					handler.evSecondaryButtonUp( self )


	def evMotion(self, drawingArea, position):
		if self.navigationListener is not None:
			self.navigationListener.evMotion( self, position )

		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evMotion( self, position )


	def evEnter(self, drawingArea, position):
		self.enterSignal.emit( self, position )
		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evEnter( self, position )


	def evLeave(self, drawingArea, position):
		self.leaveSignal.emit( self, position )
		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evLeave( self, position )


	def evLeaveUngrab(self, drawingArea):
		self.leaveUngrabSignal.emit( self )
		if self.navigationListener is not None:
			self.navigationListener.evLeaveUngrab( self )

		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evLeaveUngrab( self )


	def evScroll(self, drawingArea, x, y):
		self.scrollSignal.emit( self, x, y )
		listener = self._scrollListeners.getTop()
		if listener is not None:
			listener.evScroll( self, x, y )


	def evConfig(self, drawingArea, size):
		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evConfig( self, size )


	def evRealise(self, drawingArea):
		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evRealise( self )


	def evUnrealise(self, drawingArea):
		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evUnrealise( self )



	def _p_initGLMatrices(self):
		pass


	def _p_eventBoxRedraw(self):
		if self._bEventBoxRealised:
			rect = gtk.gdk.Rectangle()
			rect.x = rect.y = 0
			rect.width = self._viewportEventBox.allocation.width
			rect.height = self._viewportEventBox.allocation.height
			self._viewportEventBox.window.invalidate_rect( rect, False )


	def _p_onEventBoxRealise(self, widget):
		if not self._bEventBoxRealised:
			self._highlightGC = gtk.gdk.GC( self._viewportEventBox.window )

			state = self._viewportEventBox.state
			self._highlightGC.copy( self._viewportEventBox.get_style().bg_gc[state] )

			self._highlightColour = gtk.gdk.colormap_get_system().alloc_color( 0x0, 0x0, 0xffff, )

			self._highlightGC.set_foreground( self._highlightColour )
			self._highlightGC.set_background( self._highlightColour )

			self._bEventBoxRealised = True


	def _p_onEventBoxUnrealise(self, widget):
		self._bEventBoxRealised = False
		self._highlightGC = None
		self._highlightColour = None


	def _p_onEventBoxExpose(self, widget, event):
		global VIEWPORT_BORDER_WIDTH

		if self._bEventBoxRealised:
			if self._bHighlighted:
				width = self._viewportEventBox.allocation.width
				height = self._viewportEventBox.allocation.height
				window = self._viewportEventBox.window

				window.draw_rectangle( self._highlightGC, True, 0, 0, width, VIEWPORT_BORDER_SIZE )
				window.draw_rectangle( self._highlightGC, True, 0, height - VIEWPORT_BORDER_SIZE, width, VIEWPORT_BORDER_SIZE )
				window.draw_rectangle( self._highlightGC, True, 0, VIEWPORT_BORDER_SIZE, VIEWPORT_BORDER_SIZE, height - VIEWPORT_BORDER_SIZE * 2 )
				window.draw_rectangle( self._highlightGC, True, width - VIEWPORT_BORDER_SIZE, VIEWPORT_BORDER_SIZE,
														VIEWPORT_BORDER_SIZE, height - VIEWPORT_BORDER_SIZE * 2 )




	def _p_getView(self):
		"""Private - Get the view"""
		return self._view

	def _p_setView(self, view):
		"""Private - Set the view
		@view: the view"""
		self._view = view


	def _p_getPointerStatus(self):
		"""Private - Get the pointer status"""
		return self._drawingArea.pointerStatus


	def _p_isRealised(self):
		"""Private - Check if the viewport is realised"""
		return self._drawingArea.isRealised()


	def _p_getSize(self):
		"""Private - Get the viewport size"""
		return self._drawingArea.size


	view = property( _p_getView, _p_setView )
	pointerStatus = property( _p_getPointerStatus )
	bIsRealised = property( _p_isRealised )
	size = property( _p_getSize )






if __name__ == '__main__':
	from Britefury.GL.GLU import *
	from Britefury.Math import Math
	from Britefury.Math.Math import Point2
	from Britefury.Graphics import Graphics
	from Britefury.Graphics.Graphics import glDrawString2


	class HelloWorldListener (ViewportEventHandler):
		def evPrimaryButtonDown(self, viewport):
			print 'evPrimaryButtonDown'

		def evPrimaryButtonUp(self, viewport):
			print 'evPrimaryButtonUp'

		def evMotion(self, viewport, position):
			print 'evMotion %d,%d' % ( position.x, position.y )

		def evEnter(self, viewport, position):
			print 'evEnter %d,%d' % ( position.x, position.y )

		def evLeave(self, viewport, position):
			print 'evLeave %d,%d' % ( position.x, position.y )

		def evScroll(self, viewport, x, y):
			print 'evScroll %d, %d' % ( x, y )


		def evPaint(self, viewport):
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

			glColor3d( 1.0, 0.0, 0.0 )
			glDrawString2( Point2( 0.0, 0.0 ), 'Hello world' )



	class HelloWorld (object):
		def delete_event(self, widget, event, data=None):
			print( "delete_event" )
			return False

		def destroy(self, widget, data=None):
			gtk.main_quit()

		def onValue(self,value):
			pass




		def __init__(self):
			sharingManager = DrawingAreaSharingManager()
			self.listener = HelloWorldListener()
			self.viewport = Viewport( sharingManager )

			self.viewport.addEventHandler( self.listener, 0 )
			def paintPostInit(viewport):
				self.listener.evPaint()
			self.viewport.evPaintPostInit = paintPostInit
			self.viewport.highlight()

			self.window = gtk.Window( gtk.WINDOW_TOPLEVEL );
			self.window.connect( 'delete-event', self.delete_event )
			self.window.connect( 'destroy', self.destroy )
			self.window.set_border_width( 10 )

			self.viewport.getWidget().set_size_request( 300, 300 )
			self.viewport.getWidget().show()

			self.window.add( self.viewport.getWidget() );
			self.window.show();


		def main(self):
			gtk.main()

	hello = HelloWorld()
	hello.main()



