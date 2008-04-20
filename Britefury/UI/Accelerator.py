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


from Britefury.Util.SignalSlot import ClassSignal

from UIController import UIController



class Accelerator (UIController):
	accelSignal = ClassSignal()


	def __init__(self, accelString=None, key=None, mods=None):
		super( Accelerator, self ).__init__()
		self._acceleratorManager = None

		if accelString is not None:
			key, mods = gtk.accelerator_parse( accelString )
		if key is not None  and  mods is not None:
			if isinstance( key, str ):
				key = ord( key )
			self._keyAndMods = key, mods
		else:
			self._keyAndMods = ( None, None )



	def getKeyAndMods(self):
		return self._keyAndMods



	def _p_activate(self):
		self.accelSignal.emit( self.uiEvent() )



	def _p_getKey(self):
		return self._keyAndMods[0]

	def _p_setKey(self, key):
		if isinstance( key, str ):
			key = ord( key )
		oldKeyAndMods = self._keyAndMods
		self._keyAndMods = ( key, self._keyAndMods[1] )
		if self._acceleratorManager is not None:
			self._acceleratorManager._p_acceleratorModified( self, oldKeyAndMods, self._keyAndMods )


	def _p_getModifiers(self):
		return self._modifiers[1]

	def _p_setModifiers(self, modifiers):
		oldKeyAndMods = self._keyAndMods
		self._keyAndMods = ( self._keyAndMods[0], modifiers )
		if self._acceleratorManager is not None:
			self._acceleratorManager._p_acceleratorModified( self, oldKeyAndMods, self._keyAndMods )



	def _p_getAccelString(self):
		return gtk.accelerator_name( *self._keyAndMods )

	def _p_setAccelString(self, accelString):
		oldKeyAndMods = self._keyAndMods
		if accelString is None:
			self._keyAndMods = ( None, None )
		else:
			self._keyAndMods = gtk.accelerator_parse( accelString )
		if self._acceleratorManager is not None:
			self._acceleratorManager._p_acceleratorModified( self, oldKeyAndMods, self._keyAndMods )


	def _p_setManager(self, manager):
		self._acceleratorManager = manager




	key = property( _p_getKey, _p_setKey )
	modifiers = property( _p_getModifiers, _p_setModifiers )
	accelString = property( _p_getAccelString, _p_setAccelString )


