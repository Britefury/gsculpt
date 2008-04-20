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

from Britefury.WorkArea.ViewLayout import ViewLayout


class ViewLayoutSingle (ViewLayout):
	def __init__(self, view, viewportTypeToFactoryAndName):
		super( ViewLayoutSingle, self ).__init__( view, viewportTypeToFactoryAndName )

		self._box = gtk.HBox()
		self._box.show()

		self._wrappedViewport = None
		self._bViewportAttached = False

		self._p_setViewport( self._p_createViewport() )




	def getWidget(self):
		return self._box


	def setCurrentViewport(self, viewport):
		assert viewport is self._wrappedViewport.viewport

	def getCurrentWrappedViewport(self):
		return self._wrappedViewport

	def initialise(self, currentWrappedViewport):
		if currentWrappedViewport is not self._wrappedViewport  and  currentWrappedViewport is not None:
			self._p_setViewport( currentWrappedViewport )

		self._p_attachViewport()

	def shutdown(self):
		self._p_detachViewport()


	def getViewports(self):
		return [ self._wrappedViewport.viewport ]


	def _p_setViewport(self, wrappedViewport):
		if self._wrappedViewport is not None:
			self.viewportRemoveSignal.emit( self, self._wrappedViewport.viewport )
			self._p_unpackViewport( self._wrappedViewport )
			self._wrappedViewport.viewportTypeSignal.disconnect( self.onViewportType )

		self._wrappedViewport = wrappedViewport

		if self._wrappedViewport is not None:
			self._wrappedViewport.viewportTypeSignal.connect( self.onViewportType )
			self._p_packViewport( self._wrappedViewport )
			self.viewportAddSignal.emit( self, self._wrappedViewport.viewport )


	def _p_attachViewport(self):
		assert not self._bViewportAttached
		self._bViewportAttached = True
		self._p_packViewport( self._wrappedViewport )

	def _p_detachViewport(self):
		assert self._bViewportAttached
		self._p_unpackViewport( self._wrappedViewport )
		self._bViewportAttached = False


	def _p_packViewport(self, wrappedViewport):
		if self._bViewportAttached:
			wrappedViewport.showTypeMenu()
			wrappedViewport.viewport.highlight()
			wrappedViewport.getWidget().show()
			self._box.pack_start( wrappedViewport.getWidget() )

	def _p_unpackViewport(self, wrappedViewport):
		if self._bViewportAttached:
			self._box.remove( wrappedViewport.getWidget() )
			wrappedViewport.hideTypeMenu()
			wrappedViewport.getWidget().hide()
			wrappedViewport.viewport.unhighlight()


	def onViewportType(self, wrappedViewport, viewportType):
		assert wrappedViewport is self._wrappedViewport
		if wrappedViewport.viewportType != viewportType:
			self._p_setViewport( self._p_createViewport( viewportType ) )
