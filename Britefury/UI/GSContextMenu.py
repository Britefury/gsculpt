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

from Britefury.UI.PopupWindow import PopupWindow
from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.UIController import UIController




class GSContextMenuItem (UIController):
	activateSignal = ClassSignal()


	def __init__(self, labelText=None, contents=None):
		super( GSContextMenuItem, self ).__init__()
		if labelText is not None:
			self._button = gtk.Button( labelText )
		elif contents is not None:
			self._button = gtk.Button()
			self._button.add( contents )
		else:
			raise ValueError, 'could not process button contents'
		self._button.connect( 'clicked', self._p_onButton )
		self._button.show()


	def getWidget(self):
		return self._button


	def _p_onButton(self, widget):
		self.activateSignal.emit( self.uiEvent() )




class GSContextMenu (object):
	def __init__(self):
		super( GSContextMenu, self ).__init__()

		self._popupWindow = PopupWindow( spacing=2, shadowType=gtk.SHADOW_OUT )
		self._subMenus = {}

		self.layout = FormLayout()
		self._evBox = gtk.EventBox()
		self._evBox.connect( 'button-press-event', self._p_onEvBoxButtonPress )
		self._evBox.show()

		self._popupWindow.contents = self._evBox

		self._bFinalised = False



	def item(self, labelText):
		item = GSContextMenuItem( labelText=labelText )
		item.activateSignal.connect( self._p_onItemActivate )
		return item


	def submenuItem(self, labelText, submenu):
		label = gtk.Label( labelText )
		arrow = gtk.Arrow( gtk.ARROW_RIGHT, gtk.SHADOW_NONE )
		box = gtk.HBox()
		box.pack_start( label )
		box.pack_start( arrow )
		box.show_all()
		item = GSContextMenuItem( contents=box )
		item.activateSignal.connect( self._p_onSubmenuActivate )
		self._subMenus[item] = submenu
		return item



	def popup(self):
		self._p_finalise()
		self._popupWindow.popupAtPointer( PopupWindow.CLICK )


	def _p_popupAsSubmenu(self, parentItem):
		self._p_finalise()
		self._popupWindow.popupAdjacentTo( parentItem, gtk.DIR_RIGHT, PopupWindow.CLICK )



	def _p_finalise(self):
		if not self._bFinalised:
			self.layout.finalise()
			self.layout.getWidget().show()

			self._evBox.add( self.layout.getWidget() )
			self._bFinalised = True


	def _p_onItemActivate(self, event):
		self._popupWindow.hide()

	def _p_onSubmenuActivate(self, event):
		subMenu = self._subMenus[event.controller]
		self._popupWindow.hide()
		subMenu.popup()


	def _p_onEvBoxButtonPress(self, widget, event):
		self._popupWindow.hide()





if __name__ == '__main__':
	def onPopup(widget):
		mainMenu.popup()

	def onDelete(widget, event):
		gtk.main_quit()


	subMenu = GSContextMenu()
	subMenu.layout.row()  <<  'A'
	subMenu.layout.row()  <<  'B'
	subMenu.layout.row()  <<  'C'

	mainMenu = GSContextMenu()
	mainMenu.layout.row()  <<  'Section 1'
	mainMenu.layout.row()  <<  mainMenu.submenuItem( '1', subMenu )
	mainMenu.layout.row()  <<  mainMenu.item( '2' )
	mainMenu.layout.row()  <<  mainMenu.item( '3' )
	mainMenu.layout.row()  <<  mainMenu.item( '4' )


	popupButton = gtk.Button( 'Popup' )
	popupButton.connect( 'clicked', onPopup )
	popupButton.show()

	window = gtk.Window( gtk.WINDOW_TOPLEVEL )
	window.set_border_width( 20 )
	window.add( popupButton )
	window.show()
	window.connect( 'delete-event', onDelete )

	gtk.main()






