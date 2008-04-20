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

from Britefury.UI import GSImage
from Britefury.UI.ButtonPopupMenu import ButtonPopupMenu
from Britefury.UI.AcceleratorGroup import AcceleratorGroup
from Britefury.UI.UIAction import UIAction, UIActionInstanceSignal

from Britefury.WorkArea.ViewLayout import ViewLayout
from Britefury.WorkArea.ModellerView import VPType
from Britefury.WorkArea.Viewport import ViewportPageSwitchListener

VPLAYOUT_NOTEBOOK_NEW_VP_FILENAME = "vp_notebook_newvp.png"



_prevAction = UIAction( 'View.NotebookLayout.Prev', ( _( 'View' ), _( 'Notebook layout' ), _( 'Previous' ) ), accelerator='<alt>Left', tooltip=_( 'Previous view' ) )
_nextAction = UIAction( 'View.NotebookLayout.Next', ( _( 'View' ), _( 'Notebook layout' ), _( 'Next' ) ), accelerator='<alt>Right', tooltip=_( 'Next view' ) )
_view1Action = UIAction( 'View.NotebookLayout.View1', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 1' ) ), accelerator='<alt>1', tooltip=_( 'View 1' ) )
_view2Action = UIAction( 'View.NotebookLayout.View2', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 2' ) ), accelerator='<alt>2', tooltip=_( 'View 2' ) )
_view3Action = UIAction( 'View.NotebookLayout.View3', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 3' ) ), accelerator='<alt>3', tooltip=_( 'View 3' ) )
_view4Action = UIAction( 'View.NotebookLayout.View4', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 4' ) ), accelerator='<alt>4', tooltip=_( 'View 4' ) )
_view5Action = UIAction( 'View.NotebookLayout.View5', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 5' ) ), accelerator='<alt>5', tooltip=_( 'View 5' ) )
_view6Action = UIAction( 'View.NotebookLayout.View6', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 6' ) ), accelerator='<alt>6', tooltip=_( 'View 6' ) )
_view7Action = UIAction( 'View.NotebookLayout.View7', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 7' ) ), accelerator='<alt>7', tooltip=_( 'View 7' ) )
_view8Action = UIAction( 'View.NotebookLayout.View8', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 8' ) ), accelerator='<alt>8', tooltip=_( 'View 8' ) )
_view9Action = UIAction( 'View.NotebookLayout.View9', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 9' ) ), accelerator='<alt>9', tooltip=_( 'View 9' ) )
_view10Action = UIAction( 'View.NotebookLayout.View10', ( _( 'View' ), _( 'Notebook layout' ), _( 'View 10' ) ), accelerator='<alt>0', tooltip=_( 'View 10' ) )
_numViewActions = [ _view1Action, _view2Action, _view3Action, _view4Action, _view5Action, _view6Action, _view7Action, _view8Action, _view9Action, _view10Action ]




class ViewLayoutNotebook (ViewLayout, ViewportPageSwitchListener):
	def __init__(self, view, viewportTypeToFactoryAndName, acceleratorManager):
		super( ViewLayoutNotebook, self ).__init__( view, viewportTypeToFactoryAndName )

		self._acceleratorManager = acceleratorManager

		# Tooltips
		tooltips = gtk.Tooltips()

		# Controls
		newViewportMenuBox = gtk.HBox( spacing=3 )
		newViewportMenuBox.pack_start( GSImage.getGSImageWidget( VPLAYOUT_NOTEBOOK_NEW_VP_FILENAME ), False, False )
		newViewportMenuBox.pack_start( gtk.Label( _( 'New viewport' ) ), False, False )
		newViewportMenuBox.show()

		newViewportMenu = ButtonPopupMenu( contents=newViewportMenuBox )
		newViewportMenu.menuSignal.connect( self._p_onNewViewport )
		newViewportMenu.addMenuItem( VPType.LEFT, labelText=_( 'Left' ) )
		newViewportMenu.addMenuItem( VPType.RIGHT, labelText=_( 'Right' ) )
		newViewportMenu.addMenuItem( VPType.BOTTOM, labelText=_( 'Bottom' ) )
		newViewportMenu.addMenuItem( VPType.TOP, labelText=_( 'Top' ) )
		newViewportMenu.addMenuItem( VPType.FRONT, labelText=_( 'Front' ) )
		newViewportMenu.addMenuItem( VPType.BACK, labelText=_( 'Back' ) )
		newViewportMenu.addMenuItem( VPType.ORTHOGRAPHIC, labelText=_( 'Orthographic' ) )
		newViewportMenu.addMenuItem( VPType.PERSPECTIVE, labelText=_( 'Perspective' ) )
		newViewportMenu.getWidget().show()
		tooltips.set_tip( newViewportMenu.getWidget(), _( 'Create new viewport' ), _( 'Create new viewport' ) )

		moveLeftButton = gtk.Button()
		moveLeftButton.add( gtk.Arrow( gtk.ARROW_LEFT, gtk.SHADOW_ETCHED_IN ) )
		moveLeftButton.connect( 'clicked', self._p_onMoveViewportLeft )

		moveRightButton = gtk.Button()
		moveRightButton.add( gtk.Arrow( gtk.ARROW_RIGHT, gtk.SHADOW_ETCHED_IN ) )
		moveRightButton.connect( 'clicked', self._p_onMoveViewportRight )

		layoutControlsBox = gtk.HBox()
		layoutControlsBox.pack_start( newViewportMenu.getWidget(), False, False, 5 )
		layoutControlsBox.pack_start( gtk.VSeparator(), False, False, 15 )
		layoutControlsBox.pack_start( gtk.Label( _( 'Reorder:' ) ), False, False, 5 )
		layoutControlsBox.pack_start( moveLeftButton, False, False, 5 )
		layoutControlsBox.pack_start( moveRightButton, False, False, 5 )
		layoutControlsBox.show_all()


		# Notebook
		self._layoutNotebook = gtk.Notebook()
		self._layoutNotebook.connect( 'switch-page', self._p_onNotebookPageSwitch )
		self._layoutNotebook.show()


		# Main box
		self._layoutBox = gtk.VBox( spacing=5 )
		self._layoutBox.pack_start( layoutControlsBox, False, False )
		self._layoutBox.pack_start( self._layoutNotebook, True, True )
		self._layoutBox.show()



		# Accelerators
		# prev and next
		self._accelGroup = AcceleratorGroup()

		self._prevAction = _prevAction.instance( UIActionInstanceSignal )
		self._prevAction.actionSignal.connect( self._p_onPrevViewport )
		self._prevAction.installAccel( self._accelGroup )

		self._nextAction = _nextAction.instance( UIActionInstanceSignal )
		self._nextAction.actionSignal.connect( self._p_onNextViewport )
		self._nextAction.installAccel( self._accelGroup )


		# numbered
		def makeNumAccelClosure(index):
			def numAccelClosure(action):
				self._p_onNumViewport( index )
			return numAccelClosure


		self._numActions = []

		for i, action in enumerate( _numViewActions ):
			actionInstance = action.instance( UIActionInstanceSignal )
			actionInstance.actionSignal.connect( makeNumAccelClosure( i ) )
			actionInstance.installAccel( self._accelGroup )


		# Create the viewports
		self._wrappedViewports = []

		self._bViewportsAttached = False

		self._p_appendViewport( self._p_createViewport( VPType.PERSPECTIVE ) )
		self._p_appendViewport( self._p_createViewport( VPType.FRONT ) )
		self._p_appendViewport( self._p_createViewport( VPType.RIGHT ) )
		self._p_appendViewport( self._p_createViewport( VPType.TOP ) )

		self._current = self._wrappedViewports[0]
		self._visibleViewport = self._wrappedViewports[0].viewport




	def getWidget(self):
		return self._layoutBox



	def setCurrentViewport(self, viewport):
		index = self._wrappedViewports.index( viewport )

		if self._current is not None  and  self._bViewportsAttached:
			self._current.viewport.pageSwitchListener = None
			self._current.viewport.unhighlight()

		self._current = self._wrappedViewports[index]

		if self._current is not None  and  self._bViewportsAttached:
			self._current.viewport.highlight()
			self._current.viewport.pageSwitchListener = self


	def getCurrentWrappedViewport(self):
		return self._current


	def initialise(self, currentWrappedViewport):
		index = None

		if currentWrappedViewport is not None:
			if self._current is not currentWrappedViewport:
				try:
					index = self._wrappedViewports.index( currentWrappedViewport )
				except ValueError:
					self._p_appendViewport( currentWrappedViewport )
					index = len( self._wrappedViewports ) - 1
			else:
				index = self._wrappedViewports( self._current )
			self._current = currentWrappedViewport

		self._p_attachViewports()

		if index is not None:
			self._layoutNotebook.set_current_page( index )

		self._accelGroup.install( self._acceleratorManager )



	def shutdown(self):
		self._p_detachViewports()

		self._accelGroup.uninstall( self._acceleratorManager )



	def getViewports(self):
		return [ wrappedViewport.viewport   for wrappedViewport in self._wrappedViewports ]



	def _p_appendViewport(self, wrappedViewport):
		assert wrappedViewport is not None

		index = len( self._wrappedViewports )

		self._wrappedViewports.append( wrappedViewport )
		wrappedViewport.closeSignal.connect( self._p_onViewportClose )
		self._p_appendViewportPage( wrappedViewport )
		self.viewportAddSignal.emit( self, self._wrappedViewports[index].viewport )

		wrappedViewport.viewport.freeze()

		return index


	def _p_removeViewport(self, index):
		wrappedViewport = self._wrappedViewports[index]

		wrappedViewport.viewport.thaw()

		if wrappedViewport is self._current:
			self._current = None

		if wrappedViewport is not None:
			self.viewportRemoveSignal.emit( self, wrappedViewport.viewport )
			self._p_removeViewportPage( index, wrappedViewport )
			wrappedViewport.closeSignal.disconnect( self._p_onViewportClose )
			del self._wrappedViewports[index]




	def _p_attachViewports(self):
		assert not self._bViewportsAttached
		self._bViewportsAttached = True
		for v in self._wrappedViewports:
			self._p_appendViewportPage( v )

	def _p_detachViewports(self):
		assert self._bViewportsAttached
		# Remove viewports in reverse order
		for i, v in reversed( list( enumerate( self._wrappedViewports ) ) ):
			self._p_removeViewportPage( i, v )
		self._bViewportsAttached = False





	def _p_appendViewportPage(self, wrappedViewport):
		if self._bViewportsAttached:
			wrappedViewport.showCloseButton()
			wrappedViewport.getWidget().show()
			if wrappedViewport is self._current:
				wrappedViewport.viewport.highlight()

			tabLabel = gtk.Label( wrappedViewport.viewport.name )
			tabLabel.show()
			self._layoutNotebook.append_page( wrappedViewport.getWidget(), tabLabel )


	def _p_removeViewportPage(self, index, wrappedViewport):
		if self._bViewportsAttached:
			self._layoutNotebook.remove_page( index )
			wrappedViewport.getWidget().hide()
			wrappedViewport.hideCloseButton()
			if wrappedViewport is self._current:
				wrappedViewport.viewport.unhighlight()




	def _p_prevViewport(self):
		index = self._layoutNotebook.get_current_page()
		index -= 1
		if index < 0:
			index += len( self._wrappedViewports )
		self._layoutNotebook.set_current_page( index )


	def _p_nextViewport(self):
		index = self._layoutNotebook.get_current_page()
		index += 1
		if index >= len( self._wrappedViewports ):
			index -= len( self._wrappedViewports )
		self._layoutNotebook.set_current_page( index )



	def evPrevPage(self, viewport):
		self._p_prevViewport()

	def evNextPage(self, viewport):
		self._p_nextViewport()



	def _p_onNewViewport(self, event, choice):
		index = self._p_appendViewport( self._p_createViewport( choice ) )
		self._layoutNotebook.set_current_page( index )


	def _p_onPrevViewport(self, action):
		self._p_prevViewport()

	def _p_onNextViewport(self, action):
		self._p_nextViewport()


	def _p_onNumViewport(self, index):
		if index < len( self._wrappedViewports ):
			self._layoutNotebook.set_current_page( index )



	def _p_onMoveViewportLeft(self, widget):
		index = self._layoutNotebook.get_current_page()
		if index > 0:
			self._wrappedViewports[index-1], self._wrappedViewports[index]  =  self._wrappedViewports[index], self._wrappedViewports[index-1]
			self._layoutNotebook.reorder_child( self._current.getWidget(), index - 1 )


	def _p_onMoveViewportRight(self, widget):
		index = self._layoutNotebook.get_current_page()
		if index < len( self._wrappedViewports ) - 1:
			self._wrappedViewports[index+1], self._wrappedViewports[index]  =  self._wrappedViewports[index], self._wrappedViewports[index+1]
			self._layoutNotebook.reorder_child( self._current.getWidget(), index + 1 )



	def _p_onViewportClose(self, wrappedViewport):
		index = self._wrappedViewports.index( wrappedViewport )
		self._p_removeViewport( index )


	def _p_onNotebookPageSwitch(self, notebook, page, pageIndex):
		if self._visibleViewport is not None:
			self._visibleViewport.freeze()

		self.viewportSelectSignal.emit( self, self._wrappedViewports[pageIndex] )

		self._visibleViewport = self._wrappedViewports[pageIndex].viewport
		if self._visibleViewport is not None:
			self._visibleViewport.thaw()



