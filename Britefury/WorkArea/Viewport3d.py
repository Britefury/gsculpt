##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

from Britefury.GL.GL import *
from Britefury.GL.GLU import *

from math import radians

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Math import Math
from Britefury.Math.Math import Point2i, Point2, Vector2i, Vector2

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.UI.ToggleButton import ToggleButton
from Britefury.UI.ComboBox import ComboBox
from Britefury.UI.PopupWindow import PopupWindow
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI import GSImage

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import Camera
from Britefury.Graphics.Graphics import OrbitalCameraNavigation, FirstPersonCameraNavigation
from Britefury.Graphics.Graphics import CameraPosition
from Britefury.Graphics.Graphics import PerspectiveCameraProjection, OrthographicCameraProjection, CameraProjectionType

from Britefury.LogGrid import LogGrid
from Britefury.LogGrid.LogGrid import ModellerGrid, GridPlane

from Britefury.WorkArea.DrawingArea import DrawingArea, DrawingAreaSharingManager
from Britefury.WorkArea.Viewport import *

from Britefury.View import View
from Britefury.View.View import Viewport3dSettings, PaintLayer

from Britefury.Model.Pick3d import Pick3d



class Viewport3dPainter (ViewportPainter):
	def evPaint3d(self, viewport, paintLayer):
		pass





class Viewport3d (Viewport):
	LIGHTINGMODE_SINGLE = 1
	LIGHTINGMODE_QUAD = 2

	gridEnableSignal = ClassSignal()


	def __init__(self, viewSettings, sharingManager, toolbox, backgroundImagePainter = None):
		super( Viewport3d, self ).__init__( sharingManager )

		self._cameraPosition = CameraPosition()
		self._perspectiveCamProjection = PerspectiveCameraProjection( 0.03, 500.0 )
		self._orthographicCamProjection = OrthographicCameraProjection( -1.0, 500.0 )

		self.camera = Camera( 640.0, 480.0, radians( 60.0 ) )
		self.camera.cameraPosition = self._cameraPosition
		self.camera.cameraProjection = self._perspectiveCamProjection

		self._orbitalCamNavigation = OrbitalCameraNavigation()
		self._firstPersonCamNavigation = FirstPersonCameraNavigation()
		self._camNavigation = self._orbitalCamNavigation
		self._camNavigation.rotate( self._cameraPosition, 0.0, radians( -20.0 ) )

		self.navigationListener = Viewport3dNavigationListener()

		self._grid = ModellerGrid()

		self._bBackgroundImagesVisible = True
		self._backgroundImagePainter = backgroundImagePainter
		if self._backgroundImagePainter is not None  and  self._bBackgroundImagesVisible:
			self.addPainter( self._backgroundImagePainter )

		self.viewportSettings = Viewport3dSettings( viewSettings, self.camera )

		self._lightingMode = self.LIGHTINGMODE_SINGLE


		tooltips = gtk.Tooltips()



		viewMenuOriginItem = gtk.MenuItem( _( 'Origin' ) )
		viewMenuOriginItem.connect( 'activate', self._p_onOrigin )
		viewMenuZ0Item = gtk.MenuItem( _( 'Depth=0' ) )
		viewMenuZ0Item.connect( 'activate', self._p_onZ0 )
		viewMenuOneToOneItem = gtk.MenuItem( _( '1:1' ) )
		viewMenuOneToOneItem.connect( 'activate', self._p_onOneToOne )
		self.viewMenu = gtk.Menu()
		self.viewMenu.append( viewMenuOriginItem )
		self.viewMenu.append( viewMenuZ0Item )
		self.viewMenu.append( viewMenuOneToOneItem )
		self.viewMenu.show_all()

		tooltips.set_tip( viewMenuOriginItem, _( 'Focus on origin' ), _( 'Focus on origin' ) )
		tooltips.set_tip( viewMenuZ0Item, _( 'Set depth to 0' ), _( 'Set depth to 0' ) )
		tooltips.set_tip( viewMenuOneToOneItem, _( 'Zoom: 1 to 1' ), _( 'Zoom: 1 to 1' ) )

		viewMenuButtonLabel = gtk.Label( _( 'View' ) )
		viewMenuButtonArrow = gtk.Arrow( gtk.ARROW_DOWN, gtk.SHADOW_NONE )
		viewMenuContents = gtk.HBox()
		viewMenuContents.pack_start( viewMenuButtonLabel, False, False )
		viewMenuContents.pack_start( viewMenuButtonArrow, False, False )
		viewMenuButton = gtk.Button()
		viewMenuButton.add( viewMenuContents )
		viewMenuButton.connect( 'button-press-event', self._p_onViewMenuButton )

		self._gridPlaneCombo = ComboBox( [ gtk.gdk.Pixbuf ], [ ( GridPlane.X, GSImage.getGSImagePixbuf( 'gridx.png' ) ),
													( GridPlane.Y, GSImage.getGSImagePixbuf( 'gridy.png' ) ),
													( GridPlane.Z, GSImage.getGSImagePixbuf( 'gridz.png' ) ),
													( GridPlane.AUTO, GSImage.getGSImagePixbuf( 'grida.png' ) ) ] )
		self._gridPlaneCombo.choiceSignal.connect( self._p_onGridPlane )
		self._gridPlaneCombo.choice = self._grid.gridPlane

		tooltips.set_tip( self._gridPlaneCombo.getWidget(), _( 'Choose grid plane' ), _( 'Choose grid plane' ) )

		gridPlaneToggle = gtk.ToggleButton()
		gridPlaneToggle.add( GSImage.getGSImageWidget( 'grid_toggle.png' ) )
		gridPlaneToggle.show_all()
		gridPlaneToggle.set_active( self._grid.bEnabled )
		gridPlaneToggle.connect( 'clicked', self._p_onGridPlaneToggle )

		tooltips.set_tip( gridPlaneToggle, _( 'Enable grid plane' ), _( 'Enable grid plane' ) )


		self._renderingButton = gtk.Button()
		self._renderingButton.add( GSImage.getGSImageWidget( 'rendering_small.png' ) )
		self._renderingButton.show_all()

		self._renderingButton.connect( 'clicked', self._p_onRenderingButton )

		tooltips.set_tip( self._renderingButton, _( 'Viewport rendering settings' ), _( 'Viewport rendering settings' ) )


		def makeRenderFlagButton(gsImageFilename, attrName, tooltip):
			def onToggle(event, state):
				setattr( self.viewportSettings, attrName, state )
				self.postRedraw()

			button = ToggleButton( gsImageFilename=gsImageFilename )
			button.state = getattr( self.viewportSettings, attrName )
			button.toggleSignal.connect( onToggle )
			button.getWidget().show()
			tooltips.set_tip( button.getWidget(), tooltip, tooltip )
			return button.getWidget()

		backgroundImagesButton = ToggleButton( gsImageFilename='draw_backgroundimages.png' )
		backgroundImagesButton.state = self._bBackgroundImagesVisible
		backgroundImagesButton.toggleSignal.connect( self._p_onBackgroundImagesToggle )
		backgroundImagesButton.getWidget().show()

		renderingPopupLayout = FormLayout( bHomogeneous=False )
		renderingPopupLayout.row()  <<  _( 'Foreground:' )														\
			<<  makeRenderFlagButton( 'draw_vertices.png', 'bForegroundVertices', _( 'Draw vertices' ) )					\
			<<  makeRenderFlagButton( 'draw_wireframe.png', 'bForegroundWireframe', _( 'Draw wireframe' ) )				\
			<<  makeRenderFlagButton( 'draw_solid.png', 'bForegroundSolid', _( 'Draw solid' ) )							\
			<<  makeRenderFlagButton( 'draw_smooth.png', 'bForegroundSmooth', _( 'Smooth' ) )						\
			<<  makeRenderFlagButton( 'draw_markedfaces.png', 'bForegroundMarkedFaces', _( 'Draw marked faces' ) )
		renderingPopupLayout.row()  <<  _( 'Background:' )														\
			<<  None																					\
			<<  makeRenderFlagButton( 'draw_wireframe.png', 'bBackgroundWireframe', _( 'Draw wireframe' ) )			\
			<<  makeRenderFlagButton( 'draw_solid.png', 'bBackgroundSolid', _( 'Draw solid' ) )							\
			<<  makeRenderFlagButton( 'draw_smooth.png', 'bBackgroundSmooth', _( 'Smooth' ) )						\
			<<  makeRenderFlagButton( 'draw_backg_transparent.png', 'bBackgroundTransparent', _( 'Transparent' ) )
		renderingPopupLayout.row()  <<  _( 'Background images:' )												\
			<<  backgroundImagesButton.getWidget()															\
			<<  None																					\
			<<  None																					\
			<<  None																					\
			<<  None

		renderingPopupLayout.finalise()
		renderingPopupLayout.getWidget().show()

		self._renderingPopup = PopupWindow()
		self._renderingPopup.contents = renderingPopupLayout.getWidget()



		self.centreOptionsPanel.pack_start( gtk.VSeparator(), False, False, 10 )
		self.centreOptionsPanel.pack_start( viewMenuButton )
		self.centreOptionsPanel.pack_start( gtk.VSeparator(), False, False, 10 )
		self.centreOptionsPanel.pack_start( self._gridPlaneCombo.getWidget() )
		self.centreOptionsPanel.pack_start( gridPlaneToggle )
		self.centreOptionsPanel.pack_start( gtk.VSeparator(), False, False, 10 )
		self.centreOptionsPanel.pack_start( self._renderingButton )
		self.centreOptionsPanel.show_all()




	def setGridColours(self, gridColours):
		self._grid.colours = gridColours




	def _p_createPick(self, screenPoint):
		"""Private - Create a pick for the given screen point (a Point2)"""
		return Pick3d( screenPoint, deepcopy( self.camera.getProjection() ), False, self.viewportSettings.bForegroundSolid )	# HACK, get real value for bBackfaceCulling


	def computeDrawPoint(self, screenPoint=None, bNoneIfNoHit=False):
		if screenPoint is None:
			screenPoint = self._drawingArea.pointerStatus.positionF
		if len( self._drawingTemplates ) > 0:
			ray = self.computeRay( screenPoint )
			drawPoint = None
			for drawingTemplate in self._drawingTemplates:
				bSuccess, t, intersection, clippedRay = drawingTemplate.templateDrawingPoint3d( ray, False )			# HACK, get real value for bBackfaceCulling
				if bSuccess:
					drawPoint = intersection
					ray = clippedRay
			if drawPoint is not None:
				return drawPoint
		if bNoneIfNoHit:
			return None
		else:
			return self.camera.transformScreenAtTargetDistanceToWorld( screenPoint )


	def raytraceDrawingTemplates(self, ray):
		drawPoint = None
		for drawingTemplate in self._drawingTemplates:
			bSuccess, t, intersection, clippedRay = drawingTemplate.templateDrawingPoint3d( ray, False )			# HACK, get real value for bBackfaceCulling
			if bSuccess:
				drawPoint = intersection
				ray = clippedRay
		return drawPoint



	def computeRay(self, screenPoint=None):
		if screenPoint is None:
			screenPoint = self._drawingArea.pointerStatus.positionF
		return self.camera.computeRay( screenPoint )


	def transformWorldToScreen(self, world):
		return self.camera.transformWorldToScreen( world )


	def setScreenDimensions(self, dimensions):
		self.camera.setScreenDimensions( dimensions.x, dimensions.y )
		self._p_emitWorldToScreenChangedEvent()


	def _p_getRotationEnabled(self):
		return self.navigationListener.bRotationEnabled

	def _p_setRotationEnabled(self, value):
		self.navigationListener.bRotationEnabled = value


	def setCameraProjection(self, projectionType):
		projection = None

		if projectionType == CameraProjectionType.PERSPECTIVE:
			projection = self._perspectiveCamProjection
		elif projectionType == CameraProjectionType.ORTHOGRAPHIC:
			projection = self._orthographicCamProjection

		if projection is not None:
			self.camera.cameraProjection = projection
			self._p_emitWorldToScreenChangedEvent()


	def orbitalCamera(self):
		self._camNavigation = self._orbitalCamNavigation

	def firstPersonCamera(self):
		self._camNavigation = self._firstPersonCamNavigation



	def front(self):
		self._cameraPosition.front()
		self._p_emitWorldToScreenChangedEvent()

	def back(self):
		self._cameraPosition.back()
		self._p_emitWorldToScreenChangedEvent()

	def top(self):
		self._cameraPosition.top()
		self._p_emitWorldToScreenChangedEvent()

	def bottom(self):
		self._cameraPosition.bottom()
		self._p_emitWorldToScreenChangedEvent()

	def left(self):
		self._cameraPosition.left()
		self._p_emitWorldToScreenChangedEvent()

	def right(self):
		self._cameraPosition.right()
		self._p_emitWorldToScreenChangedEvent()


	def rotate(self, x, y):
		self._camNavigation.rotate( self._cameraPosition, x, y )
		self._p_emitWorldToScreenChangedEvent()

	def pan(self, panVector):
		self._camNavigation.pan( self._cameraPosition, panVector )
		self._p_emitWorldToScreenChangedEvent()

	def moveInZ(self, screenZ, worldZ):
		self._camNavigation.moveInZ( self._cameraPosition, screenZ, worldZ )
		self._p_emitWorldToScreenChangedEvent()


	def origin(self):
		self._cameraPosition.origin();
		self._p_emitWorldToScreenChangedEvent()

	def viewZ0(self):
		self._cameraPosition.viewZ0();
		self._p_emitWorldToScreenChangedEvent()

	def oneToOne(self):
		self.camera.oneToOne();
		self._p_emitWorldToScreenChangedEvent()


	def focusOn(self, point):
		translation = point - self._cameraPosition.targetPosition
		self._cameraPosition.translate( translation )
		self._p_emitWorldToScreenChangedEvent()


	def _p_getGridPlane(self):
		return self._grid.gridPlane

	def _p_setGridPlane(self, gridPlane):
		self._grid.gridPlane = gridPlane
		self._gridPlaneCombo.choice = gridPlane




	def evConfig(self, drawingArea, size):
		super( Viewport3d, self ).evConfig( drawingArea, size )
		self.setScreenDimensions( size.toVector2() )


	def evPaint(self, drawingArea):
		super( Viewport3d, self ).evPaint( drawingArea )

		glEnable( GL_DEPTH_TEST )
		glDepthFunc( GL_LEQUAL )

		#generate events
		#foreground layer
		self._grid.paint3d( self.camera )
		self._p_emitPaintEvent( PaintLayer.PAINTLAYER_FOREGROUND )


		#transparency layer
		glEnable( GL_BLEND )
		glBlendFunc( GL_ONE, GL_SRC_ALPHA )
		glDisable( GL_DEPTH_TEST )
		glDepthMask( False )

		self._p_emitPaintEvent( PaintLayer.PAINTLAYER_TRANSPARENCY )


		#wireframe layer
		glDepthMask( GL_TRUE )
		glEnable( GL_DEPTH_TEST )
		glDisable( GL_BLEND )

		self._p_emitPaintEvent( PaintLayer.PAINTLAYER_WIREFRAME )


		#blend overlay layer
		glEnable( GL_BLEND )
		glBlendFunc( GL_ONE, GL_SRC_ALPHA )

		self._p_emitPaintEvent( PaintLayer.PAINTLAYER_OVERLAY_BLEND )


		#no-depth overlay layer
		glDisable( GL_DEPTH_TEST )
		glDepthMask( GL_FALSE )
		glDisable( GL_BLEND )

		self._p_emitPaintEvent( PaintLayer.PAINTLAYER_OVERLAY_NODEPTH )


		#no-depth blend layer
		glEnable( GL_BLEND )
		self._p_emitPaintEvent( PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH )


		glDepthMask( GL_TRUE )

		glDisable( GL_BLEND )
		glDisable( GL_DEPTH_TEST )



	def _p_initGLMatrices(self):
		super( Viewport3d, self )._p_initGLMatrices()

		glMatrixMode( GL_PROJECTION )
		self.camera.setGLProjectionMatrix()
		glMatrixMode( GL_MODELVIEW )
		self.camera.setGLModelViewMatrix()



	def _p_emitWorldToScreenChangedEvent(self):
		self._grid.worldToScreenChangedNotify()
		handler = self._eventHandlers.getTop()
		if handler is not None:
			handler.evWorldToScreenChanged( self )


	def _p_emitPaintEvent(self, paintLayer):
		for painter in self._painters:
			if painter is not None:
				painter.evPaint3d( self, paintLayer )



	def evPaintPreInit(self):
		if self._lightingMode == self.LIGHTINGMODE_SINGLE:
			light_position = [ 0.3, 0.3, 1.0, 0.0 ]
			zero = [ 0.0, 0.0, 0.0, 0.0 ]
			one = [ 1.0, 1.0, 1.0, 1.0 ]

			glLightfv( GL_LIGHT0, GL_POSITION, light_position )
			glLightfv( GL_LIGHT0, GL_DIFFUSE, one )
			glLightfv( GL_LIGHT0, GL_SPECULAR, one )
			glLightfv( GL_LIGHT0, GL_AMBIENT, zero )
			glEnable( GL_LIGHT0 )
			glDisable( GL_LIGHT1 )
			glDisable( GL_LIGHT2 )
			glDisable( GL_LIGHT3 )


	def evPaintPostInit(self):
		if self._lightingMode == self.LIGHTINGMODE_QUAD:
			light0_position = [ 0.6, 1.0, 0.7, 0.0 ]
			light1_position = [ -0.9, -0.33, 0.15, 0.0 ]
			light2_position = [ -0.13, 0.3, -0.9, 0.0 ]
			light3_position = [ 1.4, -1.0, -0.2, 0.0 ]
			light0_colour = [ 0.75, 0.75, 0.75, 0.0 ]
			light1_colour = [ 0.73, 0.50, 0.28, 0.0 ]
			light2_colour = [ 0.314, 0.559, 0.75, 0.0 ]
			light3_colour = [ 0.37, 0.18, 0.18, 0.0 ]

			zero = [ 0.0, 0.0, 0.0, 0.0 ]

			glLightfv( GL_LIGHT0, GL_POSITION, light0_position )
			glLightfv( GL_LIGHT0, GL_DIFFUSE, light0_colour )
			glLightfv( GL_LIGHT0, GL_SPECULAR, light0_colour )
			glLightfv( GL_LIGHT0, GL_AMBIENT, zero )
			glEnable( GL_LIGHT0 )

			glLightfv( GL_LIGHT1, GL_POSITION, light1_position )
			glLightfv( GL_LIGHT1, GL_DIFFUSE, light1_colour )
			glLightfv( GL_LIGHT1, GL_SPECULAR, light1_colour )
			glLightfv( GL_LIGHT1, GL_AMBIENT, zero )
			glEnable( GL_LIGHT1 )

			glLightfv( GL_LIGHT2, GL_POSITION, light2_position )
			glLightfv( GL_LIGHT2, GL_DIFFUSE, light2_colour )
			glLightfv( GL_LIGHT2, GL_SPECULAR, light2_colour )
			glLightfv( GL_LIGHT2, GL_AMBIENT, zero )
			glEnable( GL_LIGHT2 )

			glLightfv( GL_LIGHT3, GL_POSITION, light3_position )
			glLightfv( GL_LIGHT3, GL_DIFFUSE, light3_colour )
			glLightfv( GL_LIGHT3, GL_SPECULAR, light3_colour )
			glLightfv( GL_LIGHT3, GL_AMBIENT, zero )
			glEnable( GL_LIGHT3 )



	def _p_onViewMenuButton(self, widget, event):
		if event.button == 1:
			self.viewMenu.popup( None, None, None, 1, event.time )


	def _p_onOrigin(self, widget):
		self.camera.cameraPosition.origin()
		self.postRedraw()

	def _p_onZ0(self, widget):
		self.camera.cameraPosition.viewZ0()
		self.postRedraw()

	def _p_onOneToOne(self, widget):
		self.camera.oneToOne()
		self.postRedraw()


	def _p_onGridPlane(self, event, choice):
		if event.bUserEvent:
			self._grid.gridPlane = choice
			self.postRedraw()


	def _p_onGridPlaneToggle(self, widget):
		self._grid.bEnabled = widget.get_active()
		self.postRedraw()



	def _p_onRenderingButton(self, widget):
		self._renderingPopup.popupAdjacentTo( self._renderingButton, gtk.DIR_DOWN, PopupWindow.TIMEOUT )



	def _p_onBackgroundImagesToggle(self, event, state):
		if state != self._bBackgroundImagesVisible:
			self._bBackgroundImagesVisible = state

			if self._backgroundImagePainter is not None:
				if state:
					self.addPainter( self._backgroundImagePainter )
				else:
					self.removePainter( self._backgroundImagePainter )

			self.postRedraw()





	bRotationEnabled = property( _p_getRotationEnabled, _p_setRotationEnabled )
	gridPlane = property( _p_getGridPlane, _p_setGridPlane )
	gridColours = property( None, setGridColours )







class Viewport3dNavigationListener (ViewportNavigationListener):
	MODE_NONE = 0
	MODE_ROTATE = 1
	MODE_ZMOTION = 2
	MODE_PAN = 3


	def __init__(self, rotatePerPixel = radians( 0.2 )):
		self._runningMode = self.MODE_NONE

		self._previousPosition = Point2i()
		self._rotatePerPixel = rotatePerPixel
		self.bRotationEnabled = True



	def evButtonDown(self, viewport, button):
		pstat = viewport.pointerStatus
		shift = pstat.bShiftKey
		control = pstat.bControlKey

		if button == 1  and  not pstat.isButtonPressed( 2 )  and  not pstat.isButtonPressed( 3 ):
			# primary button pressed:
			# if neither the secondary or tertiary buttons are pressed, then
			# the control and shift key flags are used to determine the mode:
			# CONTROL		SHIFT			MODE
			# 0				 0				rotate
			# 1				 0				pan
			# 0				 1				zoom/z-motion
			# 1				 1				pan
			if not shift and not control:
				self._runningMode = self.MODE_ROTATE
			elif shift and not control:
				self._runningMode = self.MODE_ZMOTION
			elif not shift and control:
				self._runningMode = self.MODE_PAN
			elif shift and control:
				self._runningMode = self.MODE_PAN

			self._previousPosition = pstat.position
		elif button == 2  and  not pstat.isButtonPressed( 1 )  and  not pstat.isButtonPressed( 3 ):
			# Button 2 pressed: pan
			self._runningMode = self.MODE_PAN
			self._previousPosition = pstat.position
		elif button == 3  and  not pstat.isButtonPressed( 1 )  and  not pstat.isButtonPressed( 2 ):
			# Button 2 pressed: pan
			self._runningMode = self.MODE_ZMOTION
			self._previousPosition = pstat.position


	def evButtonUp(self, viewport, button):
		pstat = viewport.pointerStatus

		if not pstat.isButtonPressed( 1 )  and  not pstat.isButtonPressed( 2 )  and  not pstat.isButtonPressed( 3 ):
			self._runningMode = self.MODE_NONE


	def evMotion(self, viewport, position):
		assert isinstance( viewport, Viewport3d ), '@viewport is not a Viewport3d'

		pstat = viewport.pointerStatus

		currentPosition = pstat.position
		motion = currentPosition - self._previousPosition

		if self._runningMode == self.MODE_PAN:
			cam = viewport.camera
			prevWorld = cam.transformScreenAtTargetDistanceToWorld( Point2( float( self._previousPosition.x ), float( self._previousPosition.y ) ) )
			curWorld = cam.transformScreenAtTargetDistanceToWorld( Point2( float( currentPosition.x ), float( currentPosition.y ) ) )
			viewport.pan( curWorld - prevWorld )
			viewport.postRedraw()
		elif self._runningMode == self.MODE_ZMOTION:
			screenZ = float(motion.x)
			worldZ = viewport.camera.cameraProjection.transformDistanceAcrossScreenToDistanceThruWorld( screenZ )
			viewport.moveInZ( screenZ, worldZ )
			viewport.postRedraw()
		elif self._runningMode == self.MODE_ROTATE:
			if self.bRotationEnabled:
				viewport.rotate( float(motion.x) * -self._rotatePerPixel, float(motion.y) * self._rotatePerPixel )
				viewport.postRedraw()

		self._previousPosition = currentPosition


	def evLeaveUngrab(self, viewport):
		self._runningMode = self.MODE_NONE





if __name__ == '__main__':
	from Britefury.GL.GLU import *
	from Britefury.Math import Math
	from Britefury.Math.Math import Point2


	class HelloWorldListener (ViewportEventHandler):
		def evPrimaryButtonDown(self, viewport):
			pass

		def evPrimaryButtonUp(self, viewport):
			pass

		def evMotion(self, viewport, position):
			pass

		def evEnter(self, viewport, position):
			pass

		def evLeave(self, viewport, position):
			pass

		def evScroll(self, viewport, x, y):
			pass




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
			self.listener = HelloWorldListener()
			self.viewport = Viewport3d( sharingManager, None, backgroundPainter )

			self.viewport.addEventHandler( self.listener, 0 )
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


