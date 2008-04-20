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


from Britefury.Util.PriorityList import PriorityList



class AcceleratorManager (object):
	def __init__(self):
		self._gtkAccelGroup = gtk.AccelGroup()
		self._accelCounter = 0
		self._accelsByKeyAndMods = {}



	def attachToWindow(self, window):
		window.add_accel_group( self._gtkAccelGroup )


	def detachFromWindow(self, window):
		window.remove_accel_group( self._gtkAccelGroup )



	def addAccelerator(self, accel):
		accel.__AcceleratorManager_priority = self._accelCounter
		accel._p_setManager( self )

		self._accelCounter += 1

		self._p_addAcceleratorToList( accel, accel.getKeyAndMods() )


	def removeAccelerator(self, accel):
		accel.__AcceleratorManager_priority
		accel._p_setManager( None )

		self._p_removeAcceleratorFromList( accel, accel.getKeyAndMods() )




	def _p_acceleratorModified(self, accel, oldKeyAndMods, newKeyAndMods):
		self._p_removeAcceleratorFromList( accel, oldKeyAndMods )
		self._p_addAcceleratorToList( accel, newKeyAndMods )




	def _p_addAcceleratorToList(self, accel, keyAndMods):
		if keyAndMods[0] is not None  and  keyAndMods[1] is not None:
			try:
				accelList = self._accelsByKeyAndMods[keyAndMods]
			except KeyError:
				accelList = PriorityList()
				self._accelsByKeyAndMods[keyAndMods] = accelList
				self._gtkAccelGroup.connect_group( keyAndMods[0], keyAndMods[1], 0, self._p_onAccelKey )
			accelList.add( accel, accel.__AcceleratorManager_priority )


	def _p_removeAcceleratorFromList(self, accel, keyAndMods):
		if keyAndMods[0] is not None  and  keyAndMods[1] is not None:
			accelList = self._accelsByKeyAndMods[keyAndMods]
			accelList.remove( accel )
			if accelList.isEmpty():
				del self._accelsByKeyAndMods[keyAndMods]
				self._gtkAccelGroup.disconnect_key( *keyAndMods )



	def _p_onAccelKey(self, accelGroup, acceleratable, key, modifiers):
		keyAndMods = ( key, modifiers )
		accelList = self._accelsByKeyAndMods[keyAndMods]
		accelList.getTop()._p_activate()

		return True

