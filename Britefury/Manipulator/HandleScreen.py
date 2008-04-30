##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from Britefury.GL.GL import *

from Britefury.Math import Math
from Britefury.Math.Math import Point2, Matrix4

from Britefury.Graphics import Graphics
from Britefury.Graphics.Graphics import glMultMatrix

from Britefury.View import View
from Britefury.View.View import PaintLayer

from Britefury.WorkArea.Viewport import ViewportEventHandler
from Britefury.WorkArea.Viewport3d import Viewport3d, Viewport3dPainter

from Handle import Handle


XFORMAXISHANDLE_ALPHA = 0.5
HANDLE_UNHIGHLIGHTEDBRIGHTNESS = 0.5
XFORMAXISHANDLE_SIZE = 0.1



class HandleComponentScreen (object):
	def paintHandleScreen(self, viewport, highlightedHandle):
		assert False, 'abstract'

	def pickChooseScreen(self, pointerPosition, localPointerPosition, currentHandle):
		assert False, 'abstract'

	def isWithinBounds(self, boundaryPolygon):
		assert False, 'abstract'


	def dragStartScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		assert False, 'abstract'

	def dragMotionScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		assert False, 'abstract'

	def dragFinishScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		assert False, 'abstract'




class SingleHandleComponentScreen (HandleComponentScreen):
	def __init__(self, colour):
		self._colour = colour


	def paintHandleScreen(self, viewport, highlightedHandle):
		brightness = HANDLE_UNHIGHLIGHTEDBRIGHTNESS
		if highlightedHandle is self:
			brightness = 1.0
		self._p_paintScreen( viewport, brightness )


	def pickChooseScreen(self, pointerPosition, localPointerPosition, currentHandle):
		if self._p_pick( pointerPosition, localPointerPosition ):
			return self
		else:
			return currentHandle


	def _p_paintScreen(self, viewport, brightness):
		assert False, 'abstract'

	def _p_pick(self, pointerPosition, localPointerPosition):
		assert False, 'abstract'


	def dragStartScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		if chosenHandle is self:
			return self._p_dragStartScreen( pointerStatus, pointerPosition, localPointerPosition, motionScale )
		else:
			return None

	def dragMotionScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		if chosenHandle is self:
			return self._p_dragMotionScreen( pointerStatus, pointerPosition, localPointerPosition, motionScale )
		else:
			return None

	def dragFinishScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		if chosenHandle is self:
			return self._p_dragFinishScreen( pointerStatus, pointerPosition, localPointerPosition, motionScale )
		else:
			return None


	def _p_dragStartScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		assert False, 'abstract'

	def _p_dragMotionScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		assert False, 'abstract'

	def _p_dragFinishScreen(self, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		assert False, 'abstract'




class CompositeHandleComponentScreen (HandleComponentScreen):
	def __init__(self):
		self._handles = []


	def paintHandleScreen(self, viewport, highlightedHandle):
		for handle in self._handles:
			handle.paintHandleScreen( viewport, highlightedHandle )

	def pickChooseScreen(self, pointerPosition, localPointerPosition, currentHandle):
		for handle in self._handles:
			currentHandle = handle.pickChooseScreen( pointerPosition, localPointerPosition, currentHandle )
		return currentHandle

	def isWithinBounds(self, boundaryPolygon):
		for handle in self._handles:
			if not handle.isWithinBounds( boundaryPolygon ):
				return False
		return True


	def dragStartScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		for handle in self._handles:
			result  = handle.dragStartScreen( chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale )
			if result is not None:
				return result
		return None

	def dragMotionScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		for handle in self._handles:
			result  = handle.dragMotionScreen( chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale )
			if result is not None:
				return result
		return None

	def dragFinishScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		for handle in self._handles:
			result  = handle.dragFinishScreen( chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale )
			if result is not None:
				return result
		return None



class TransformedSingleHandleComponentScreen (SingleHandleComponentScreen):
	def __init__(self, handle):
		self._handle = handle
		self._xform = Matrix4()
		self._inverseXform = Matrix4()
		self._inverseXformAtDragStart = None


	def paintHandleScreen(self, viewport, highlightedHandle):
		glPushMatrix()
		glMultMatrix( self._xform )
		self._handle.paintHandleScreen( viewport, highlightedHandle )
		glPopMatrix()


	def pickChooseScreen(self, pointerPosition, localPointerPosition, currentHandle):
		return self._handle.pickChooseScreen( pointerPosition, localPointerPosition * self._inverseXform, currentHandle )


	def isWithinBounds(self, boundaryPolygon):
		return self._handle.isWithinBounds( boundaryPolygon * self._inverseXform )


	def dragStartScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		self._inverseXformAtDragStart = self._inverseXform
		return self._handle.dragStartScreen( chosenHandle, pointerStatus, pointerPosition, pointerPosition * self._inverseXformAtDragStart, motionScale )

	def dragMotionScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		return self._handle.dragMotionScreen( chosenHandle, pointerStatus, pointerPosition, pointerPosition * self._inverseXformAtDragStart, motionScale )

	def dragFinishScreen(self, chosenHandle, pointerStatus, pointerPosition, localPointerPosition, motionScale):
		self._handle.dragFinishScreen( chosenHandle, pointerStatus, pointerPosition, pointerPosition * self._inverseXformAtDragStart, motionScale )
		self._inverseXformAtDragStart = None


	def setXform(self, xform, inverseXform):
		self._xform = xform
		self._inverseXform = inverseXform






class HandleScreen (Handle, Viewport3dPainter):
	class ViewportHandleScreen (Handle.ViewportHandle):
		def __init__(self, owner, baseHandle):
			super( HandleScreen.ViewportHandleScreen, self ).__init__( owner, baseHandle )
			self._handle = TransformedSingleHandleComponentScreen( baseHandle )
			self._position = Point2()
			self._size = 1.0


		def paintScreen(self, viewport, highlightedHandle):
			self._p_refreshPositionAndSize( viewport )
			self._handle.paintHandleScreen( viewport, highlightedHandle )

		def choose(self, viewport, pointerPosition, defaultHandle):
			self._p_refreshPositionAndSize( viewport )
			return self._handle.pickChooseScreen( pointerPosition, pointerPosition, defaultHandle )


		def dragStartScreen(self, chosenHandle, pointerStatus, pointerPosition, motionScale):
			return self._handle.dragStartScreen( chosenHandle, pointerStatus, pointerPosition, pointerPosition, motionScale )

		def dragMotionScreen(self, chosenHandle, pointerStatus, pointerPosition, motionScale):
			return self._handle.dragMotionScreen( chosenHandle, pointerStatus, pointerPosition, pointerPosition, motionScale )

		def dragFinishScreen(self, chosenHandle, pointerStatus, pointerPosition, motionScale):
			self._handle.dragFinishScreen( chosenHandle, pointerStatus, pointerPosition, pointerPosition, motionScale )


		def _p_refreshPositionAndSize(self, viewport):
			if self._p_isRefreshRequired():
				viewHeight = float( viewport.view.size.y )
				self._size = viewHeight * XFORMAXISHANDLE_SIZE

				viewportSize = viewport.size
				self._position = Point2( float( viewportSize.x ) * 0.25,  float( viewportSize.y ) * 0.25 )

				xform = Matrix4.scale( self._size )  *  Matrix4.translate( self._position.toVector2() )
				invXform = Matrix4.translate( -self._position.toVector2() )  *  Matrix4.scale( 1.0 / self._size )

				self._handle.setXform( xform, invXform )


				self._p_refreshed()



	def _p_addEventHandlersToView(self, view):
		"""Private - add event handlers to the supplied view"""
		view.addPainter( self, Viewport3d )
		view.addEventHandler( self, Viewport3d )

	def _p_removeEventHandlersFromView(self, view):
		"""Private - remove event handlers from the supplied view"""
		view.removePainter( self, Viewport3d )
		view.removeEventHandler( self, Viewport3d )



	def evPaint3d(self, viewport, paintLayer):
		if self._bSensitive:
			viewportHandle = self._p_getViewportHandle( viewport )
			if paintLayer == PaintLayer.PAINTLAYER_OVERLAY_BLEND_NODEPTH:
				glMatrixMode( GL_PROJECTION )
				glPushMatrix()
				viewport.glProjectionIdentity()

				glMatrixMode( GL_MODELVIEW )
				glPushMatrix()
				viewport.glModelViewIdentity()

				viewportHandle.paintScreen( viewport, self._highlightedHandle )

				glPopMatrix()
				glMatrixMode( GL_PROJECTION )
				glPopMatrix()
				glMatrixMode( GL_MODELVIEW )



	def _p_dragStart(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		bResult = viewportHandle.dragStartScreen( chosenHandle, viewport.pointerStatus, pointerPosition, self._p_computeMotionScale( bScaledMotion ) )

		if bResult:
			self._p_onDragStart( viewport )
		return bResult

	def _p_dragMotion(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		result = viewportHandle.dragMotionScreen( chosenHandle, viewport.pointerStatus, pointerPosition, self._p_computeMotionScale( bScaledMotion ) )

		if result is not None:
			self._p_onDragMotion( viewport, result )

	def _p_dragFinish(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		viewportHandle.dragFinishScreen( chosenHandle, viewport.pointerStatus, pointerPosition, self._p_computeMotionScale( bScaledMotion ) )

		self._p_onDragFinish( viewport )


	def _p_createViewportHandle(self, baseHandle):
		"""Private - create a viewport handle wrapper"""
		return HandleScreen.ViewportHandleScreen( self, baseHandle )


	def _p_computeHighlightedHandle(self, viewport, viewportHandle, pointerPosition, defaultHandle):
		"""Private - to be overloaded - get the handle component under the mouse"""
		return viewportHandle.choose( viewport, pointerPosition, defaultHandle )

