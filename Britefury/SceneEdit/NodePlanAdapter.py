##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal



class NodePlanAdapter (object):
	nodeAttachSignal = ClassSignal()
	nodeDetachSignal = ClassSignal()
	validitySignal = ClassSignal()
	appendedSignal = ClassSignal()
	insertedSignal = ClassSignal()
	setSignal = ClassSignal()
	setSliceSignal = ClassSignal()
	removeSignal = ClassSignal()
	removedSignal = ClassSignal()
	swappedSignal = ClassSignal()
	invalidateProductSignal = ClassSignal()


	def __init__(self):
		super( NodePlanAdapter, self ).__init__()
		self._node = None
		self._plan = None



	def isValid(self):
		return self._node is not None


	def __len__(self):
		"Get the number of procedures"
		assert self._node is not None, 'No node'
		return len( self._plan ) + 1


	def index(self, proc):
		"Get the index of a procedure"
		assert self._node is not None, 'No node'
		if proc is self._node:
			return 0
		else:
			return self._plan.index( proc ) + 1



	def __contains__(self, proc):
		"Determine if a procedure is contained within self"
		assert self._node is not None, 'No node'
		if proc is self._node:
			return True
		else:
			return proc in self._plan


	def __getitem__(self, index):
		"Get the procedure at the specified index"
		assert self._node is not None, 'No node'
		if isinstance( index, slice ):
			i, j, stride = index.indices( len( self ) )
			if stride != 1:
				raise IndexError, 'slice step must be 1'

			if i == 0:
				return [ self._node ]  +  self._plan[0:j-1]
			else:
				return self._plan[i-1:j-1]
		else:
			if index == 0  or  index == -len( self ):
				return self._node
			else:
				if index < 0:
					return self._plan[index]
				else:
					return self._plan[index - 1]


	def __setitem__(self, index, proc):
		"Set the procedure at the specified index"
		assert self._node is not None, 'No node'
		if isinstance( index, slice):
			i, j, stride = index.indices( len( self ) )
			if stride != 1:
				raise IndexError, 'slice step must be 1'

			if start == 0:
				raise IndexError, 'cannot set item at index 0'

			self._plan[i-1:j-1] = proc
		else:
			if index == 0  or  index == -len( self ):
				raise IndexError, 'cannot set item at index 0'
			else:
				if index < 0:
					self._plan[index] = proc
				else:
					self._plan[index - 1] = proc


	def __delitem__(self, index):
		"Remove the procedure at the specified index"
		assert self._node is not None, 'No node'
		if isinstance( index, slice):
			i, j, stride = index.indices( len( self ) )
			if stride != 1:
				raise IndexError, 'slice step must be 1'

			if start == 0:
				raise IndexError, 'cannot delete item at index 0'

			del self._plan[i-1:j-1]
		else:
			if index == 0  or  index == -len( self ):
				raise IndexError, 'cannot delete item at index 0'
			else:
				if index < 0:
					del self._plan[index]
				else:
					del self._plan[index - 1]


	def __iter__(self):
		"Get an iterator to iterate over the procedures"
		assert self._node is not None, 'No node'
		yield self._node
		for proc in self._plan:
			yield proc







	def append(self, proc):
		"Append a procedure"
		assert self._node is not None, 'No node'
		self._plan.append( proc )


	def insert(self, index, proc):
		"Insert a procedure"
		assert self._node is not None, 'No node'
		if index == 0  or  index == -len( self ):
			raise IndexError, 'cannot insert before index 0'
		self._plan.insert( index - 1, proc )


	def insertBefore(self, beforeProc, proc):
		"Insert a procedure before another procedure"
		assert self._node is not None, 'No node'
		if beforeProc is self._node:
			raise IndexError, 'cannot insert before node'
		self._plan.insertBefore( beforeProc, proc )


	def insertAfter(self, afterProc, proc):
		"Insert a procedure after another procedure"
		assert self._node is not None, 'No node'
		if afterProc is self._node:
			self._plan.insert( 0, proc )
		else:
			self._plan.insertAfter( afterProc, proc )


	def remove(self, proc):
		"Remove the specified procedure"
		assert self._node is not None, 'No node'
		if proc is self._node:
			raise ValueError, 'cannot remove node'
		self._plan.remove( proc )


	def swap(self, index1, index2):
		"Swap the procedures at the specified indices"
		assert self._node is not None, 'No node'
		if index1 == 0  or  index1 == -len( self )   or   index2 == 0  or  index2 == -len( self ):
			raise IndexError, 'cannot change position of node'
		self._plan.swap( index1 - 1, index2 - 1 )


	def swapProcedures(self, proc1, proc2):
		"Swap the specified procedures"
		assert self._node is not None, 'No node'
		if proc1 is self._node  or  proc2 is self._node:
			raise IndexError, 'cannot change position of node'
		self._plan.swapProcedures( proc1, proc2 )



	def computeProduct(self):
		"Compute the product"
		assert self._node is not None, 'No node'
		return self._plan.computeProduct()

	def computeProductClass(self):
		"Compute the class of the product"
		assert self._node is not None, 'No node'
		return self._plan.computeProductClass()

	def computePartialProduct(self, func):
		"Compute the product of the plan as far as @procIndex"
		assert self._node is not None, 'No node'
		index = self._p_getFunctionIndexFromParameter( func )
		return self._plan.computePartialProduct( index - 1 )

	def computePartialProductClass(self, func):
		"Compute the class of the product of the plan as far as @procIndex"
		assert self._node is not None, 'No node'
		index = self._p_getFunctionIndexFromParameter( func )
		return self._plan.computePartialProductClass( index - 1 )

	def findInconsistency(self):
		"Find inconsistency"
		assert self._node is not None, 'No node'
		inconsistencyIndex = self._plan.findInconsistency()
		if inconsistencyIndex is None:
			return None
		else:
			return inconsistencyIndex + 1

	def isConsistent(self):
		"Determine if the plan is consistent"
		assert self._node is not None, 'No node'
		return self._plan.isConsistent()




	def _p_getFunctionIndexFromParameter(self, func):
		if func is self._node:
			return 0
		elif isinstance( func, int )  or  isinstance( func, long ):
			return func
		else:
			return self.index( func )



	def _p_getNode(self):
		"Get the node"
		return self._node

	def _p_setNode(self, node):
		"Set the node"
		if node is not self._node:
			bWasValid = self._node is not None
			if bWasValid:
				self.nodeDetachSignal.emit( self._node )
				self._plan.appendedSignal.disconnect( self._p_onAppended )
				self._plan.insertedSignal.disconnect( self._p_onInserted )
				self._plan.setSignal.disconnect( self._p_onSet )
				self._plan.setSliceSignal.disconnect( self._p_onSetSlice )
				self._plan.removeSignal.disconnect( self._p_onRemove )
				self._plan.removedSignal.disconnect( self._p_onRemoved )
				self._plan.swappedSignal.disconnect( self._p_onSwapped )
				self._plan.invalidateProductSignal.disconnect( self._p_onInvalidateProduct )

			self._node = node
			if self._node is not None:
				self._plan = node.plan
			else:
				self._plan = None

			bIsValid = self._node is not None
			if bIsValid:
				self._plan.appendedSignal.connect( self._p_onAppended )
				self._plan.insertedSignal.connect( self._p_onInserted )
				self._plan.setSignal.connect( self._p_onSet )
				self._plan.setSliceSignal.connect( self._p_onSetSlice )
				self._plan.removeSignal.connect( self._p_onRemove )
				self._plan.removedSignal.connect( self._p_onRemoved )
				self._plan.swappedSignal.connect( self._p_onSwapped )
				self._plan.invalidateProductSignal.connect( self._p_onInvalidateProduct )
				self.nodeAttachSignal.emit( self._node )

			if bWasValid != bIsValid:
				self.validitySignal.emit( bIsValid )



	def _p_onAppended(self, plan, index, proc):
		self.appendedSignal.emit( index + 1, proc )

	def _p_onInserted(self, plan, index, proc):
		self.insertedSignal.emit( index + 1, proc )

	def _p_onSet(self, plan, index, oldProc, newProc):
		self.setSignal.emit( index + 1, oldProc, newProc )

	def _p_onSetSlice(self, plan, i, j, oldProcs, newProcs):
		self.setSliceSignal.emit( i + 1, j + 1, oldProcs, newProcs )

	def _p_onRemove(self, plan, index, proc):
		self.removeSignal.emit( index + 1, proc )

	def _p_onRemoved(self, plan, index):
		self.removedSignal.emit( index + 1 )

	def _p_onSwapped(self, plan, index1, index2):
		self.swappedSignal.emit( index1 + 1, index2 + 1 )

	def _p_onInvalidateProduct(self, plan, index):
		self.invalidateProductSignal.emit( index + 1 )



	node = property( _p_getNode, _p_setNode )





if __name__ == '__main__':
	from Britefury.ProceduralCore import GSFunction, GSProcedure, GSPlan
	from Britefury.ProceduralCore.GSPlan import GSPlan
	from Britefury.ProceduralCore.GSProcedure import *


	def SimpleClass__invoke(inputObject, a):
		return inputObject + a

	SimpleClass = makeGSProcedureClass( 'SimpleClass', __name__, GSProcedure, [ GSFunction.FieldSpec( 'a', int, 1 ) ], None, None, SimpleClass__invoke )



	class Node (object):
		def __init__(self):
			self.plan = GSPlan( None )



	node = Node()

	adapter = NodePlanAdapter()
	adapter.node = node


	assert len( adapter ) == 1  and  adapter[0] is node
	a = [ SimpleClass()   for x in xrange( 0, 3 ) ]
	adapter.append( a[0] )
	adapter.append( a[1] )
	adapter.append( a[2] )
	assert len( adapter ) == 4  and  adapter[1] is a[0]
	assert adapter.index( node ) == 0  and  adapter.index( a[2] ) == 3
	assert node in adapter  and  a[1] in adapter
	assert adapter[0] is node  and  adapter[3] is a[2]
	assert adapter[:] == [ node ] + a
	assert adapter[1:] == a
	assert adapter[2:] == a[1:]
	assert adapter[:-1] == [ node ] + a[:-1]
	assert adapter[1:3] == a[:2]
	assert adapter[1:-1] == a[:-1]
	assert adapter[1:3] == a[0:2]
	del adapter[2]
	assert adapter[:] == [ node, a[0], a[2] ]

	for x, y in zip( adapter, [ node, a[0], a[2] ] ):
		assert x == y





