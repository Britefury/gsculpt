##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import weakref

from Britefury.Util.SignalSlot import ClassSignal
from Britefury.Util import LinkedList



class GSCacheEntry (object):
	"""GSCacheEntry - an entry in a cache
	Note: cache entries must be cleared using the clear() method, otherwise the reference held by
	the GSCache will prevent it from being removed by the garbage collector"""

	purgeSignal = ClassSignal()


	def __init__(self, cache = None):
		"""Constructor:
		@cache the cache that manages this cache entry"""
		self._cache = cache
		self._object = None
		self._lruNode = LinkedList.LinkedListNode()


	def setCache(self, cache):
		"Set the cache that manages this cache entry"
		assert self._cache is None, 'cache already set'
		self._cache = cache

	def accessObject(self):
		"Access the cached object"
		assert self._cache is not None, 'no cache set'
		if self._object is not None:
			self._cache._p_entryAccessed( self )
		return self._object

	def getObjectClass(self):
		"Get the class of the cached object"
		assert self._cache is not None, 'no cache set'
		assert self._object is not None, 'no object set'
		return self._object.__class__

	def setObject(self, object):
		"Set the cached object"
		assert self._cache is not None, 'no cache set'
		self.clear()
		self._object = object
		self._cache._p_entrySet( self, object.estimateCost() )

	def clear(self):
		"Clear the cache entry"
		assert self._cache is not None, 'no cache set'
		if self._object is not None:
			self._cache._p_entryCleared( self, self._object.estimateCost() )
			self._object = None

	def hasObject(self):
		"Determine if the cache entry contains an object"
		assert self._cache is not None, 'no cache set'
		return self._object is not None

	def isEmpty(self):
		"Determine if the cache entry is empty"
		assert self._cache is not None, 'no cache set'
		return self._object is None


	def _p_purge(self):
		"Private - purge the cache entry"
		assert self._cache is not None, 'no cache set'
		if self._object is not None:
			self.clear()
			self.purgeSignal.emit( self )

	def _p_getSize(self):
		"Private - get the size (in bytes) of the cached object"
		assert self._cache is not None, 'no cache set'
		if self._object is not None:
			return self._object.estimateCost()
		else:
			return 0

	def _p_lruNodeFunction(self):
		"Private - get the LRU linked list node"
		return self._lruNode





class GSCache (object):
	"""GSCache - a cache entry manager
	removes least recently used cache entries in order to keep the used size below a set limit"""
	def __init__(self, capacity):
		self._cacacity = capacity
		self._spaceUsed = 0
		self._lruList = LinkedList.LinkedList( GSCacheEntry._p_lruNodeFunction )

	def _p_entryAccessed(self, entry):
		"Private - log an access"
		self._lruList.remove( entry )
		self._lruList.append( entry )

	def _p_entryCleared(self, entry, entrySize):
		"Private - notify of entry clear"
		self._lruList.remove( entry )
		self._spaceUsed -= entrySize

	def _p_entrySet(self, entry, entrySize):
		"Private - notify of entry set"
		self._spaceUsed += entrySize
		self._p_removeLRUEntries()
		self._lruList.append( entry )

	def _p_removeLRUEntries(self):
		"Private - remove least recently used entries in order to stay within size limit"
		entry = self._lruList.head

		while entry is not None  and  self._spaceUsed > self._cacacity:
			next = entry._lruNode.next
			entry._p_purge()
			entry = next




# *************************** UNIT TESTING ***************************

if __name__ == '__main__':
	import unittest

	class TestObject (object):
		def __init__(self, cost=1):
			self._cost = cost

		def estimateCost(self):
			return self._cost


	class GSCacheTest (unittest.TestCase):
		def testCache(self):
			cache = GSCache( 10 )

			entry0 = GSCacheEntry( cache )
			self.assert_( entry0.isEmpty() )
			self.assertEqual( cache._spaceUsed, 0 )

			entry0.setObject( TestObject( 2 ) )
			self.assert_( entry0.hasObject() )
			self.assertEqual( cache._spaceUsed, 2 )

			entry1 = GSCacheEntry( cache )
			entry1.setObject( TestObject( 5 ) )
			self.assertEqual( cache._spaceUsed, 7 )

			entry2 = GSCacheEntry( cache )
			entry2.setObject( TestObject( 4 ) )
			self.assert_( entry0.isEmpty() )
			self.assertEqual( cache._spaceUsed, 9 )

			entry1.accessObject()
			entry0.setObject( TestObject( 2 ) )
			self.assert_( entry2.isEmpty() )
			self.assertEqual( cache._spaceUsed, 7 )


	unittest.main()



