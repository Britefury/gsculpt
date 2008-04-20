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


from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.GSContextMenu import GSContextMenu

from Britefury.WorkArea.Viewport import Viewport, ViewportContextButtonListener



TOOLSET_EVENTHANDLER_PRIORITY = -100

TOOLSET_CONEXTBUTTONLISTENER_PRIORITY = -100


class Toolset (ViewportContextButtonListener):
	def __init__(self, title):
		self._layout = FormLayout()

		titleLabel = gtk.Label()
		titleLabel.set_markup( '<span weight="bold">%s</span>' % ( title, ) )
		titleLabel.show()

		self._layout.row() << titleLabel
		self._layout.row()

		self._scrolledWindow = gtk.ScrolledWindow()
		self._scrolledWindow.set_policy( gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC )
		self._viewport = gtk.Viewport( self._scrolledWindow.get_hadjustment(), self._scrolledWindow.get_vadjustment() )
		self._viewport.set_shadow_type( gtk.SHADOW_NONE )

		self._acceleratorManager = None
		self._toolGroups = []

		self._bLayoutFinalised = False

		self._contextMenu = GSContextMenu()

		self._contextButtonListenerView = None



	def getWidget(self):
		if not self._bLayoutFinalised:
			self._layout.finalise()
			self._layout.getWidget().show()
			self._viewport.add( self._layout.getWidget() )
			self._viewport.show()
			self._scrolledWindow.add( self._viewport )
			self._scrolledWindow.show()
			self._bLayoutFinalised = True
		return self._scrolledWindow


	def attachProductCell(self, cell):
		pass

	def detachProductCell(self):
		pass


	def installEventHandlers(self, view):
		pass

	def uninstallEventHandlers(self):
		pass


	def installContextButtonListener(self, view):
		self._contextButtonListenerView = view
		self._contextButtonListenerView.addContextButtonListener( self, Viewport, TOOLSET_CONEXTBUTTONLISTENER_PRIORITY )

	def uninstallContextButtonListener(self):
		self._contextButtonListenerView.removeContextButtonListener( self, Viewport )
		self._contextButtonListenerView = None


	def installAccelerators(self, acceleratorManager):
		self._acceleratorManager = acceleratorManager
		for toolGroup in self._toolGroups:
			toolGroup.installAccelerators( self._acceleratorManager )


	def uninstallAccelerators(self):
		for toolGroup in self._toolGroups:
			toolGroup.uninstallAccelerators( self._acceleratorManager )
		self._acceleratorManager = None


	def addToolGroup(self, toolGroup):
		toolGroup.layoutContents( self._layout )
		toolGroup.initContextMenu( self._contextMenu )
		self._toolGroups.append( toolGroup )
		if self._acceleratorManager is not None:
			toolGroup.installAccelerators( self._acceleratorManager )



	def evContextButton(self, viewport):
		self._contextMenu.popup()
