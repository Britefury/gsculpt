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

from Britefury.UI.ComboBox import ComboBox

from ToolPanel import ToolPanelPage



class Toolbox (ToolPanelPage):
	def __init__(self, pageName, acceleratorManager, view):
		super( Toolbox, self ).__init__()
		self._acceleratorManager = acceleratorManager
		self._view = view
		self._productCell = None

		self._bProductCellAttached = False
		self._bEventHandlersInstalled = False
		self._bContextButtonListenerInstalled = False


		self._miscContainer = gtk.VBox()
		self._miscContainer.show()


		self._miscControlSpecs = []
		self._miscControlToSpec = {}


		self._combo = ComboBox()
		self._combo.choiceSignal.connect( self._p_onCombo )
		self._combo.getWidget().show()

		self._comboContainer = gtk.VBox()
		self._comboContainer.show()


		self._toolkitContainer = gtk.VBox()
		self._toolkitContainer.show()

		self._box = gtk.VBox( spacing=5 )
		self._box.pack_start( self._miscContainer, False, False, 2 )
		self._box.pack_start( self._comboContainer, False, False, 5 )
		self._box.pack_start( self._toolkitContainer, True, True, 0 )

 		self._currentToolkit = None
		self._toolkits = []

		self._view = None
		self._pageName = pageName

		self._bAcceleratorsInstalled = False

		# HACK
		self.dndHandler = None



	def getWidget(self):
		return self._box


	def addToolkit(self, toolkit, description):
		self._combo.append( toolkit, description )

		self._toolkits.append( toolkit )

		if len( self._toolkits ) == 1:
			self._combo.choice = toolkit

		if len( self._toolkits ) >= 2:
			self._comboContainer.pack_start( self._combo, False, False )


	def _o_packMiscControl(self, control, bExpand=False, bFill=False, padding=0):
		spec = control, bExpand, bFill, padding
		self._miscControlSpecs.append( spec )
		self._miscControlToSpec[control] = spec
		self._miscContainer.pack_start( control, bExpand, bFill, padding )

	def _o_unpackMiscControl(self, control):
		spec = self._miscControlToSpec[control]
		self._miscControlSpecs.remove( spec )
		del self._miscControlToSpec[control]
		self._miscContainer.remove( control )



	def attachProductCell(self, source):
		assert not self._bProductCellAttached, 'product cell already attached'
		self._productCell = source
		if self._currentToolkit is not None:
			self._currentToolkit.attachProductCell( source )
		self._bProductCellAttached = True

	def detachProductCell(self):
		assert self._bProductCellAttached, 'no product cell attached'
		if self._currentToolkit is not None:
			self._currentToolkit.detachProductCell()
		self._productCell = None
		self._bProductCellAttached = False


	def installAccelerators(self):
		if self._bAttachedToToolPanel:
			if self._currentToolkit is not None:
				self._currentToolkit.installAccelerators( self._acceleratorManager )

	def uninstallAccelerators(self):
		if self._bAttachedToToolPanel:
			if self._currentToolkit is not None:
				self._currentToolkit.uninstallAccelerators()


	def installEventHandlers(self):
		if not self._bEventHandlersInstalled:
			if self._currentToolkit is not None:
				self._currentToolkit.installEventHandlers( self._view )
			self._bEventHandlersInstalled = True

	def uninstallEventHandlers(self):
		if self._bEventHandlersInstalled:
			if self._currentToolkit is not None:
				self._currentToolkit.uninstallEventHandlers()
			self._bEventHandlersInstalled = False


	def installContextButtonListener(self):
		if not self._bContextButtonListenerInstalled:
			if self._currentToolkit is not None:
				self._currentToolkit.installContextButtonListener( self._view )
			self._bContextButtonListenerInstalled = True

	def uninstallContextButtonListener(self):
		if self._bContextButtonListenerInstalled:
			if self._currentToolkit is not None:
				self._currentToolkit.uninstallContextButtonListener()
			self._bContextButtonListenerInstalled = False



	def install(self, toolPanel, view):
		self._box.show()
		self._view = view
		toolPanel.addPage( self, self._pageName, False )


	def uninstall(self, toolPanel, view):
		toolPanel.removePage( self )
		self._view = None
		self._box.hide()



	def onPageSwitchIn(self):
		self.installAccelerators()
		self.installEventHandlers()

	def onPageSwitchOut(self):
		pass

	def onAttachToolPanel(self, toolPanel):
		self.installEventHandlers()
		self.installContextButtonListener()
		self.installAccelerators()

	def onDetachToolPanel(self, toolPanel):
		self.uninstallEventHandlers()
		self.uninstallContextButtonListener()
		self.uninstallAccelerators()


	def _p_onCombo(self, event, toolkit):
		if self._currentToolkit is not None:
			self._toolkitContainer.remove( self._currentToolkit.getWidget() )
			self._currentToolkit.getWidget().hide()
			if self._bAttachedToToolPanel:
				if self._view is not None:
					self._currentToolkit.uninstallEventHandlers()
					self._currentToolkit.uninstallContextButtonListener()
				if self._acceleratorManager is not None:
					self._currentToolkit.uninstallAccelerators( self._acceleratorManager )
				if self._productCell is not None:
					self._currentToolkit.detachProductCell()

		self._currentToolkit = toolkit

		if self._currentToolkit is not None:
			self._currentToolkit.getWidget().show()
			self._toolkitContainer.pack_start( self._currentToolkit.getWidget() )
			if self._bAttachedToToolPanel:
				if self._productCell is not None:
					self._currentToolkit.attachProductCell( self._objectSource )
				if self._acceleratorManager is not None:
					self._currentToolkit.installAccelerators( self._acceleratorManager )
				if self._view is not None:
					self._currentToolkit.installEventHandlers( self._view )
					self._currentToolkit.installContextButtonListener( self._view )




