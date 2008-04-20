##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.extlibs.greenlet import greenlet



class CoEventHandler (object):
	def __init__(self, eventProcessor):
		self._greenlet = greenlet.greenlet( eventProcessor )
		self._storedEvents = []
		self._bAlive = True


	def start(self, *args):
		self._greenlet.switch( *args )

	def terminate(self):
		if self._bAlive:
			if not self._greenlet.dead:
				self._greenlet.throw( greenlet.GreenletExit )
				assert self._greenlet.dead, 'greenlet is still alive'
			self._bAlive = False


	def sendEvent(self, event):
		if self._bAlive:
   			if self._greenlet.dead:
				self._bAlive = False

		if self._bAlive:
			if greenlet.getcurrent() is self._greenlet:
				self._storedEvents.append( event )
			else:
				if len( self._storedEvents ) > 0:
					self._storedEvents.append( event )
					event = self._storedEvents[0]
					self._storedEvents = self._storedEvents[1:]
				self._greenlet.switch( event )



	def waitForNextEvent(self):
		assert self._bAlive, 'co-routine is dead'
		assert not self._greenlet.dead, 'greenlet is dead'
		if len( self._storedEvents ) > 0:
			event = self._storedEvents[0]
			self._storedEvents = self._storedEvents[1:]
			return event
		else:
			return self._greenlet.parent.switch()
