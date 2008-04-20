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


from Britefury.UI.FormLayout import FormLayout
from Britefury.UI.PopupWindow import PopupWindow

from Britefury.CellEdit.FloatCellEditEntry import FloatCellEditEntry





class SnapSettingsPopup (object):
	class Row (object):
		def __init__(self, minValue, maxValue, step, page):
			self._manipulatorSnapping = None
			self.aEntry = FloatCellEditEntry( minValue, maxValue, step, page, 4 )
			self.bEntry = FloatCellEditEntry( minValue, maxValue, step, page, 4 )
			self.abEntry = FloatCellEditEntry( minValue, maxValue, step, page, 4 )
			self.aEntry.entry.show()
			self.bEntry.entry.show()
			self.abEntry.entry.show()


		def attachManipulatorSnapping(self, manipulatorSnapping):
			self._manipulatorSnapping = manipulatorSnapping
			self.aEntry.attachCell( self._manipulatorSnapping.aGranularity )
			self.bEntry.attachCell( self._manipulatorSnapping.bGranularity )
			self.abEntry.attachCell( self._manipulatorSnapping.abGranularity )

		def detachManipulatorSnapping(self):
			self.aEntry.detachCell()
			self.bEntry.detachCell()
			self.abEntry.detachCell()
			self._manipulatorSnapping = None





	def __init__(self):
		self._snapSettings = None
		self._distRow = SnapSettingsPopup.Row( 0.0, 1.0e18, 0.001, 0.01 )
		self._fractionRow = SnapSettingsPopup.Row( 0.0, 1.0, 0.0001, 0.001 )
		self._angleRow = SnapSettingsPopup.Row( 0.0, 3.6e8, 0.001, 0.01 )
		self._layout = FormLayout()
		self._layout.row()  <<  None  <<  _( 'Ctrl+Shift' ) <<  _( 'Ctrl' )  <<  _( 'Shift' )
		self._layout.row()  <<  _( 'Distance / offset' )  <<  self._distRow.abEntry.entry  <<  self._distRow.aEntry.entry  <<  self._distRow.bEntry.entry
		self._layout.row()  <<  _( 'Fraction / scale' )  <<  self._fractionRow.abEntry.entry  <<  self._fractionRow.aEntry.entry  <<  self._fractionRow.bEntry.entry
		self._layout.row()  <<  _( 'Angle / rotation' )  <<  self._angleRow.abEntry.entry  <<  self._angleRow.aEntry.entry  <<  self._angleRow.bEntry.entry
		self._layout.finalise()
		self._layout.getWidget().show()
		self._popupWindow = PopupWindow()
		self._popupWindow.contents = self._layout.getWidget()



	def popup(self, belowWidget):
		self._popupWindow.popupAdjacentTo( belowWidget, gtk.DIR_DOWN, PopupWindow.CLICK )





	def attachSnapSettings(self, snapSettings):
		assert self._snapSettings is None, 'snap settings already attached'
		self._snapSettings = snapSettings
		self._distRow.attachManipulatorSnapping( self._snapSettings.distance )
		self._fractionRow.attachManipulatorSnapping( self._snapSettings.fraction )
		self._angleRow.attachManipulatorSnapping( self._snapSettings.angle )


	def detachSnapSettings(self):
		assert self._snapSettings is not None, 'no snap settings attached'
		self._distRow.detachManipulatorSnapping()
		self._fractionRow.detachManipulatorSnapping()
		self._angleRow.detachManipulatorSnapping()
		self._snapSettings = None
