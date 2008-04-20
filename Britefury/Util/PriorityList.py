##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
class PriorityList (object):
	"""Priority list"""
	def __init__(self):
		"""Initialise"""
		self._entries = []


	def add(self, item, priority):
		"""Add an item
		@item: the item to add
		@priority: the priority of the item"""

		# Remove duplicate entries
		self.remove( item )

		# Find the correct position to add the item, and add it
		bFoundPosition = False
		for i, entry in enumerate( self._entries ):
			if entry[1] > priority:
				self._entries.insert( i, ( item, priority ) )
				bFoundPosition = True
				break

		if not bFoundPosition:
			self._entries.append( ( item, priority ) )


	def remove(self, item):
		"""Remove item
		@item: the item to remove"""

		# Find and remove the item
		for entry in self._entries:
			if entry[0] == item:
				self._entries.remove( entry )
				break


	def getTop(self):
		"""Get the top item"""
		item, priority = self._p_getTop()
		return item


	def isEmpty(self):
		"""Determine if the list is empty"""
		return len( self._entries ) == 0


	def _p_getTop(self):
		if len( self._entries ) > 0:
			return self._entries[-1]
		else:
			return None, None
