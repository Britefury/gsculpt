##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Event.QueuedEvent import queueEvent

from Britefury.Cell.CellInterface import CellInterface










class CellEdit (object):
	def __init__(self):
		super( CellEdit, self ).__init__()
		self._sensitivity = True



	def setSensitivity(self, sensitivity):
		assert isinstance( sensitivity, bool )  or  isinstance( sensitivity, CellInterface ), 'sensitivity must be a boolean, or a cell'
		if self._sensitivity is not None  and  isinstance( self._sensitivity, CellInterface ):
			self._sensitivity.changedSignal.disconnect( self._p_onSensitivityCell )
		self._sensitivity = sensitivity
		if self._sensitivity is not None  and  isinstance( self._sensitivity, CellInterface ):
			self._sensitivity.changedSignal.connect( self._p_onSensitivityCell )
		self._o_refreshSensitivity()

	def _o_refreshSensitivity(self):
		if isinstance( self._sensitivity, CellInterface ):
			bUserSensitivity = self._sensitivity.value
		else:
			bUserSensitivity = self._sensitivity
		if not isinstance( bUserSensitivity, bool ):
			bUserSensitivity = True
		bSensitive = bUserSensitivity  and  self._o_isValid()
		self._o_setUISensitivity( bSensitive )


	def _o_watchCellValidity(self, cell):
		cell.validitySignal.connect( self._o_onCellValidity )

	def _o_stopWatchingCellValidity(self, cell):
		cell.validitySignal.disconnect( self._o_onCellValidity )

	def _o_onCellValidity(self):
		queueEvent( self._o_refreshSensitivity )

	def _p_onSensitivityCell(self):
		queueEvent( self._o_refreshSensitivity )


	sensitivity = property( None, setSensitivity )
