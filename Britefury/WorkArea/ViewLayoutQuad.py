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
from Britefury.WorkArea.ModellerView import VPType



class ViewLayoutQuad (ViewLayout):
	def __init__(self, view, viewportTypeToFactoryAndName):
		super( ViewLayoutQuad, self ).__init__( view, viewportTypeToFactoryAndName )

		self._wrappedViewports = [ None ] * 4

		self._layoutTable = gtk.Table( 2, 2, True )
		self._bViewportsAttached = False

		self._current = None

		self._p_setViewport( 0, self._p_createViewport( VPType.TOP ) )
		self._p_setViewport( 1, self._p_createViewport( VPType.PERSPECTIVE ) )
		self._p_setViewport( 2, self._p_createViewport( VPType.FRONT ) )
		self._p_setViewport( 3, self._p_createViewport( VPType.RIGHT ) )

		self._current = self._wrappedViewports[1]




	def getWidget(self):
		return self._layoutTable



	def setCurrentViewport(self, viewport):
		index = self._wrappedViewports.index( viewport )

		if self._current is not None  and  self._bViewportsAttached:
			self._current.viewport.unhighlight()

		self._current = self._wrappedViewports[index]

		if self._current is not None  and  self._bViewportsAttached:
			self._current.viewport.highlight()


	def getCurrentWrappedViewport(self):
		return self._current


	def initialise(self, currentWrappedViewport):
		if currentWrappedViewport is not None:
			if self._current is not currentWrappedViewport:
				try:
					index = self._wrappedViewports.index( currentWrappedViewport )
				except ValueError:
					try:
						# VPType can be compared to WrappedViewport
						index = self._wrappedViewports.index( currentWrappedViewport.viewportType )
					except ValueError:
						index = 0
						if self._current is not None:
							index = self._wrappedViewports.index( self._current )
					self._p_setViewport( index, currentWrappedViewport )
			self._current = currentWrappedViewport

		self._p_attachViewports()


	def shutdown(self):
		self._p_detachViewports()



	def getViewports(self):
		return [ wrappedViewport.viewport   for wrappedViewport in self._wrappedViewports ]



	def _p_setViewport(self, index, wrappedViewport):
		if self._wrappedViewports[index] is not None:
			self.viewportRemoveSignal.emit( self, self._wrappedViewports[index].viewport )
			self._p_unpackViewport( index, self._wrappedViewports[index] )
			self._wrappedViewports[index].viewportTypeSignal.disconnect( self.onViewportType )

		if self._current is self._wrappedViewports[index]:
			self._current = wrappedViewport
		self._wrappedViewports[index] = wrappedViewport

		if self._wrappedViewports[index] is not None:
			self._wrappedViewports[index].viewportTypeSignal.connect( self.onViewportType )
			self._p_packViewport( index, self._wrappedViewports[index] )
			self.viewportAddSignal.emit( self, self._wrappedViewports[index].viewport )




	def _p_attachViewports(self):
		assert not self._bViewportsAttached
		self._bViewportsAttached = True
		for i, v in enumerate( self._wrappedViewports ):
			self._p_packViewport( i, v )

	def _p_detachViewports(self):
		assert self._bViewportsAttached
		for i, v in enumerate( self._wrappedViewports ):
			self._p_unpackViewport( i, v )
		self._bViewportsAttached = False


	def _p_packViewport(self, index, wrappedViewport):
		if self._bViewportsAttached:
			wrappedViewport.showTypeMenu()
			wrappedViewport.getWidget().show()
			if wrappedViewport is self._current:
				wrappedViewport.viewport.highlight()
			x = index % 2
			y = index / 2
			self._layoutTable.attach( wrappedViewport.getWidget(), x, x + 1, y, y + 1, gtk.FILL | gtk.EXPAND, gtk.FILL | gtk.EXPAND, 0, 0 )

	def _p_unpackViewport(self, index, wrappedViewport):
		if self._bViewportsAttached:
			self._layoutTable.remove( wrappedViewport.getWidget() )
			wrappedViewport.hideTypeMenu()
			wrappedViewport.getWidget().hide()
			if wrappedViewport is self._current:
				wrappedViewport.viewport.unhighlight()


	def onViewportType(self, wrappedViewport, viewportType):
		index = self._wrappedViewports.index( wrappedViewport )
		if wrappedViewport.viewportType != viewportType:
			self._p_setViewport( index, self._p_createViewport( viewportType ) )
