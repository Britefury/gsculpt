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



class ToolPanelPage (object):
	def __init__(self):
		super( ToolPanelPage, self ).__init__()
		self._toolPanel = None
		self._acceleratorManager = None
		self._bAttachedToToolPanel = False


	def getWidget(self):
		assert False, 'abstract'


	def attachToolPanel(self, toolPanel, acceleratorManager):
		assert self._toolPanel is None, 'already attached to a toolbox'
		self._toolPanel = toolPanel
		self._acceleratorManager = acceleratorManager
		self._bAttachedToToolPanel = True
		self.onAttachToolPanel( toolPanel )

	def detachToolPanel(self, toolPanel):
		assert toolPanel is self._toolPanel, 'not attached to @toolbox'
		self.onDetachToolPanel( toolPanel )
		self._bAttachedToToolPanel = False
		self._toolPanel = None
		self._acceleratorManager = None


	def onPageSwitchIn(self):
		pass

	def onPageSwitchOut(self):
		pass

	def onAttachToolPanel(self, toolPanel):
		pass

	def onDetachToolPanel(self, toolPanel):
		pass





class ToolPanel (object):
	"""ToolPanel"""

	def __init__(self, acceleratorManager):
		"""
		Constructor
		@acceleratorTable: the accelerator table into which tool box pages can add their accelerators
		@view: the view which will send events to the tool box pages
		"""

		self._notebook = gtk.Notebook()
		self._notebook.set_scrollable( True )
		self._notebook.popup_enable()

		self._notebook.connect( 'switch-page', self.onSwitchPage )

		self._widgetToPage = {}

		self._currentPage = None

		self._acceleratorManager = acceleratorManager



	def getWidget(self):
		return self._notebook


	def addPage(self, page, name, makeCurrent = False):
		"""
		Add a page to the tool box
		@page: the page (subclass of ToolboxPage)
		@name: the name to give the page in the note book
		@makeCurrent: True if @page is to become the current page
		"""
		pageWidget = page.getWidget()
		self._widgetToPage[pageWidget] = page

		page.attachToolPanel( self, self._acceleratorManager )

		self._notebook.append_page( pageWidget, None )
		self._notebook.set_tab_label_text( pageWidget, name )
		self._notebook.set_menu_label_text( pageWidget, name )

		if makeCurrent:
			self.setCurrentPage( page )


	def removePage(self, page):
		"""
		Remove a page from the tool box
		@page: the page to remove
		"""
		pageWidget = page.getWidget()
		assert pageWidget in self._widgetToPage, '@page has not been added to this tool box'

		pageIndex = self._notebook.page_num( pageWidget )
		assert pageIndex != -1, 'page not found in note book'
		self._notebook.remove_page( pageIndex )

		page.detachToolPanel( self )

		del self._widgetToPage[pageWidget]



	def setCurrentPage(self, page):
		"""
		Set the current page
		@page: the new page
		"""
		pageWidget = page.getWidget()

		assert pageWidget in self._widgetToPage, '@page has not been added to this tool box'

		pageIndex = self._notebook.page_num( pageWidget )
		assert pageIndex != -1, 'page not found in note book'
		self._notebook.set_current_page( pageIndex )


	def _p_switchPageByWidget(self, pageWidget):
		"""
		Private method to handle page switching
		"""
		page = self._widgetToPage[pageWidget]

		if page != self._currentPage:
			if self._currentPage is not None:
				self._currentPage.onPageSwitchOut()

			self._currentPage = page

			if self._currentPage is not None:
				self._currentPage.onPageSwitchIn()




	def onSwitchPage(self, widget, pageWidget, pageIndex):
		"""
		Respond to gtk.Notebook.'switch-page' event
		"""
		pageWidget = self._notebook.get_nth_page( pageIndex )
		self._p_switchPageByWidget( pageWidget )



if __name__ == '__main__':
	class TestPage (ToolPanelPage):
		def __init__(self, text,):
			super( TestPage, self ).__init__()
			self._label = gtk.Label( text )
			self._label.show()
			self._text = text

		def getWidget(self):
			return self._label

		def onAttachToolPanel(self, toolPanel):
			print '%s attached' % self._text

		def onDetachToolPanel(self, toolPanel):
			print '%s detached' % self._text

		def pageSwitchIn(self):
			print '%s switch in' % self._text

		def pageSwitchOut(self):
			print '%s switch out' % self._text


	def delete_event(widget, event, data=None):
		return False

	def destroy(widget, data=None):
		gtk.main_quit()

	window = gtk.Window( gtk.WINDOW_TOPLEVEL );
	window.connect( 'delete-event', delete_event )
	window.connect( 'destroy', destroy )
	window.set_border_width( 10 )

	toolPanel = ToolPanel()
	toolPanel.getWidget().show()

	window.add( toolPanel.getWidget() )
	window.show()


	page0 = TestPage( "page0" )
	toolPanel.addPage( page0, "page 0" )

	page1 = TestPage( "page1" )
	toolPanel.addPage( page1, "page 1" )

	page2 = TestPage( "page2" )
	toolPanel.addPage( page2, "page 2" )

	page3 = TestPage( "page3" )
	toolPanel.addPage( page3, "page 3" )


	gtk.main()
