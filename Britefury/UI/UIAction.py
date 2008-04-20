##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import weakref

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.UI.Accelerator import Accelerator
from Britefury.UI.GSContextMenu import GSContextMenu




class UIAction (object):
	class CannotHandleButtonContentsError (Exception): pass

	_pathToActionTable = {}
	_actions = []


	def __init__(self, path, humanReadablePathTuple, buttonContents=None, menuName=None, accelerator=None, tooltip=None):
		self.path = path
		self.humanReadablePathTuple = humanReadablePathTuple
		self.buttonContents = buttonContents
		self.menuName = menuName
		self._accelerator = accelerator
		self.tooltip = tooltip

		self._pathToActionTable[self.path] = self
		self._actions.append( self )
		self._instances = weakref.WeakKeyDictionary()


	def instance(self, instanceClass, *args, **kwArgs):
		instance = instanceClass( self, *args, **kwArgs )
		self._instances[instance] = None
		return instance


	def createButtonContentsWidget(self):
		contents = None
		if isinstance( self.buttonContents, str ):
			contents = gtk.Label( self.buttonContents )
		elif callable( self.buttonContents ):
			contents = self.buttonContents()
		else:
			raise self.CannotHandleButtonContentsError, self.buttonContents
		contents.show()
		return contents


	def getAccelerator(self):
		if self._accelerator is None:
			return ''
		else:
			return self._accelerator

	def setAccelerator(self, accelString):
		if accelString != self._accelerator:
			self._accelerator = accelString
			instanceAccelString = self._f_getAccelStringForInstance()
			for instance in self._instances.keys():
				instance._f_setAccelerator( instanceAccelString )


	def _f_getAccelStringForInstance(self):
		if self._accelerator is None:
			return None
		elif self._accelerator == '':
			return None
		else:
			return self._accelerator



	accelerator = property( getAccelerator, setAccelerator )









class UIActionInstance (object):
	class CannotHandleButtonContentsError: pass


	_gtkTooltips = None


	def __init__(self, action):
		self._action = action
		self._accelCollections = set()

		self._accelerator = None

		self._f_setAccelerator( self._action._f_getAccelStringForInstance() )

		if self._accelerator is not None:
			self._accelerator.accelSignal.connect( self._p_onAccel )




	def button(self):
		contents = self._action.createButtonContentsWidget()
		button = gtk.Button()
		button.add( contents )
		button.show()

		button.connect( 'clicked', self._p_onGtkButton )
		self._p_getGtkTooltips().set_tip( button, self._action.tooltip )
		return button


	def contextMenuItem(self, contextMenu):
		item = self._o_createContextMenuItem( contextMenu )
		item.activateSignal.connect( self._p_onMenuItem )
		self._p_getGtkTooltips().set_tip( item.getWidget(), self._action.tooltip )
		return item




	def installAccel(self, accels):
		if self._accelerator is not None:
			accels.addAccelerator( self._accelerator )
		self._accelCollections.add( accels )

	def uninstallAccel(self, accels):
		if self._accelerator is not None:
			accels.removeAccelerator( self._accelerator )
		self._accelCollections.remove( accels )



	def _o_createContextMenuItem(self, contextMenu):
		return contextMenu.item( self._action.menuName )



	def _p_onGtkButton(self, button, data=None):
		self._o_activate()

	def _p_onMenuItem(self, event):
		self._o_activate()

	def _p_onAccel(self, event):
		self._o_activate()


	def _f_setAccelerator(self, accelString):
		if self._accelerator is None  and  accelString is not None:
			self._accelerator = Accelerator( accelString=accelString )
			for accels in self._accelCollections:
				accels.addAccelerator( self._accelerator )
		elif self._accelerator is not None  and  accelString is None:
			for accels in self._accelCollections:
				accels.removeAccelerator( self._accelerator )
			self._accelerator = None
		elif self._accelerator is not None  and  accelString is not None:
			self._accelerator.accelString = accelString


	def _o_activate(self):
		assert False, 'abstract'



	@staticmethod
	def _p_getGtkTooltips():
		if UIActionInstance._gtkTooltips is None:
			UIActionInstance._gtkTooltips = gtk.Tooltips()
		return UIActionInstance._gtkTooltips






class UIActionInstanceSignal (UIActionInstance):
	actionSignal = ClassSignal()



	def _o_activate(self):
		self.actionSignal.emit( self )





class UIActionInstanceSubmenu (UIActionInstance):
	def __init__(self, action):
		super( UIActionInstanceSubmenu, self ).__init__( action )
		self.menu = GSContextMenu()



	def _o_createContextMenuItem(self, contextMenu):
		return contextMenu.submenuItem( self._action.menuName, self.menu )



	def _o_activate(self):
		self.menu.popup()


	def _p_onMenuItem(self, event):
		pass


