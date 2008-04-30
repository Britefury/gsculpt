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

from copy import *


from Britefury.UI.ComboBox import ComboBox
from Britefury.UI import GSImage

from Britefury.Util.SignalSlot import *

from Britefury.Math import Math
from Britefury.Math.Math import Vector2i

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import CameraProjectionType

from Britefury.LogGrid import LogGrid
from Britefury.LogGrid.LogGrid import GridPlane

from Britefury.WorkArea import Viewport3d, DrawingArea
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter
from Britefury.WorkArea.DrawingArea import DrawingAreaSharingManager

from Britefury.View import View
from Britefury.View.View import ViewSettings




VIEW_REMOVE_FILENAME = 'red_x.png'



class VPType (object):
	def __init__(self, name):
		self.name = name

VPType.NONE = VPType( _( 'None' ) )
VPType.LEFT = VPType( _( 'Left' ) )
VPType.RIGHT = VPType( _( 'Right' ) )
VPType.BOTTOM = VPType( _( 'Bottom' ) )
VPType.TOP = VPType( _( 'Top' ) )
VPType.FRONT = VPType( _( 'Front' ) )
VPType.BACK = VPType( _( 'Back' ) )
VPType.ORTHOGRAPHIC = VPType( _( 'Orthographic' ) )
VPType.PERSPECTIVE = VPType( _( 'Perspective' ) )

VPType.viewportTypes = [ VPType.PERSPECTIVE, VPType.ORTHOGRAPHIC, VPType.FRONT, VPType.BACK, VPType.LEFT, VPType.RIGHT, VPType.TOP, VPType.BOTTOM ]




class WrappedViewport (object):
	class ViewportTypeMenu (object):
		viewportTypeSignal = ClassSignal()


		def __init__(self, typeToFactoryAndName, initialValue):
			self._combo = ComboBox()

			for viewportType in VPType.viewportTypes:
				factory, name = typeToFactoryAndName[viewportType]
				self._combo.append( viewportType, name )

			assert initialValue in VPType.viewportTypes
			self._combo.choice = initialValue

			self._combo.choiceSignal.connect( self._p_onChoice )


		def getWidget(self):
			return self._combo.getWidget()


		def _p_onChoice(self, event, choice):
			if event.bUserEvent:
				self.viewportTypeSignal.emit( choice )


	viewportTypeSignal = ClassSignal()
	closeSignal = ClassSignal()


	def __init__(self, viewport, viewportType, viewportTypesAndNames):
		self.viewport = viewport
		self.viewportType = viewportType

		# Viewport type menu
		self._viewportTypeMenu = WrappedViewport.ViewportTypeMenu( viewportTypesAndNames, viewportType )
		self.viewport.leftOptionsPanel.pack_start( self._viewportTypeMenu.getWidget() )

		self._viewportTypeMenu.viewportTypeSignal.connect( self._p_onViewportType )


		# Close button
		closeButton = gtk.Button()
		closeButton.add( GSImage.getGSImageWidget( VIEW_REMOVE_FILENAME ) )
		closeButton.connect( 'clicked', self._p_onClose )

		self._closeButtonBox = gtk.VBox()
		self._closeButtonBox.pack_start( closeButton, True, False, 1 )
		self._closeButtonBox.show_all()



	def destroy(self):
		self.viewport.leftOptionsPanel.remove( self._viewportTypeMenu.getWidget() )


	def getWidget(self):
		return self.viewport.getWidget()


	def showTypeMenu(self):
		self._viewportTypeMenu.getWidget().show()

	def hideTypeMenu(self):
		self._viewportTypeMenu.getWidget().hide()


	def showCloseButton(self):
		self.viewport.rightOptionsPanel.pack_end( self._closeButtonBox, False, False, 1 )

	def hideCloseButton(self):
		self.viewport.rightOptionsPanel.remove( self._closeButtonBox )


	def _p_onViewportType(self, viewportType):
		self.viewportTypeSignal.emit( self, viewportType )

	def _p_onClose(self, widget):
		self.closeSignal.emit( self )


	def __eq__(self, viewport):
		if isinstance( viewport, WrappedViewport ):
			return self is viewport
		elif isinstance( viewport, VPType ):
			return self.viewportType == viewport
		else:
			return self.viewport is viewport




def createViewportTypeToFactoryAndNameTable(sharingManager, backgroundImagePainter = None):
	typeToFactoryAndName = {}

	def perspective(viewSettings):
		vp = Viewport3d( viewSettings, sharingManager, None, backgroundImagePainter )
		vp.setCameraProjection( CameraProjectionType.PERSPECTIVE )
		vp.gridPlane = GridPlane.Y
		vp.name = _( 'Perspective' )
		return WrappedViewport( vp, VPType.PERSPECTIVE, typeToFactoryAndName )

	def orthographic(viewSettings):
		vp = Viewport3d( viewSettings, sharingManager, None, backgroundImagePainter )
		vp.setCameraProjection( CameraProjectionType.ORTHOGRAPHIC )
		vp.gridPlane = GridPlane.AUTO
		vp.front()
		vp.name = _( 'Orthographic' )
		return WrappedViewport( vp, VPType.ORTHOGRAPHIC, typeToFactoryAndName )

	def createDirectionalViewportFactory(directionMethod, gridPlane, viewportType, name):
		def factory(viewSettings):
			vp = Viewport3d( viewSettings, sharingManager, None, backgroundImagePainter )
			vp.setCameraProjection( CameraProjectionType.ORTHOGRAPHIC )
			vp.gridPlane = gridPlane
			directionMethod( vp )
			vp.bRotationEnabled = False
			vp.name = name
			return WrappedViewport( vp, viewportType, typeToFactoryAndName )
		return factory

	typeToFactoryAndName[VPType.FRONT] = createDirectionalViewportFactory( Viewport3d.front, GridPlane.Z, VPType.FRONT, _( 'Front' ) ), _( 'Front' )
	typeToFactoryAndName[VPType.BACK] = createDirectionalViewportFactory( Viewport3d.back, GridPlane.Z, VPType.BACK, _( 'Back' ) ), _( 'Back' )
	typeToFactoryAndName[VPType.LEFT] = createDirectionalViewportFactory( Viewport3d.left, GridPlane.X, VPType.LEFT, _( 'Left' ) ), _( 'Left' )
	typeToFactoryAndName[VPType.RIGHT] = createDirectionalViewportFactory( Viewport3d.right, GridPlane.X, VPType.RIGHT, _( 'Right' ) ), _( 'Right' )
	typeToFactoryAndName[VPType.BOTTOM] = createDirectionalViewportFactory( Viewport3d.bottom, GridPlane.Y, VPType.BOTTOM, _( 'Bottom' ) ), _( 'Bottom' )
	typeToFactoryAndName[VPType.TOP] = createDirectionalViewportFactory( Viewport3d.top, GridPlane.Y, VPType.TOP, _( 'Top' ) ), _( 'Top' )
	typeToFactoryAndName[VPType.PERSPECTIVE] = perspective, _( 'Perspective' )
	typeToFactoryAndName[VPType.ORTHOGRAPHIC] = orthographic, _( 'Orthographic' )

	return typeToFactoryAndName








class ModellerView (object):
	class ListenerList (object):
		def __init__(self, viewports, viewportAddListenerAttrName, viewportRemoveListenerAttrName):
			self._viewports = viewports
			self._listeners = []
			self._viewportAddListenerAttrName = intern( viewportAddListenerAttrName )
			self._viewportRemoveListenerAttrName = intern( viewportRemoveListenerAttrName )



		def addListener(self, listener, priority, viewportType):
			self._listeners = [ ( l, p, t )   for l, p, t in self._listeners   if l is not listener ]
			self._listeners.append( ( listener, priority, viewportType ) )

			for viewport in self._viewports:
				if isinstance( viewport, viewportType ):
					if priority is not None:
						getattr( viewport, self._viewportAddListenerAttrName )( listener, priority )
					else:
						getattr( viewport, self._viewportAddListenerAttrName )( listener )


		def removeListener(self, listener, viewportType):
			self._listeners = [ ( l, p, t )   for l, p, t in self._listeners   if l is not listener ]

			for viewport in self._viewports:
				if isinstance( viewport, viewportType ):
					getattr( viewport, self._viewportRemoveListenerAttrName )( listener )


		def viewportAdded(self, viewport):
			for listener, priority, viewportType in self._listeners:
				if isinstance( viewport, viewportType ):
					if priority is not None:
						getattr( viewport, self._viewportAddListenerAttrName )( listener, priority )
					else:
						getattr( viewport, self._viewportAddListenerAttrName )( listener )


		def viewportRemoved(self, viewport):
			for listener, priority, viewportType in self._listeners:
				if isinstance( viewport, viewportType ):
					getattr( viewport, self._viewportRemoveListenerAttrName )( listener )



	viewportAddedSignal = ClassSignal()
	viewportRemovedSignal = ClassSignal()


	def __init__(self):
		self._layout = None

		self._currentViewport = None
		self._pointerViewport = None

		self._box = gtk.HBox()

		self._viewports = []

		self._painters = ModellerView.ListenerList( self._viewports, 'addPainter', 'removePainter' )
		self._eventHandlers = ModellerView.ListenerList( self._viewports, 'addEventHandler', 'removeEventHandler' )
		self._scrollListeners = ModellerView.ListenerList( self._viewports, 'addScrollListener', 'removeScrollListener' )
		self._contextListeners = ModellerView.ListenerList( self._viewports, 'addContextButtonListener', 'removeContextButtonListener' )
		self._drawingTemplateListeners = ModellerView.ListenerList( self._viewports, 'addDrawingTemplate', 'removeDrawingTemplate' )

		self.viewSettings = ViewSettings()

		self._gridColours = None



	def getWidget(self):
		return self._box



	def setGridColours(self, gridColours):
		self._gridColours = gridColours

		if self._gridColours is not None:
			for v in self._viewports:
				if isinstance( v, Viewport3d ):
					v.setGridColours( gridColours )



	def postRedraw(self):
		for v in self._viewports:
			v.postRedraw()

	def forceRedraw(self):
		for v in self._viewports:
			v.forceRedraw()

	def forcePendingRedraw(self):
		for v in self._viewports:
			v.forcePendingRedraw()


	def orbitalCamera(self):
		for v in self._viewports:
			if isinstance( v, Viewport3d ):
				v.orbitalCamera()

	def firstPersonCamera(self):
		for v in self._viewports:
			if isinstance( v, Viewport3d ):
				v.firstPersonCamera()



	def addPainter(self, painter, viewportType):
		self._painters.addListener( painter, None, viewportType )

	def removePainter(self, painter, viewportType):
		self._painters.removeListener( painter, viewportType )


	def addEventHandler(self, eventHandler, viewportType, priority = 0):
		self._eventHandlers.addListener( eventHandler, priority, viewportType )

	def removeEventHandler(self, eventHandler, viewportType):
		self._eventHandlers.removeListener( eventHandler, viewportType )


	def addScrollListener(self, listener, viewportType, priority = 0):
		self._scrollListeners.addListener( listener, priority, viewportType )

	def removeScrollListener(self, listener, viewportType):
		self._scrollListeners.removeListener( listener, viewportType )


	def addContextButtonListener(self, listener, viewportType, priority = 0):
		self._contextListeners.addListener( listener, priority, viewportType )

	def removeContextButtonListener(self, listener, viewportType):
		self._contextListeners.removeListener( listener, viewportType )


	def addDrawingTemplate(self, listener, viewportType):
		self._drawingTemplateListeners.addListener( listener, None, viewportType )

	def removeDrawingTemplate(self, listener, viewportType):
		self._drawingTemplateListeners.removeListener( listener, viewportType )




	def _p_getCurrentViewport(self):
		if self._currentViewport is None:
			try:
				return self._viewports[0]
			except IndexError:
				return None
		else:
			return self._currentViewport

	def _p_setCurrentViewport(self, viewport):
		self._currentViewport = viewport
		if self._layout is not None:
			self._layout.setCurrentViewport( viewport )


	def _p_getPointerViewport(self):
		return self._pointerViewport


	def _p_getSize(self):
		return Vector2i( self._box.allocation.width, self._box.allocation.height )



	def _p_getLayout(self):
		return self._layout

	def _p_setLayout(self, layout):
		if layout is not self._layout:
			currentWrappedViewport = None
			viewports = []

			if self._layout is not None:
				currentWrappedViewport = self._layout.getCurrentWrappedViewport()
				self._layout.shutdown()
				self._box.remove( self._layout.getWidget() )
				self._layout.getWidget().hide()

				self._layout.viewportAddSignal.disconnect( self.onLayoutViewportAdd )
				self._layout.viewportRemoveSignal.disconnect( self.onLayoutViewportRemove )
				self._layout.viewportSelectSignal.disconnect( self.onLayoutViewportSelect )

			for viewport in self._viewports:
				self._p_viewportRemoved( viewport )
			self._p_setViewports( [] )

			self._layout = layout

			if self._layout is not None:
				viewports = self._layout.getViewports()
				for viewport in viewports:
					self._p_viewportAdded( viewport )
				self._p_setViewports( viewports )

				self._layout.viewportAddSignal.connect( self.onLayoutViewportAdd )
				self._layout.viewportRemoveSignal.connect( self.onLayoutViewportRemove )
				self._layout.viewportSelectSignal.connect( self.onLayoutViewportSelect )

				self._layout.initialise( currentWrappedViewport )
				self._layout.getWidget().show()
				self._box.pack_start( self._layout.getWidget() )

			for viewport in viewports:
				viewport.postRedraw()



	def _p_setViewports(self, viewports):
		self._viewports[:] = viewports

		if self._currentViewport not in self._viewports:
			self._currentViewport = None

		if self._pointerViewport not in self._viewports:
			self._pointerViewport = None





	def _p_viewportAdded(self, viewport):
		viewport.view = self

		viewport.enterSignal.connect( self.onEnter )
		viewport.leaveSignal.connect( self.onLeave )
		viewport.leaveUngrabSignal.connect( self.onLeaveUngrab )
		viewport.buttonDownSignal.connect( self.onButtonDown )
		viewport.scrollSignal.connect( self.onScroll )

		self._painters.viewportAdded( viewport )
		self._eventHandlers.viewportAdded( viewport )
		self._scrollListeners.viewportAdded( viewport )
		self._contextListeners.viewportAdded( viewport )
		self._drawingTemplateListeners.viewportAdded( viewport )

		self.viewportAddedSignal.emit( self, viewport )

		if self._gridColours is not None  and  isinstance( viewport, Viewport3d ):
			viewport.setGridColours( self._gridColours )


	def _p_viewportRemoved(self, viewport):
		self.viewportRemovedSignal.emit( self, viewport )

		viewport.enterSignal.disconnect( self.onEnter )
		viewport.leaveSignal.disconnect( self.onLeave )
		viewport.leaveUngrabSignal.disconnect( self.onLeaveUngrab )
		viewport.buttonDownSignal.disconnect( self.onButtonDown )
		viewport.scrollSignal.disconnect( self.onScroll )

		self._painters.viewportRemoved( viewport )
		self._eventHandlers.viewportRemoved( viewport )
		self._scrollListeners.viewportRemoved( viewport )
		self._contextListeners.viewportRemoved( viewport )
		self._drawingTemplateListeners.viewportRemoved( viewport )

		viewport.view = None



	def onEnter(self, viewport, position):
		self._pointerViewport = viewport

	def onLeave(self, viewport, position):
		self._pointerViewport = None

	def onLeaveUngrab(self, viewport):
		self._pointerViewport = None

	def onButtonDown(self, viewport, button):
		self._p_setCurrentViewport( viewport )

	def onScroll(self, viewport, x, y ):
		self._p_setCurrentViewport( viewport )



	def onLayoutViewportAdd(self, layout, viewport):
		assert viewport not in self._viewports
		self._p_viewportAdded( viewport )
		self._viewports.append( viewport )


	def onLayoutViewportRemove(self, layout, viewport):
		assert viewport in self._viewports
		self._p_viewportRemoved( viewport )
		self._viewports.remove( viewport )


	def onLayoutViewportSelect(self, layout, viewport):
		assert viewport in self._viewports
		self._p_setCurrentViewport( viewport )




	layout = property( _p_getLayout, _p_setLayout )
	currentViewport = property( _p_getCurrentViewport )
	pointerViewport = property( _p_getPointerViewport )
	size = property( _p_getSize )
	gridColours = property( None, setGridColours )










if __name__ == '__main__':
	from Britefury.GL.GL import *
	from Britefury.GL.GLU import *
	from Britefury.Math import Math
	from Britefury.Math.Math import Point2

	from Britefury.WorkArea.ViewLayoutSingle import ViewLayoutSingle
	from Britefury.WorkArea.ViewLayoutQuad import ViewLayoutQuad


	class HelloWorldBackgroundPainter (Viewport3dPainter):
		def evPaint3d(self, viewport, paintLayer):
			glBegin( GL_LINES )

			glColor3d( 1.0, 0.0, 0.0 )
			glVertex3d( 0.0, 0.0, 0.0 )
			glVertex3d( 100.0, 0.0, 0.0 )

			glColor3d( 0.0, 1.0, 0.0 )
			glVertex3d( 0.0, 0.0, 0.0 )
			glVertex3d( 0.0, 100.0, 0.0 )

			glColor3d( 0.0, 0.0, 1.0 )
			glVertex3d( 0.0, 0.0, 0.0 )
			glVertex3d( 0.0, 0.0, 100.0 )

			glEnd()



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
			backgroundPainter = HelloWorldBackgroundPainter()
			self.view = ModellerView()
			typeToFactoryAndName = createViewportTypeToFactoryAndNameTable( sharingManager, backgroundPainter )
			singleLayout = ViewLayoutSingle( self.view, typeToFactoryAndName )
			quadLayout = ViewLayoutQuad( self.view, typeToFactoryAndName )
			self.view.layout = singleLayout
			self.layouts = [ singleLayout, quadLayout ]

			self.window = gtk.Window( gtk.WINDOW_TOPLEVEL );
			self.window.connect( 'delete-event', self.delete_event )
			self.window.connect( 'destroy', self.destroy )
			self.window.set_border_width( 10 )

			self.view.getWidget().set_size_request( 640, 480 )
			self.view.getWidget().show()

			self.layoutCombo = gtk.combo_box_new_text()
			self.layoutCombo.append_text( "Single" )
			self.layoutCombo.append_text( "Quad" )
			self.layoutCombo.set_active( 0 )
			self.layoutCombo.connect( 'changed', self.onLayoutCombo )
			self.layoutCombo.show()

			self.box = gtk.VBox()
			self.box.pack_start( self.layoutCombo, False, False )
			self.box.pack_start( self.view.getWidget(), True, True )
			self.box.show()

			self.window.add( self.box );
			self.window.show();


		def onLayoutCombo(self, combo, data=None):
			index = combo.get_active()
			self.view.layout = self.layouts[index]


		def main(self):
			gtk.main()

	hello = HelloWorld()
	hello.main()


