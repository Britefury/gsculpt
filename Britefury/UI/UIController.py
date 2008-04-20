##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
class UIController (object):
	def __init__(self):
		super( UIController, self ).__init__()
		self._bApplicationEvents = False


	def applicationEventsBegin(self):
		self._bApplicationEvents = True

	def applicationEventsEnd(self):
		self._bApplicationEvents = False


	def uiEvent(self, bUserEvent = None):
		if bUserEvent is None:
			return UIEvent( not self._bApplicationEvents, self )
		else:
			return UIEvent( bUserEvent, self )




class UIEvent (object):
	def __init__(self, bUserEvent, controller):
		self.bUserEvent = bUserEvent
		self.controller = controller