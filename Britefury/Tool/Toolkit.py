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


from Britefury.UI.Accelerator import Accelerator
from Britefury.UI.AcceleratorGroup import AcceleratorGroup
from Britefury.UI.ToggleGroup import ToggleGroup
from Britefury.UI.ToggleButton import ToggleButton
from Britefury.UI.UIAction import UIActionInstanceSignal




class Toolkit (object):
	def __init__(self):
		self._acceleratorManager = None
		self._accelGroup = AcceleratorGroup()
		self._eventHandlerView = None
		self._contextButtonListenerView = None
		self._productCell = None

		self._toolsets = []
		self._toolsetButtons = []
		self._toolsetToggleGroup = ToggleGroup( False )
		self._toolsetToggleGroup.choiceSignal.connect( self._p_onChoice )
		self._currentToolset = None
		self._actionInstanceToToolset = {}

		self._toggleButtonBox = gtk.HBox()
		self._toggleButtonBox.show()

		self._vbox = gtk.VBox( spacing=10 )
		self._vbox.pack_start( self._toggleButtonBox, False, False )



	def getWidget(self):
		return self._vbox



	def addToolset(self, toolset, gsImageFilename, action):
		toolsetButton = ToggleButton( gsImageFilename=gsImageFilename )
		toolsetButton.getWidget().show()
		self._toggleButtonBox.pack_start( toolsetButton.getWidget(), False, False )
		self._toolsetToggleGroup.addToggle( toolset, toolsetButton )
		actionInstance = action.instance( UIActionInstanceSignal )
		actionInstance.actionSignal.connect( self._p_onAction )
		actionInstance.installAccel( self._accelGroup )
		self._actionInstanceToToolset[actionInstance] = toolset



	def attachProductCell(self, cell):
		self._productCell = cell
		if self._currentToolset is not None:
			self._currentToolset.attachProductCell( cell )

	def detachProductCell(self):
		if self._currentToolset is not None:
			self._currentToolset.detachProductCell()
		self._productCell = None


	def installAccelerators(self, acceleratorManager):
		self._acceleratorManager = acceleratorManager
		self._accelGroup.install( self._acceleratorManager )
		if self._currentToolset is not None:
			self._currentToolset.installAccelerators( self._acceleratorManager )

	def uninstallAccelerators(self):
		if self._currentToolset is not None:
			self._currentToolset.uninstallAccelerators()
		self._accelGroup.uninstall( self._acceleratorManager )
		self._acceleratorManager = None


	def installEventHandlers(self, view):
		self._eventHandlerView = view
		if self._currentToolset is not None:
			self._currentToolset.installEventHandlers( self._eventHandlerView )

	def uninstallEventHandlers(self):
		if self._currentToolset is not None:
			self._currentToolset.uninstallEventHandlers()
		self._eventHandlerView = None


	def installContextButtonListener(self, view):
		self._contextButtonListenerView = view
		if self._currentToolset is not None:
			self._currentToolset.installContextButtonListener( self._contextButtonListenerView )

	def uninstallContextButtonListener(self):
		if self._currentToolset is not None:
			self._currentToolset.uninstallContextButtonListener()
		self._contextButtonListenerView = None



	def _p_switchInToolset(self, toolset):
		toolset.getWidget().show()
		self._vbox.pack_start( toolset.getWidget(), True, True )
		if self._productCell is not None:
			toolset.attachProductCell( self._productCell )
		if self._acceleratorManager is not None:
			toolset.installAccelerators( self._acceleratorManager )
		if self._eventHandlerView is not None:
			toolset.installEventHandlers( self._eventHandlerView )
		if self._contextButtonListenerView is not None:
			toolset.installContextButtonListener( self._contextButtonListenerView )

	def _p_switchOutToolset(self, toolset):
		if self._eventHandlerView is not None:
			toolset.uninstallEventHandlers()
		if self._contextButtonListenerView is not None:
			toolset.uninstallContextButtonListener()
		if self._acceleratorManager is not None:
			toolset.uninstallAccelerators()
		if self._productCell is not None:
			toolset.detachProductCell()
		self._vbox.remove( toolset.getWidget() )
		toolset.getWidget().hide()



	def _p_onChoice(self, event, toolset):
		if self._currentToolset is not None:
			self._p_switchOutToolset( self._currentToolset )

		self._currentToolset = toolset

		if self._currentToolset is not None:
			self._p_switchInToolset( self._currentToolset )


	def _p_onAction(self, action):
		toolset = self._actionInstanceToToolset[action]
		self._toolsetToggleGroup.choice = toolset


