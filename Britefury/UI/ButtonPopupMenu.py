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

import GSImage

from UIController import UIController




class ButtonPopupMenu (UIController):
	menuSignal = ClassSignal()


	def __init__(self, labelText=None, gsImageFilename=None, contents=None, stock=None):
		super( ButtonPopupMenu, self ).__init__()
		if stock is not None:
			self._button = gtk.Button( stock=stock )
			self._contents = None
			self._contentsBox = None
			self._contentsWidget = None
			self._stock = stock
		else:
			self._button = gtk.Button()
			self._contents = None
			self._contentsBox = None
			self._contentsWidget = None
			self._stock = None
			self.setButtonContents( labelText, gsImageFilename, contents )
		self._button.connect( 'button-press-event', self._p_onButton )

		self._menu = gtk.Menu()
		self._menuItemToResponse = {}



	def getWidget(self):
		return self._button


	def setButtonContents(self, labelText=None, gsImageFilename=None, contents=None):
		if self._contents is not None:
			self._button.remove( self._contents )
			self._contents = None

		if self._contentsBox is not None  and  self._contentsWidget is not None:
			self._contentsBox.remove( self._contentsWidget )
			self._contentsBox = None
			self._contentsWidget = None

		self._contents = self._p_createContentsWidget( True, labelText, gsImageFilename, contents )

		if self._contents is not None:
			self._button.add( self._contents )



	def addMenuItem(self, responseValue, labelText=None, gsImageFilename=None, contents=None):
		itemContents = self._p_createContentsWidget( False, labelText, gsImageFilename, contents )
		item = gtk.MenuItem()
		item.add( itemContents )
		item.connect( 'activate', self._p_onItemActivate )
		item.show()
		self._menu.append( item )
		self._menuItemToResponse[item] = responseValue



	def _p_onButton(self, button, event):
		if event.button == 1:
			self._menu.popup( None, None, None, event.button, event.time )


	def _p_onItemActivate(self, item):
		try:
			response = self._menuItemToResponse[item]
		except KeyError:
			pass
		else:
			self.menuSignal.emit( self.uiEvent(), response )


	def _p_createContentsWidget(self, bForButton, labelText=None, gsImageFilename=None, contents=None):
		widget = None
		if labelText is not None:
			widget = gtk.Label()
			widget.set_markup( labelText )
			widget.show()
		elif gsImageFilename is not None:
			widget = GSImage.getGSImageWidget( gsImageFilename )
			widget.show()
		elif contents is not None:
			widget = contents
		else:
			widget = None

		if bForButton:
			arrow = gtk.Arrow( gtk.ARROW_DOWN, gtk.SHADOW_NONE )
			arrow.show()
			self._contentsBox = gtk.HBox()
			if widget is not None:
				self._contentsBox.pack_start( widget, True, True )
			self._contentsBox.pack_end( arrow, False, False )
			self._contentsBox.show()
			self._contentsWidget = widget
			return self._contentsBox
		else:
			return widget
