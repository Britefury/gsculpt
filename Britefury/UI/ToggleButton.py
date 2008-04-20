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

from Britefury.UI import GSImage
from Britefury.UI.UIController import *



class ToggleButton (UIController):
	toggleSignal = ClassSignal()


	def __init__(self, labelText=None, gsImageFilename=None, contents=None):
		super( ToggleButton, self ).__init__()
		if labelText is not None:
			self._button = gtk.ToggleButton( labelText )
		else:
			self._button = gtk.ToggleButton()
			if gsImageFilename is not None:
				self._button.add( GSImage.getGSImageWidget( gsImageFilename ) )
			elif contents is not None:
				self._button.add( contents )

		self._button.connect( 'clicked', self._p_onClicked )
		self._group = None


	def getWidget(self):
		return self._button



	def _p_onClicked(self, toggle):
		self.toggleSignal.emit( self.uiEvent(), toggle.get_active() )

		if self._group is not None:
			self._group._p_onToggleClicked( self.uiEvent(), toggle.get_active() )



	def _p_getState(self):
		return self._button.get_active()

	def _p_setState(self, state):
		self.applicationEventsBegin()
		self._button.set_active( state )
		self.applicationEventsEnd()


	state = property( _p_getState, _p_setState )


