##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import weakref

from Britefury.gSculptConfig import gSculptConfig

from Britefury.WorkArea.Viewport import ViewportEventHandler




class Handle (ViewportEventHandler):
	class ViewportHandle (object):
		def __init__(self, owner, baseHandle):
			self._baseHandle = baseHandle
			self._owner = owner
			self._bRequiresRefresh = True


		def _p_requestRefresh(self):
			self._bRequiresRefresh = True

		def _p_refreshed(self):
			self._bRequiresRefresh = False

		def _p_isRefreshRequired(self):
			return self._bRequiresRefresh

		def onViewportWorldToScreenChange(self):
			self._p_requestRefresh()




	def __init__(self, baseHandle, defaultHandle = None):
		self._viewportHandles = weakref.WeakKeyDictionary()
		self._highlightedHandle = None
		self._baseHandle = baseHandle
		self._defaultHandle = defaultHandle
		self._bButtonDown = False
		self._bScaledMotion = False
		self._view = None
		self._bDragValid = False
		self._dragViewport = None
		self._bSensitive = True


	def attachToView(self, view):
		assert self._view is None, 'already attached to a view'
		self._view = view
		self._p_addEventHandlersToView( view )

	def detachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._p_removeEventHandlersFromView( self._view )
		self._view = None



	def setSensitive(self, bSensitive):
		bSensitivityChanged = bSensitive != self._bSensitive
		self._bSensitive = bSensitive
		if self._view is not None  and  bSensitivityChanged:
			# Sensitivity changed; send a motion event to ensure that the highlighted handle is updated
			if bSensitive:
				viewport = self._view.pointerViewport
				if viewport is not None:
					self._p_motion( viewport, viewport.pointerStatus.positionF )

			self._view.postRedraw()



	def _p_addEventHandlersToView(self, view):
		"""Private - to be overloaded - add event handlers to the supplied view"""
		assert False, 'abstract'

	def _p_removeEventHandlersFromView(self, view):
		"""Private - to be overloaded - remove event handlers from the supplied view"""
		assert False, 'abstract'



	def evPrimaryButtonDown(self, viewport):
		super( Handle, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._p_buttonDown( viewport, False, viewport.pointerStatus.position.toPoint2() )

	def evPrimaryButtonUp(self, viewport):
		super( Handle, self ).evPrimaryButtonUp( viewport )
		if self._bSensitive:
			self._p_buttonUp( viewport, False, viewport.pointerStatus.position.toPoint2() )


	def evSecondaryButtonDown(self, viewport):
		super( Handle, self ).evPrimaryButtonDown( viewport )
		if self._bSensitive:
			self._p_buttonDown( viewport, True, viewport.pointerStatus.position.toPoint2() )

	def evSecondaryButtonUp(self, viewport):
		super( Handle, self ).evPrimaryButtonUp( viewport )
		if self._bSensitive:
			self._p_buttonUp( viewport, True, viewport.pointerStatus.position.toPoint2() )


	def evMotion(self, viewport, pointerPosition):
		super( Handle, self ).evMotion( viewport, pointerPosition )
		if self._bSensitive:
			self._p_motion( viewport, pointerPosition.toPoint2() )

	def evEnter(self, viewport, pointerPosition):
		super( Handle, self ).evEnter( viewport, pointerPosition )
		if self._bSensitive:
			self._p_motion( viewport, pointerPosition.toPoint2() )

	def evLeave(self, viewport, pointerPosition):
		super( Handle, self ).evLeave( viewport, pointerPosition )
		if self._bSensitive:
			self._p_motion( viewport, pointerPosition.toPoint2() )

	def evLeaveUngrab(self, viewport):
		super( Handle, self ).evLeaveUngrab( viewport )
		if self._bSensitive:
			viewportHandle = self._p_getViewportHandle( viewport )
			if self._bButtonDown:
				self._bButtonDown = False
				self._bScaledMotion = False
				if self._bDragValid:
					self._p_dragFinish( viewport, viewportHandle, viewport.pointerStatus.position.toPoint2(), self._highlightedHandle, self._bScaledMotion )
					self._bDragValid = False


	def evWorldToScreenChanged(self, viewport):
		super( Handle, self ).evWorldToScreenChanged( viewport )
		if self._bSensitive:
			viewportHandle = self._p_getViewportHandle( viewport )
			viewportHandle.onViewportWorldToScreenChange()
			if not self._bButtonDown:
				self._p_refreshHighlightedHandle( viewport, viewportHandle, viewport.pointerStatus.position.toPoint2() )



	def _p_buttonDown(self, viewport, bScaledMotion, pointerPosition):
		viewportHandle = self._p_getViewportHandle( viewport )
		if not self._bButtonDown:
			self._bButtonDown = True
			self._bScaledMotion = bScaledMotion
			self._bDragValid = self._p_dragStart( viewport, viewportHandle, pointerPosition, self._highlightedHandle, self._bScaledMotion )
			if self._bDragValid:
				self._dragViewport = viewport

	def _p_buttonUp(self, viewport, bScaledMotion, pointerPosition):
		viewportHandle = self._p_getViewportHandle( viewport )
		if self._bButtonDown  and  self._bScaledMotion == bScaledMotion:
			self._bButtonDown = False
			self._bScaledMotion = False
			if self._bDragValid  and  viewport is self._dragViewport:
				self._p_dragFinish( viewport, viewportHandle, pointerPosition, self._highlightedHandle, self._bScaledMotion )
				self._bDragValid = False
			self._dragViewport = None
			self._p_refreshHighlightedHandle( viewport, viewportHandle, pointerPosition )

	def _p_motion(self, viewport, pointerPosition):
		viewportHandle = self._p_getViewportHandle( viewport )
		if self._bButtonDown:
			if self._bDragValid  and  viewport is self._dragViewport:
				self._p_dragMotion( viewport, viewportHandle, pointerPosition, self._highlightedHandle, self._bScaledMotion )
		else:
			self._p_refreshHighlightedHandle( viewport, viewportHandle, pointerPosition )


	def _p_dragStart(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		assert False, 'abstract'

	def _p_dragMotion(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		assert False, 'abstract'

	def _p_dragFinish(self, viewport, viewportHandle, pointerPosition, chosenHandle, bScaledMotion):
		assert False, 'abstract'



	def _p_getViewportHandle(self, viewport):
		try:
			return self._viewportHandles[viewport]
		except KeyError:
			viewportHandle = self._p_createViewportHandle( self._baseHandle )
			self._viewportHandles[viewport] = viewportHandle
			return viewportHandle


	def _p_createViewportHandle(self, baseHandle):
		"""Private - to be overloaded - create a viewport handle wrapper"""
		assert False, 'abstract'



	def _p_computeHighlightedHandle(self, viewport, viewportHandle, pointerPosition, defaultHandle):
		"""Private - to be overloaded - get the handle component under the mouse"""
		assert False, 'abstract'

	def _p_refreshHighlightedHandle(self, viewport, viewportHandle, pointerPosition):
		handle = self._p_computeHighlightedHandle( viewport, viewportHandle, pointerPosition, self._defaultHandle )
		if handle is not self._highlightedHandle:
			self._highlightedHandle = handle
			self._view.postRedraw()



	def _p_computeMotionScale(self, bScaledMotion):
		if bScaledMotion:
			return gSculptConfig.manipulatorFineTuneMotionScale
		else:
			return 1.0
