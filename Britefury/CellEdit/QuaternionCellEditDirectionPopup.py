##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Quaternion, Vector3

from Britefury.UI.ButtonPopupMenu import ButtonPopupMenu

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.CellEdit.BasicWidgetSingleCellEdit import BasicWidgetSingleCellEdit






class QuaternionCellEditDirectionPopup (BasicWidgetSingleCellEdit):
	"""Quaternion cell editor - direction popup menu
	@popup - popup menu button"""
	__valueclass__ = Quaternion

	def __init__(self, modellerView, labelText):
		super( QuaternionCellEditDirectionPopup, self ).__init__()

		self.popup = ButtonPopupMenu( labelText=labelText )
		self.popup.menuSignal.connect( self._p_onPopup )
		self.popup.addMenuItem( Vector3( -1.0, 0.0, 0.0 ), '-X' )
		self.popup.addMenuItem( Vector3( 1.0, 0.0, 0.0 ), '+X' )
		self.popup.addMenuItem( Vector3( 0.0, -1.0, 0.0 ), '-Y' )
		self.popup.addMenuItem( Vector3( 0.0, 1.0, 0.0 ), '+Y' )
		self.popup.addMenuItem( Vector3( 0.0, 0.0, -1.0 ), '-Z' )
		self.popup.addMenuItem( Vector3( 0.0, 0.0, 1.0 ), '+Z' )
		self.popup.addMenuItem( lambda: modellerView.currentViewport.camera.cameraPosition.forwardAxis, _( 'Camera forward' ) )
		self.popup.addMenuItem( lambda: -modellerView.currentViewport.camera.cameraPosition.forwardAxis, _( 'Camera back' ) )
		self.popup.addMenuItem( lambda: -modellerView.currentViewport.camera.cameraPosition.rightAxis, _( 'Camera left' ) )
		self.popup.addMenuItem( lambda: modellerView.currentViewport.camera.cameraPosition.rightAxis, _( 'Camera right' ) )
		self.popup.addMenuItem( lambda: -modellerView.currentViewport.camera.cameraPosition.upAxis, _( 'Camera down' ) )
		self.popup.addMenuItem( lambda: modellerView.currentViewport.camera.cameraPosition.upAxis, _( 'Camera up' ) )

		self._o_setWidgetSensitivity( False )


	def show(self):
		self.popup.getWidget().show()

	def hide(self):
		self.popup.getWidget().hide()


	def _o_setWidgetSensitivity(self, bSensitive):
		self.popup.getWidget().set_sensitive( bSensitive )


	def _o_setWidgetValue(self, value):
		pass


	def _p_onPopup(self, event, choice):
		assert self._cell is not None
		if event.bUserEvent:
			if self._cell.isLiteral()  and  self._cell.isValid():
				direction = None
				if isinstance( choice, Vector3 ):
					direction = choice
				elif callable( choice ):
					direction = choice()
				else:
					raise TypeError, 'choice is invalid'
				value = Quaternion.axisToAxis( Vector3( 0.0, 0.0, 1.0 ), direction )
				if value != self._cell.literalValue:
					self._o_blockCell( value )
					self._cell.literalValue = value
					self._o_unblockCell()

