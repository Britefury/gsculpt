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

from Britefury.Event import QueuedEvent

from Britefury.ProceduralCore.GSSceneNode import GSSceneNode

from Britefury.SceneEdit import NodePlanAdapter
from Britefury.SceneEdit.NodePlanAdapter import NodePlanAdapter



class NodePlanViewList (object):
	COLUMN_PROC = 0
	NUM_COLUMNS = 1


	selectionSignal = ClassSignal()


	def __init__(self):
		self._planStore = gtk.ListStore( str )
		self._planView = gtk.TreeView( self._planStore )
		self._planView.set_enable_search( False )

		renderer = gtk.CellRendererText()
		column = gtk.TreeViewColumn( _( 'Action' ), renderer, markup=0 )
		self._planView.append_column( column )

		self._consistency_inconsistencyPosition = None
		self._consistency_validityUpperRange = None
		self._consistency_bRefreshRequired = False

		self._planSelection = self._planView.get_selection()
		self._planSelection.set_mode( gtk.SELECTION_SINGLE )

		self._planSelectionHandler = self._planSelection.connect( 'changed', self._p_onPlanSelectionChange )
		self._planView.show()

		self._selectedFunction = None


		self._planScrolledWindow = gtk.ScrolledWindow()
		self._planScrolledWindow.set_policy( gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC )
		# Need to know about scroll events
		vAdjust = self._planScrolledWindow.get_vadjustment()
		vAdjust.connect( 'value-changed', self._p_onScrollWindowChange )

		self._planScrolledWindow.add( self._planView )


		self._planAdapter = None





	def attachPlanAdapter(self, planAdapter):
		assert self._planAdapter is None, 'plan adapter already attached'

		self._planAdapter = planAdapter
		self._planAdapter.nodeAttachSignal.connect( self._p_onNodeAttach )
		self._planAdapter.nodeDetachSignal.connect( self._p_onNodeDetach )
		self._planAdapter.validitySignal.connect( self._p_onValidity )
		self._planAdapter.appendedSignal.connect( self._p_onAppended )
		self._planAdapter.insertedSignal.connect( self._p_onInserted )
		self._planAdapter.setSignal.connect( self._p_onSet )
		self._planAdapter.setSliceSignal.connect( self._p_onSetSlice )
		self._planAdapter.removedSignal.connect( self._p_onRemoved )
		self._planAdapter.swappedSignal.connect( self._p_onSwapped )
		self._planAdapter.invalidateProductSignal.connect( self._p_onInvalidateProduct )

		if self._planAdapter.node is not None:
			self._p_onNodeAttach( self._planAdapter.node )

		self._p_onValidity( self._planAdapter.isValid() )



	def detachPlanAdapter(self):
		assert self._planAdapter is not None, 'no plan adapter attached'

		if self._planAdapter.node is not None:
			self._p_onNodeDetach( self._planAdapter.node )

		if self._planAdapter.isValid():
			self._p_onValidity( False )

		self._planAdapter.nodeAttachSignal.disconnect( self._p_onNodeAttach )
		self._planAdapter.nodeDetachSignal.disconnect( self._p_onNodeDetach )
		self._planAdapter.validitySignal.disconnect( self._p_onValidity )
		self._planAdapter.appendedSignal.disconnect( self._p_onAppended )
		self._planAdapter.insertedSignal.disconnect( self._p_onInserted )
		self._planAdapter.setSignal.disconnect( self._p_onSet )
		self._planAdapter.setSliceSignal.disconnect( self._p_onSetSlice )
		self._planAdapter.removedSignal.disconnect( self._p_onRemoved )
		self._planAdapter.swappedSignal.disconnect( self._p_onSwapped )
		self._planAdapter.invalidateProductSignal.disconnect( self._p_onInvalidateProduct )

		self._planAdapter = None




	def getWidget(self):
		return self._planScrolledWindow



	def unselectAll(self):
		self._planSelection.unselect_all()

	def selectNode(self):
		self._p_listSelect( 0 )

	def selectFunction(self, function):
		if isinstance( function, int )  or  isinstance( function, long ):
			self._p_listSelectRow( function )
		else:
			self._p_listSelectRow( self._planAdapter.index( function ) )



	def _p_refreshConsistency(self):
		assert self._planAdapter.isValid(), 'invalid plan adapter'

		oldPosition = self._consistency_inconsistencyPosition
		self._consistency_bRefreshRequired = False
		self._consistency_validityUpperRange = None
		self._consistency_inconsistencyPosition = self._planAdapter.findInconsistency()

		return self._consistency_inconsistencyPosition != oldPosition



	def _p_blockListEvents(self):
		self._planSelection.handler_block( self._planSelectionHandler )

	def _p_unblockListEvents(self):
		self._planSelection.handler_unblock( self._planSelectionHandler )


	def _p_clearList(self):
		self._planStore.clear()

	def _p_populateList(self):
		self._p_blockListEvents()
		self._p_clearList()
		for function in self._planAdapter:
			self._p_listAppend( self._p_getFunctionDescription( function ) )
		self._p_unblockListEvents()

	def _p_refreshList(self, fromFunc):
		if not isinstance( fromFunc, int ):
			fromFunc = self._planAdapter.index( fromFunc )
		for i, function in enumerate( self._planAdapter[fromFunc:] ):
			self._p_listSet( fromFunc + i, self._p_getFunctionDescription( function ) )

	def _p_listAppend(self, text):
		self._planStore.append( ( text, ) )

	def _p_listInsert(self, row, text):
		self._planStore.insert( row, ( text, ) )

	def _p_listSet(self, row, text):
		self._planStore[row] = ( text, )

	def _p_listSetSlice(self, i, j, texts):
		prevLen = j - i
		newLen = len( texts )
		minLen = min( prevLen, newLen )
		for index, text in enumerate( texts[:minLen] ):
			self._planStore[i+index] = ( text, )
		if prevLen > minLen:
			for index in xrange( prevLen-1, minLen-1, -1 ):
				del self._planStore[i+index]
		elif newLen > minLen:
			for index in range( minLen, newLen ):
				self._planStore.insert( i+index, texts[index] )


	def _p_listRemove(self, row):
		del self._planStore[row]

	def _p_listSwap(self, row1, row2):
		storeIter1 = self._planStore.iter_nth_child( None, row1 )
		storeIter2 = self._planStore.iter_nth_child( None, row2 )

		assert storeIter1 is not None   and   storeIter2 is not None

		self._planStore.swap( storeIter1, storeIter2 )



	def _p_listScrollTo(self, row):
		self._planView.scroll_to_cell( ( row, ) )



	def _p_listSelectRow(self, row):
		self._p_listScrollTo( row )

		storeIter = self._planStore.iter_nth_child( None, row )
		assert storeIter is not None

		self._planSelection.select_iter( storeIter )


	def _p_getFunctionDescription(self, func):
		if isinstance( func, GSSceneNode ):
			return '<b><span foreground="#008000"><i>'  +  _( 'Node:')  +  ' </i></span>%s</b>'  %  ( func.getDescription(), )
		else:
			text = func.getDescription()

			if self._consistency_inconsistencyPosition is not None:
				index = self._planAdapter.index( func )

				if index == self._consistency_inconsistencyPosition:
					return '<i><span foreground="#c00000">   ' + text + '   </span></i>';
				elif index > self._consistency_inconsistencyPosition:
					return '<i>   ' + text + '   </i>';

			return text



	def _p_getVisibleRowRange(self):
		vAdjust = self._planScrolledWindow.get_vadjustment()

		# Get the size of one row
		rowSize = vAdjust.step_increment

		# Get the lower and upper range of the visible page
		pageLower = vAdjust.value
		pageUpper = vAdjust.value  +  vAdjust.page_size

		return int( pageLower / rowSize ), int( pageUpper / rowSize )


	def _p_rowSelected(self, row):
		self._selectedFunction = self._planAdapter[row]
		self.selectionSignal.emit( self._selectedFunction )


	def _p_allRowsUnselected(self):
		self._selectedFunction = None
		self.selectionSignal.emit( self._selectedFunction )



	def _p_queuedConsistencyRefresh(self):
		if self._consistency_bRefreshRequired:
			self._p_refreshConsistency()
			if self._consistency_validityUpperRange is not None:
				self._p_refreshList( self._consistency_validityUpperRange )



	def _p_onPlanSelectionChange(self, selection, data=None):
		model, storeIter = selection.get_selected()

		if model is not None  and  storeIter is not None:
			path = model.get_path( storeIter )
			row = path[0]
			self._p_rowSelected( row )
		else:
			self._p_allRowsUnselected()


	def _p_onScrollWindowChange(self, adjustment, data=None):
		# Get the visible range
		lowerRow, upperRow = self._p_getVisibleRowRange()

		# If the consistency information needs refreshing, and the upper bound of validity is belowire the upper
		# bound if visibility, then queue a refresh event
		if self._consistency_bRefreshRequired  and  self._consistency_validityUpperRange < upperRow:
			QueuedEvent.queueEvent( self._p_queuedConsistencyRefresh )



	def _p_onAppended(self, index, func):
		self._p_refreshConsistency()
		self._p_listAppend( self._p_getFunctionDescription( func ) )
		self._p_refreshList( index )


	def _p_onInserted(self, index, func):
		self._p_refreshConsistency()
		self._p_listInsert( index, self._p_getFunctionDescription( func ) )
		self._p_refreshList( index )


	def _p_onSet(self, index, oldFunc, newFunc):
		self._p_refreshConsistency()
		self._p_listSet( index, self._p_getFunctionDescription( newFunc ) )
		self._p_refreshList( index )


	def _p_onSetSlice(self, i, j, oldFuncs, newFuncs):
		self._p_refreshConsistency()
		self._p_listSetSlice( i, j, [ self._p_getFunctionDescription( func )   for func in newFuncs ] )
		self._p_refreshList( i )


	def _p_onRemoved(self, index):
		self._p_listRemove( index )
		self._p_refreshConsistency()
		self._p_refreshList( index )


	def _p_onSwapped(self, index1, index2):
		self._p_listSwap( index1, index2 )
		self._p_refreshConsistency()
		self._p_refreshList( min( index1, index2 ) )


	def _p_onInvalidateProduct(self, index):
		if self._consistency_bRefreshRequired:
			self._consistency_validityUpperRange = min( self._consistency_validityUpperRange, index )
		else:
			self._consistency_validityUpperRange = index
			self._consistency_bRefreshRequired = True

		# Get the visible range
		lowerRow, upperRow = self._p_getVisibleRowRange()

		if self._consistency_validityUpperRange < upperRow:
			QueuedEvent.queueEvent( self._p_queuedConsistencyRefresh )


	def _p_onNodeAttach(self, node):
		self._p_refreshConsistency()
		self._p_populateList()


	def _p_onNodeDetach(self, node):
		self._selection_function = None

		self._consistency_inconsistencyPosition = None
		self._consistency_validityUpperRange = None
		self._consistency_bRefreshRequired = False

		self._p_clearList()


	def _p_onValidity(self, bValid):
		self._planScrolledWindow.set_sensitive( bValid )



	def _p_getSelectedFunction(self):
		return self._selectedFunction


	selectedFunction = property( _p_getSelectedFunction )