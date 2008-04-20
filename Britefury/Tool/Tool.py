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

from Britefury.UI.UIAction import UIAction, UIActionInstanceSignal

import ToolPanel
from ToolPanel import ToolPanelPage


_doneAction = UIAction( 'Tools.Done', ( _( 'Tools' ), _( 'Done / dismiss' ) ), _( 'Done' ), _( 'Done' ), 'backslash', _( 'Finish with this tool' ) )




class Tool (ToolPanelPage):
	"Tool base class"
	def __init__(self, view, parentWindow):
		super( Tool, self ).__init__()
		self._parentWindow = parentWindow

		self.bInstalled = False
		self.doneListener = None
		self._pageTitle = None

		self._doneAction = _doneAction.instance( UIActionInstanceSignal )
		self._doneAction.actionSignal.connect( self._p_onDone )

		self._box = gtk.VBox()
		self._box.show()

		self._titleLabel = gtk.Label( '' )
		self._titleLabel.show()

		titleTable = gtk.Table( 2, 1, True )
		titleTable.attach( self._titleLabel, 0, 1, 0, 1, gtk.EXPAND | gtk.FILL | gtk.SHRINK, gtk.FILL | gtk.SHRINK, xpadding=0, ypadding=5 )
		titleTable.show()

		self._box.pack_start( titleTable, False, True )


		self._toolContentsBox = gtk.VBox()
		self._toolContentsBox.show()
		self._box.pack_start( self._toolContentsBox, False, True );


		doneTable = gtk.Table( 1, 1, True )
		doneTable.attach( self._doneAction.button(), 0, 1, 0, 1, gtk.EXPAND | gtk.FILL | gtk.SHRINK, gtk.FILL | gtk.SHRINK )
		doneTable.show()

		self._box.pack_start( doneTable, False, True )
		# self._box.pack_start( primarySeparator, False, False, 20 )
		# self._box.pack_start( errorButton.getWidget(), False, False, 30 )

		self._toolScrolledWindow = gtk.ScrolledWindow()
		self._toolViewport = gtk.Viewport( self._toolScrolledWindow.get_hadjustment(), self._toolScrolledWindow.get_vadjustment() )
		self._toolViewport.set_shadow_type( gtk.SHADOW_NONE )
		self._toolViewport.add( self._box )
		self._toolViewport.show()
		self._toolScrolledWindow.set_policy( gtk.POLICY_NEVER, gtk.POLICY_AUTOMATIC )
		self._toolScrolledWindow.add( self._toolViewport )




	def shutdownTool(self):
		self._box = None
		self._toolContentsBox = None
		self._toolScrolledWindow = None
		self._toolViewport = None





	def getWidget(self):
		"Get the overall tool widget"
		return self._toolScrolledWindow




	def setTitleWidgetText(self, text):
		"Set the text, with added markup for dark blue bold type face"
		markupText = '<span weight=\'bold\' foreground=\'#000080\'>' + text + '</span>'
		self.setTitleWidgetMarkup( markupText )

	def setTitleWidgetMarkup(self, markupText):
		self._titleLabel.set_markup( markupText )




	def install(self, toolPanel, topLevelWindow):
		if not self.bInstalled:
			assert topLevelWindow is self._parentWindow

			# Add to tool panel
			self._p_addToToolPanel( toolPanel )
			self.bInstalled = True


	def uninstall(self):
		if self.bInstalled:
			# Remove from tool panel
			self._p_removeFromToolPanel( self._toolPanel )

			self.bInstalled = False



	def _o_installEventHandlers(self):
		pass

	def _o_uninstallEventHandlers(self):
		pass


	def _o_installAccelerators(self, acceleratorManager):
		self._doneAction.installAccel( acceleratorManager )


	def _o_uninstallAccelerators(self, acceleratorManager):
		self._doneAction.uninstallAccel( acceleratorManager )



	def _o_packContents(self, contents):
		self._toolContentsBox.pack_start( contents, False, True )



	def _p_addToToolPanel(self, toolPanel):
		assert self._pageTitle is not None, 'page title not set'
		self.getWidget().show()
		toolPanel.addPage( self, self._pageTitle, True )

	def _p_removeFromToolPanel(self, toolPanel):
		toolPanel.removePage( self )
		self._toolScrolledWindow.hide()


	def onPageSwitchIn(self):
		assert self._acceleratorManager is not None
		self._o_installAccelerators( self._acceleratorManager )
		self._o_installEventHandlers()

	def onPageSwitchOut(self):
		assert self._acceleratorManager is not None
		self._o_uninstallEventHandlers()
		self._o_uninstallAccelerators( self._acceleratorManager )



	def _o_done(self):
		if self.doneListener is not None:
			self.doneListener( self )

	def _p_onDone(self, action):
		self._o_done()






