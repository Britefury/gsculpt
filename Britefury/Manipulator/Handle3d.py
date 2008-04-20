##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from OpenGL.GL import *

from Britefury.Math import Math
from Britefury.Math.Math import Point3, Matrix4, Quaternion

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



class HandleComponent3d (object):
	def paintHandle3d(self, viewport, highlightedHandle):
		assert False, 'abstract'

	def raytraceChoose3d(self, raySegment, currentHandle):
		assert False, 'abstract'

	def isInsideFrustum(self, frustum):
		assert False, 'abstract'


	def dragStart3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		assert False, 'abstract'

	def dragMotion3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		assert False, 'abstract'

	def dragFinish3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		assert False, 'abstract'




class SingleHandleComponent3d (HandleComponent3d):
	def __init__(self, colour):
		self._colour = colour


	def paintHandle3d(self, viewport, highlightedHandle):
		self._p_paint3d( viewport, highlightedHandle is self )


	def raytraceChoose3d(self, raySegment, currentHandle):
		bSuccess, intersectionPoint = self._p_raytrace( raySegment )
		if bSuccess:
			raySegment.b = intersectionPoint
			return self, intersectionPoint
		else:
			return currentHandle, intersectionPoint


	def _p_paint3d(self, viewport, bHighlighted):
		assert False, 'abstract'

	def _p_raytrace(self, raySegment):
		assert False, 'abstract'


	def dragStart3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		if chosenHandle is self:
			return self._p_dragStart3d( pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )
		else:
			return None

	def dragMotion3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		if chosenHandle is self:
			return self._p_dragMotion3d( pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )
		else:
			return None

	def dragFinish3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		if chosenHandle is self:
			return self._p_dragFinish3d( pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )
		else:
			return None


	def _p_dragStart3d(self, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		assert False, 'abstract'

	def _p_dragMotion3d(self, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		assert False, 'abstract'

	def _p_dragFinish3d(self, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		assert False, 'abstract'




class CompositeHandleComponent3d (HandleComponent3d):
	def __init__(self):
		self._handles = []


	def paintHandle3d(self, viewport, highlightedHandle):
		for handle in self._handles:
			handle.paintHandle3d( viewport, highlightedHandle )


	def raytraceChoose3d(self, raySegment, currentHandle):
		seg = raySegment
		resultIntersection = None
		for handle in self._handles:
			currentHandle, intersection = handle.raytraceChoose3d( seg, currentHandle )
			if intersection is not None:
				if seg is raySegment:
					seg = copy( raySegment )
				seg.b = intersection
				resultIntersection = intersection

		return currentHandle, resultIntersection


	def isInsideFrustum(self, frustum):
		for handle in self._handles:
			if not handle.isInsideFrustum( frustum ):
				return False
		return True



	def dragStart3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		for handle in self._handles:
			result  = handle.dragStart3d( chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )
			if result is not None:
				return result
		return None

	def dragMotion3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		for handle in self._handles:
			result  = handle.dragMotion3d( chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )
			if result is not None:
				return result
		return None

	def dragFinish3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		for handle in self._handles:
			result  = handle.dragFinish3d( chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )
			if result is not None:
				return result
		return None



class TransformedSingleHandleComponent3d (SingleHandleComponent3d):
	def __init__(self, handle):
		self._handle = handle
		self._xform = Matrix4()
		self._xformAtDragStart = None
		self._inverseXform = Matrix4()
		self._inverseXformAtDragStart = None


	def paintHandle3d(self, viewport, highlightedHandle):
		glPushMatrix()
		glMultMatrix( self._xform )
		self._handle.paintHandle3d( viewport, highlightedHandle )
		glPopMatrix()


	def raytraceChoose3d(self, raySegment, currentHandle):
		xformedSegment = raySegment * self._inverseXform
		currentHandle, intersection = self._handle.raytraceChoose3d( xformedSegment, currentHandle )
		if intersection is not None:
			return currentHandle, intersection * self._xform
		else:
			return currentHandle, None


	def isInsideFrustum(self, frustum):
		return self._handle.isInsideFrustum( frustum * self._inverseXform )


	def dragStart3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._xformAtDragStart = copy( self._xform )
		self._inverseXformAtDragStart = copy( self._inverseXform )
		return self._handle.dragStart3d(  chosenHandle, pointerStatus, pointerPosition, raySegment * self._inverseXformAtDragStart,
									( camForward * self._inverseXformAtDragStart ).getNormalised(), self._xformAtDragStart * localToCameraMatrix, motionScale )

	def dragMotion3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		return self._handle.dragMotion3d(  chosenHandle, pointerStatus, pointerPosition, raySegment * self._inverseXformAtDragStart,
									( camForward * self._inverseXformAtDragStart ).getNormalised(), self._xformAtDragStart * localToCameraMatrix, motionScale )

	def dragFinish3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
		self._handle.dragFinish3d( chosenHandle, pointerStatus, pointerPosition, raySegment * self._inverseXformAtDragStart,
								( camForward * self._inverseXformAtDragStart ).getNormalised(), self._xformAtDragStart * localToCameraMatrix, motionScale )
		self._xformAtDragStart = None
		self._inverseXformAtDragStart = None



	def setXform(self, xform, inverseXform):
		self._xform = xform
		self._inverseXform = inverseXform






class Handle3d (Handle, Viewport3dPainter):
	class ViewportHandle3d (Handle.ViewportHandle):
		def __init__(self, owner, baseHandle):
			super( Handle3d.ViewportHandle3d, self ).__init__( owner, baseHandle )
			self._handle = TransformedSingleHandleComponent3d( baseHandle )
			self._position = Point3()
			self._size = 1.0


		def paint3d(self, viewport, highlightedHandle):
			self._p_refreshPositionAndSize( viewport )
			self._handle.paintHandle3d( viewport, highlightedHandle )

		def choose(self, viewport, raySegment, defaultHandle):
			self._p_refreshPositionAndSize( viewport )
			handle, intersection = self._handle.raytraceChoose3d( raySegment, defaultHandle )
			return handle


		def dragStart3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
			return self._handle.dragStart3d( chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )

		def dragMotion3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
			return self._handle.dragMotion3d( chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )

		def dragFinish3d(self, chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale):
			self._handle.dragFinish3d( chosenHandle, pointerStatus, pointerPosition, raySegment, camForward, localToCameraMatrix, motionScale )


		def onHandleTransformationChange(self):
			self._p_requestRefresh()


		def _p_refreshPositionAndSize(self, viewport):
			if self._p_isRefreshRequired():
				self._owner._p_refreshPositionAndSpaceRotation()
				desiredPosition = self._owner._position

				camera = viewport.camera
				cameraZ = camera.transformWorldToCameraZ( desiredPosition )

				viewHeight = float( viewport.view.size.y )
				self._size = camera.transformDistanceAcrossScreenToDistanceThruWorld( viewHeight * XFORMAXISHANDLE_SIZE, abs( cameraZ ) )

				invXform = Matrix4.translate( -desiredPosition.toVector3() )  *  Matrix4.scale( 1.0 / self._size )  *  self._owner._spaceRotationInvMatrix


				if not self._baseHandle.isInsideFrustum( camera.getFrustum()  *  invXform ):
					self._position = camera.cameraPosition.position  +  camera.cameraPosition.forwardAxis  *  abs( cameraZ )
				else:
					self._position = desiredPosition


				xform = self._owner._spaceRotationMatrix  *  Matrix4.scale( self._size )  *  Matrix4.translate( self._position.toVector3() )
				invXform = Matrix4.translate( -self._position.toVector3() )  *  Matrix4.scale( 1.0 / self._size )  *  self._owner._spaceRotationInvMatrix

				self._handle.setXform( xform, invXform )


				self._p_refreshed()



	def __init__(self, baseHandle, defaultHandle = None):
		super( Handle3d, self ).__init__( baseHandle, defaultHandle )
		self._positionCell = None
		self._position = None
		self._spaceRotationCell = None
		self._spaceRotationMatrix = None
		self._spaceRotationInvMatrix = None



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
				viewportHandle.paint3d( viewport, self._highlightedHandle )



	def _p_dragStart(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		bResult = viewportHandle.dragStart3d( chosenHandle, viewport.pointerStatus, pointerPosition, viewport.camera.computeRay( pointerPosition ),
										viewport.camera.cameraPosition.forwardAxis, viewport.camera.cameraPosition.worldToCameraMatrix,
										self._p_computeMotionScale( bScaledMotion ) )

		if bResult:
			self._p_onDragStart( viewport )
		return bResult

	def _p_dragMotion(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		result = viewportHandle.dragMotion3d( chosenHandle, viewport.pointerStatus, pointerPosition, viewport.camera.computeRay( pointerPosition ),
										viewport.camera.cameraPosition.forwardAxis, viewport.camera.cameraPosition.worldToCameraMatrix,
										self._p_computeMotionScale( bScaledMotion ) )

		if result is not None:
			self._p_onDragMotion( viewport, result )

	def _p_dragFinish(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		viewportHandle.dragFinish3d( chosenHandle, viewport.pointerStatus, pointerPosition, viewport.camera.computeRay( pointerPosition ),
									viewport.camera.cameraPosition.forwardAxis, viewport.camera.cameraPosition.worldToCameraMatrix,
									self._p_computeMotionScale( bScaledMotion ) )

		self._p_onDragFinish( viewport )



	def _p_onDragStart(self, viewport):
		assert False, 'abstract'

	def _p_onDragMotion(self, viewport, motionResult):
		assert False, 'abstract'

	def _p_onDragFinish(self, viewport):
		assert False, 'abstract'



	def _p_createViewportHandle(self, baseHandle):
		"""Private - create a viewport handle wrapper"""
		return Handle3d.ViewportHandle3d( self, baseHandle )


	def _p_computeHighlightedHandle(self, viewport, viewportHandle, pointerPosition, defaultHandle):
		"""Private - to be overloaded - get the handle component under the mouse"""
		raySegment = viewport.camera.computeRay( pointerPosition )
		return viewportHandle.choose( viewport, raySegment, defaultHandle )



	def _p_setPositionCell(self, positionCell):
		if positionCell is not self._positionCell:
			if self._positionCell is not None:
				self._positionCell.changedSignal.disconnect( self._p_onPositionCell )
			self._positionCell = positionCell
			if self._positionCell is not None:
				self._positionCell.changedSignal.connect( self._p_onPositionCell )
			self._p_onPositionCell()

	def _p_setSpaceRotationCell(self, rotCell):
		if rotCell is not self._spaceRotationCell:
			if self._spaceRotationCell is not None:
				self._spaceRotationCell.changedSignal.disconnect( self._p_onSpaceRotationCell )
			self._spaceRotationCell = rotCell
			if self._spaceRotationCell is not None:
				self._spaceRotationCell.changedSignal.connect( self._p_onSpaceRotationCell )
			for viewportHandle in self._viewportHandles.values():
				viewportHandle.onHandleTransformationChange()
			if self._view is not None:
				self._view.postRedraw()




	def _p_onPositionCell(self):
		self._position = None
		for viewportHandle in self._viewportHandles.values():
			viewportHandle.onHandleTransformationChange()
		if self._view is not None:
			self._view.postRedraw()


	def _p_onSpaceRotationCell(self):
		self._spaceRotationMatrix = None
		self._spaceRotationInvMatrix = None
		for viewportHandle in self._viewportHandles.values():
			viewportHandle.onHandleTransformationChange()
		if self._view is not None:
			self._view.postRedraw()


	def _p_refreshPositionAndSpaceRotation(self):
		if self._position is None:
			self._position = Point3()
			if self._positionCell is not None:
				position = self._positionCell.getValue()
				if isinstance( position, Point3 ):
					self._position = position

		if self._spaceRotationMatrix is None:
			self._spaceRotationMatrix = Matrix4()
			if self._spaceRotationCell is not None:
				spaceRot = self._spaceRotationCell.getValue()
				if isinstance( spaceRot, Quaternion ):
					self._spaceRotationMatrix = Matrix4.rotate( spaceRot )
			self._spaceRotationInvMatrix = self._spaceRotationMatrix.rigidBodyInverse()



	positionCell = property( None, _p_setPositionCell )
	spaceRotationCell = property( None, _p_setSpaceRotationCell )



