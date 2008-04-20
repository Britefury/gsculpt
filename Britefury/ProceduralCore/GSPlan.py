##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import deepcopy

import weakref

from Britefury.FileIO import IOXml

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Cache import GSCache

from Britefury.ProceduralCore import GSProcedure, Product, getUnknownProductClass
from Britefury.ProceduralCore import GSPlanCommandTracker
from Britefury.ProceduralCore.Product import GSProduct







class GSPlanCacheEntry (GSCache.GSCacheEntry):
	def __init__(self, cache, index):
		GSCache.GSCacheEntry.__init__( self, cache )
		self.indexInPlan = index



class GSPlanInitialProductSource (object):
	"GSPlan initial product source base class"

	initialProductModifiedSignal = ClassSignal()


	def getInitialProduct(self, errorHandler):
		return None

	def getInitialProductClass(self):
		return None



class GSPlan (object):
	"""GSPlan - a list of GS procedures that define the creation of a GSProduct object

	Signals:
		appendedSignal, insertedSignal, setSignal, removeSignal, removedSignal, swappedSignal, invalidateProductSignal, productModifiedSignal
	"""
	trackerClass = GSPlanCommandTracker.GSPlanCommandTracker

	_GSPlan_planCache = GSCache.GSCache( 1 << 28 )			#256 MB


	appendedSignal = ClassSignal()
	insertedSignal = ClassSignal()
	setSignal = ClassSignal()
	setSliceSignal = ClassSignal()
	removeSignal = ClassSignal()
	removedSignal = ClassSignal()
	swappedSignal = ClassSignal()
	invalidateProductSignal = ClassSignal()
	productModifiedSignal = ClassSignal()


	def __init__(self, initialSource):
		"""Constructor:
		@initialSource - initial product source
		@src - source to copy"""
		self._procs = []
		self._cacheEntries = []
		self._initialProductSource = None
		self._firstCacheEntry = -1
		self._lastCacheEntry = -1

		self._commandTracker_ = None

		self.initialSource = initialSource



	def __deepcopy__(self, memo):
		"Create a copy"
		p = GSPlan()
		for x in self._procs:
			p.append( deepcopy( x, memo ) )
		return p

	def __readxml__(self, xmlNode):
		"Read from an XML node"
		self._p_deleteContents()
		self._procs = [ n.readObject()   for n in xmlNode.getChild( 'procedures' ).childrenNamed( 'procedure' ) ]
		self._procs = [ proc   for proc in self._procs   if proc is not None  and  isinstance( proc, GSProcedure.GSProcedure ) ]
		for index, proc in enumerate( self._procs ):
			self._p_registerProcedure( proc, index )
		self._cacheEntries = [ self._p_createPlanCacheEntry( index )   for index in xrange( 0, len( self._procs ) ) ]

	def __writexml__(self, xmlNode):
		"Write to an XML node"
		procsNode = xmlNode.addChild( 'procedures' )
		for proc in self._procs:
			procsNode.addChild( 'procedure' ).writeObject( proc )



	def __len__(self):
		"Get the number of procedures"
		return len( self._procs )


	def index(self, proc):
		"Get the index of a procedure"
		if isinstance( proc, GSProcedure.GSProcedure ):
			if proc.r_plan is self:
				return proc.r_indexInPlan
			else:
				raise ValueError, 'GSPlan.index( proc ): proc not in plan; proc plan is %s, self is %s'  %  ( proc.r_plan, self )
		else:
			raise ValueError, 'proc is not a procedure'


	def __contains__(self, proc):
		"Determine if a procedure is contained within self"
		return proc.r_plan == self


	def __getitem__(self, index):
		"Get the procedure at the specified index"
		return self._procs[index]


	def __setitem__(self, index, proc):
		"Set the procedure at the specified index"
		if isinstance( index, slice):
			i, j, stride = index.indices( len( self._procs ) )
			if stride != 1:
				raise IndexError, 'slice step must be 1'

			if i <= j:
				if i > len( self ):
					raise IndexError, 'slice start index out of range'
				if j > len( self ):
					raise IndexError, 'slice end index out of range'

				#get the old procedures
				oldProcs = self._procs[i:j]
				#invalidate the cache from @i onwards
				self._p_invalidateCache( i )
				#unregister old procedures
				for p in oldProcs:
					self._p_unregisterProcedure( p )
				#register new procedures
				for x, p in enumerate( proc ):
					self._p_registerProcedure( p, i + x )
				#put procedures in procedure list
				self._procs[i:j] = proc

				# Update cache entry list
				oldLen = j - i
				newLen = len( proc )
				#remove unneeded cache entries
				if oldLen > newLen:
					del self._cacheEntries[i+newLen:j]
				elif newLen > oldLen:
					self._cacheEntries[j:i+newLen] = [ self._p_createPlanCacheEntry( x )   for x in xrange( j, i + newLen ) ]

				#index contents
				self._p_indexContents( i, len(self) - 1 )
				#procedure list modified
				self._p_onModify( i )
				#set signal
				if self._commandTracker_ is not None:
					self._commandTracker_._f_onPlanProcSetSlice( self, i, j, oldProcs, proc )
				self.setSliceSignal.emit( self, i, j, oldProcs, proc )
		else:
			if index >= -len( self._procs )  and  index < len( self._procs ):
				#get the old proc
				oldProc = self._procs[index]
				#invalidate the cache
				self._p_invalidateCache( index )
				#remove old procedure
				self._p_unregisterProcedure( self._procs[index] )
				#register new procedure
				self._p_registerProcedure( proc, index )
				#put in procedure list
				self._procs[index] = proc
				#index contents
				self._p_indexContents( index, index + 1 )
				#procedure list modified
				self._p_onModify( index )
				#set signal
				if self._commandTracker_ is not None:
					self._commandTracker_._f_onPlanProcSet( self, index, oldProc, proc )
				self.setSignal.emit( self, index, oldProc, proc )
			else:
				raise IndexError, 'GSPlan assignment index out of range'


	def __delitem__(self, index):
		"Remove the procedure at the specified index"
		if isinstance( index, slice):
			i, j, stride = index.indices( len( self._procs ) )
			if stride != 1:
				raise IndexError, 'slice step must be 1'

			self[i:j] = []
		else:
			if index >= -len( self._procs )  and  index < len( self._procs ):
				procToRemove = self._procs[index]
				#remove signal
				self.removeSignal.emit( self, index, procToRemove )
				#invalidate the cache
				self._p_invalidateCache( index )
				#unregister
				self._p_unregisterProcedure( self._procs[index] )
				self._cacheEntries[index].clear()
				#remove from procedure list
				del self._procs[index]
				del self._cacheEntries[index]
				#index contents
				self._p_indexContents( index, len( self._procs ) )
				#removed signal
				self.removedSignal.emit( self, index )
				if self._commandTracker_ is not None:
					self._commandTracker_._f_onPlanProcRemoved( self, index, procToRemove )
				#procedure list modified
				self._p_onModify( min( index, len( self._procs ) - 1 ) )
			else:
				raise IndexError, 'GSPlan del index out of range'


	def __iter__(self):
		"Get an iterator to iterate over the procedures"
		return iter( self._procs )




	def append(self, proc):
		"Append a procedure"
		index = len( self._procs )
		#register
		self._p_registerProcedure( proc, index )
		#add to the procedure list
		self._procs.append( proc )
		self._cacheEntries.append( self._p_createPlanCacheEntry( index ) )
		#procedure list modified
		self._p_onModify( index )
		#append signal
		if self._commandTracker_ is not None:
			self._commandTracker_._f_onPlanProcAppended( self, index, proc )
		self.appendedSignal.emit( self, index, proc )

	def extend(self, procs):
		"Extend"
		end = len( self._procs )
		self[end:end] = procs

	def insert(self, index, proc):
		"Insert a procedure"
		assert index >= 0  and  index <= len( self._procs ), 'index out of range'

		if index == len( self._procs ):
			#Just use append()
			self.append( proc )
		else:
			#invalidate the cache
			self._p_invalidateCache( index )
			#insert into procedure list
			self._procs.insert( index, proc )
			self._cacheEntries.insert( index, self._p_createPlanCacheEntry( index ) )
			#register
			self._p_registerProcedure( proc, index )
			#index contents
			self._p_indexContents( index + 1, len( self._procs ) )
			#procedure list modified
			self._p_onModify( index )
			#insert signal
			if self._commandTracker_ is not None:
				self._commandTracker_._f_onPlanProcInserted( self, index, proc )
			self.insertedSignal.emit( self, index, proc )


	def insertBefore(self, beforeProc, proc):
		"Insert a procedure before another procedure"
		assert beforeProc.r_plan == self, '@beforeProc is not attached to this plan'
		self.insert( self.index( beforeProc ), proc )


	def insertAfter(self, afterProc, proc):
		"Insert a procedure after another procedure"
		assert afterProc.r_plan == self, '@afterProc is not attached to this plan'
		self.insert( self.index( afterProc ) + 1, proc )


	def remove(self, proc):
		"Remove the specified procedure"
		assert proc.r_plan == self, 'proc not attached to @self'
		index = self.index( proc )
		del self[index]


	def swap(self, proc1, proc2):
		"Swap the procedures at the specified indices"
		index1 = self._p_getProcedureIndexFromParameter( proc1 )
		index2 = self._p_getProcedureIndexFromParameter( proc2 )

		assert index1 >= 0  and  index1 < len( self._procs ), 'index1 out of range'
		assert index2 >= 0  and  index2 < len( self._procs ), 'index2 out of range'


		if index1 != index2:
			#invalidate the cache
			invalidateIndex = min( index1, index2 )
			self._p_invalidateCache( invalidateIndex )
			#swap the procedures around
			self._procs[index1], self._procs[index2] = self._procs[index2], self._procs[index1]
			#index contents
			self._p_indexContents( index1, index1 + 1 )
			self._p_indexContents( index2, index2 + 1 )
			#procedure list modified
			self._p_onModify( invalidateIndex )
			#swap signal
			if self._commandTracker_ is not None:
				self._commandTracker_._f_onPlanProcsSwapped( self, index1, index2 )
			self.swappedSignal.emit( self, index1, index2 )






	def computeProduct(self):
		"Compute the product"
		return self.computePartialProduct( len( self._procs ) - 1 )

	def computeProductClass(self):
		"Compute the class of the product"
		return self.computePartialProductClass( len( self._procs ) - 1 )

	def computePartialProduct(self, proc):
		"Compute the product of the plan as far as @proc (either a procedure, or an index)"
		procIndex = self._p_getProcedureIndexFromParameter( proc )
		product, bestCacheEntryIndex, inconsistencyPosition = self._p_evaluatePartialProduct( procIndex )
		self._p_encacheProduct( procIndex, bestCacheEntryIndex, product, True )
		return product

	def computePartialProductClass(self, proc):
		"Compute the class of the product of the plan as far as @proc (either a procedure, or an index)"
		procIndex = self._p_getProcedureIndexFromParameter( proc )
		productClass, inconsistencyIndex = self._p_computePartialProductClass( procIndex )
		return productClass

	def findInconsistency(self):
		"""Find the position of any inconsistency (point after which processing cannot continue due to a procedure unable to operate on the product)
		Returns None if there is no inconsistency, or the index of the inconsistency"""
		productClass, inconsistencyIndex = self._p_computePartialProductClass( len( self ) - 1 )
		return inconsistencyIndex

	def isConsistent(self):
		"Determine if the plan is consistent"
		return self.findInconsistency() == None





	def _p_getProcedureIndexFromParameter(self, proc):
		if isinstance( proc, int )  or  isinstance( proc, long ):
			return proc
		else:
			return self.index( proc )


	def _p_computeInitialProduct(self):
		"Private - Compute the initial product"
		assert self._initialProductSource is not None, 'no initial product source'
		return self._initialProductSource.getInitialProduct( None )

	def _p_computeInitialProductClass(self):
		"Private - Compute initial product class"
		assert self._initialProductSource is not None, 'no initial product source'
		return self._initialProductSource.getInitialProductClass()





	def _p_computePartialProductClass(self, procIndex):
		"""Private - compute the type of the partial product up to the specified index
		Returns a tuple containing:
			On success: ( product_type, None )
			On failure (due to inconsistency): ( None, inconsistency_position )"""
		productClass = None
		bestCacheEntryIndex = self._p_findBestCacheEntryFor( procIndex )

		if bestCacheEntryIndex == -1:
			#Start with the initial product
			productClass = self._p_computeInitialProductClass()
		else:
			#Found a cache entry
			productClass = self._cacheEntries[bestCacheEntryIndex].getObjectClass()

		if productClass is None:
			#Inconsistency found
			return None, bestCacheEntryIndex

		#evaluate the plan from the best cache entry onwards to @procIndex
		for i in xrange( bestCacheEntryIndex + 1, procIndex + 1 ):
			proc = self._procs[i]
			productClass = proc.getOutputClass( productClass )

			if productClass is None:
				#Inconsistency found
				return None, i

			if productClass == getUnknownProductClass():
				product, bestCacheEntryIndex, inconsistencyPosition = self._p_evaluatePartialProduct( i )
				self._p_encacheProduct( i, bestCacheEntryIndex, product, False )

				if product is not None:
					productClass = product.__class__
				else:
					return None, inconsistencyPosition

		return productClass, None


	def _p_encacheProduct(self, procIndex, bestCacheEntryIndex, product, bEncacheCopyOfProduct):
		"""Encache the product:
			@procIndex - the procedure index
			@bestCacheEntryIndex - the index of the cache entry used in the computation of @product
			@product - the product to encache
			@bEncacheCopyOfProduct - a copy of @product will be encached if set to True"""
		if procIndex > -1  and  procIndex != bestCacheEntryIndex  and  product is not None:
			cacheEntry = self._cacheEntries[procIndex]
			if bEncacheCopyOfProduct:
				product = deepcopy( product )
			cacheEntry.setObject( product )
			cacheEntry.indexInPlan = procIndex
			self._p_onCacheEntryAdd( cacheEntry )



	def _p_evaluatePartialProduct(self, procIndex):
		"""Evalute the partial product up to @procIndex
		Returns a tuple containing:
			On success: ( product, best_cache_entry_index, None )
			On failure: ( None, -1, inconsistency_position )"""
		#Find the best cache entry
		product = None
		bestCacheEntryIndex = self._p_findBestCacheEntryFor( procIndex )

		if bestCacheEntryIndex == -1:
			product = deepcopy( self._p_computeInitialProduct() )
		else:
			product = deepcopy( self._cacheEntries[bestCacheEntryIndex].accessObject() )

		if product is None:
			#Inconsistency found
			return None, -1, bestCacheEntryIndex

		#evaluate the plan from the best cache entry onwards to @procIndex
		for i in xrange( bestCacheEntryIndex + 1, procIndex + 1 ):
			proc = self._procs[i]
			product = proc.invoke( product, None )

			if product is None:
				#Inconsistency found
				return None, -1, i

		return product, bestCacheEntryIndex, None


	def _p_findBestCacheEntryFor(self, procIndex):
		"Find the best cache entry for computing the product up to @procIndex"
		#search backward from @procIndex down to 0
		for i in xrange( procIndex, -1, -1 ):
			#does a cache entry exist for the object at @i
			if self._p_isCached( i ):
				return i

		#no cache entry found
		return -1


	def _p_isCached(self, procIndex):
		return self._cacheEntries[procIndex].hasObject()




	def _p_invalidateCache(self, beginIndex):
		"Private - invalidate all cache entries from @beginIndex onward"
		if self._firstCacheEntry != -1:
			if beginIndex <= self._lastCacheEntry:
				#compute the range that needs to be cleared
				first = max( beginIndex, self._firstCacheEntry )
				last = min( len( self._procs ) - 1, self._lastCacheEntry )

				#clear the cache entries
				for i in xrange( first, last + 1 ):
					cacheEntry = self._cacheEntries[i]
					if cacheEntry.hasObject():
						cacheEntry.clear()
						cacheEntry.indexInPlan = -1

				#compute the cache range
				if beginIndex <= self._firstCacheEntry:
					#all cache entries are clear; cache is empty
					self._firstCacheEntry = self._lastCacheEntry = -1
				else:
					self._lastCacheEntry = -1

					#begin > m_firstCacheEntry
					#scan for last cache entry, from @beginIndex - 1 to 0
					for i in xrange( beginIndex, -1, -1 ):
						if self._cacheEntries[i].hasObject():
							self._lastCacheEntry = i
							break

					if self._lastCacheEntry == -1:
						#Could not find any cache entries; cache is empty
						self._firstCacheEntry = -1


	def _p_deleteContents(self):
		"Private - Delete contents"
		self._procs = []

		for c in self._cacheEntries:
			c.clear()
			c.purgeSignal.disconnect( self._p_cacheEntryPurgeSlot )

		self._cacheEntries = []

		self._firstCacheEntry = -1
		self._lastCacheEntry = -1


	def _p_indexContents(self, beginIndex, endIndex):
		"Private - reindex contents between in range [beginIndex,endIndex)"
		for i in xrange( beginIndex, endIndex ):
			proc = self._procs[i]
			cacheEntry = self._cacheEntries[i]

			proc.setIndexInPlan( self, i )

			if cacheEntry.hasObject():
				cacheEntry.indexInPlan = i


	def _p_onCacheEntryAdd(self, cacheEntry):
		"Private - notify of cache entry being added"
		index = cacheEntry.indexInPlan

		if self._firstCacheEntry == -1  and  self._lastCacheEntry == -1:
			self._firstCacheEntry = self._lastCacheEntry = index
		else:
			self._firstCacheEntry = min( self._firstCacheEntry, index )
			self._lastCacheEntry = max( self._lastCacheEntry, index )


	def _p_cacheEntryPurgeSlot(self, cacheEntry):
		"Private - cache entry purge notify slot"
		#get the index
		index = cacheEntry.indexInPlan

		#update cache range
		if self._firstCacheEntry == index  and  self._lastCacheEntry == index:
			self._firstCacheEntry = self._lastCacheEntry = -1
		elif self._firstCacheEntry == index:
			for i in xrange( self._firstCacheEntry + 1, self._lastCacheEntry + 1 ):
				if self._cacheEntries[i].hasObject():
					self._firstCacheEntry = i
					break
		elif self._lastCacheEntry == index:
			for i in xrange( self._lastCacheEntry - 1, self._firstCacheEntry - 1, -1 ):
				if self._cacheEntries[i].hasObject():
					self._lastCacheEntry = i
					break


	def _p_onInitialProductModify(self):
		"Private - initial product modify notify slot"
		#invalidate all cache entries
		self._p_invalidateCache( 0 )

		#modified; would normally call @self._p_onModify( -1 ), however, since the signals and slots chain connection system is used to pass
		#the signal on to @self.productModifeidSignal, we invoke @self.invalidateProductSignal here
		self.invalidateProductSignal.emit( self, -1 )


	def _p_onProcedureModify(self, proc):
		"Private - procedure modify notify slot"
		assert proc is not None, "procedure is None"

		#get the index of the procedure
		index = self.index( proc )

		#invalidate cache entries from the position of @proc onward
		self._p_invalidateCache( index )

		#modified
		self._p_onModify( index )


	def _p_onModify(self, index):
		"Private - procedure list modified notify"
		# See @p_onInitialProductModify(); the signals and slots chain connection system is used to pass a signal emission on to @productModifiedSignal;
		# If @p_onModify() is changed, ensure that appropriate changes are made to @p_onInitialProductModify()
		self.invalidateProductSignal.emit( self, index )
		self.productModifiedSignal.emit()



	def _p_registerProcedure(self, proc, index):
		"Private - register a procedure"
		proc.setPlan( self, index )

	def _p_unregisterProcedure(self, proc):
		"Private - unregister a procedure"
		proc.setPlan( None, -1 )


	def _p_createPlanCacheEntry(self, index):
		"Private - create a cache entry"
		cacheEntry = GSPlanCacheEntry( GSPlan._GSPlan_planCache, index )
		cacheEntry.purgeSignal.connect( self._p_cacheEntryPurgeSlot )
		return cacheEntry



	def _p_setInitialSource(self, initialSource):
		"Private - Set the initial object source"
		if self._initialProductSource is not None:
			self._initialProductSource.initialProductModifiedSignal.disconnect( self._p_onInitialProductModify )
			self._initialProductSource.initialProductModifiedSignal.chainDisconnect( self.productModifiedSignal )
		self._initialProductSource = initialSource
		if self._initialProductSource is not None:
			self._initialProductSource.initialProductModifiedSignal.connect( self._p_onInitialProductModify )
			self._initialProductSource.initialProductModifiedSignal.chainConnect( self.productModifiedSignal )
		self._p_onModify( -1 )


	initialSource = property( None, _p_setInitialSource )


IOXml.ioObjectFactoryRegister( 'GSPlan', GSPlan )










