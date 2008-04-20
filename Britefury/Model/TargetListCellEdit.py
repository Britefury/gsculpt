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

from Britefury.Model.TargetList import TargetList

from Britefury.CellEdit.WidgetSingleCellEdit import *

from Britefury.Event.QueuedEvent import queueEvent



class TargetListCellEdit (WidgetSingleCellEdit):
	def __init__(self, targetLabelText=_( '%d target' ), pluralTargetLabelText=_( '%d targets' ), removeTargetButtonText=_( 'Remove target' )):
		super( TargetListCellEdit, self ).__init__()
		self.commandHistory = None

		self.numTargets = gtk.Label( pluralTargetLabelText  %  ( 0, ) )
		self._targetLabelText = targetLabelText
		self._pluralTargetLabelText = pluralTargetLabelText

		self.removeTarget = gtk.Button( removeTargetButtonText )
		self.removeTarget.connect( 'clicked', self._p_onRemoveTarget )

		self._o_setUISensitivity( False )


	def attachCell(self, cell):
		super( TargetListCellEdit, self ).attachCell( cell )
		self._p_onCellChanged()
		self._cell.changedSignal.connect( self._p_onCellChanged )

	def detachCell(self):
		assert self._cell is not None, 'no cell attached'
		self._cell.changedSignal.disconnect( self._p_onCellChanged )
		super( TargetListCellEdit, self ).detachCell()


	def show(self):
		self.numTargets.show()
		self.removeTarget.show()

	def hide(self):
		self.numTargets.hide()
		self.removeTarget.hide()

	def _o_setUISensitivity(self, bSensitive):
		self.numTargets.set_sensitive( bSensitive )
		self.removeTarget.set_sensitive( bSensitive )


	def _p_onRemoveTarget(self, widget):
		assert self._cell is not None
		x = self._cell.getValue()
		if len( x )  >  0:
			x[:] = x[:-1]
			self._cell.setLiteralValue( x )
			if self.commandHistory is not None:
				self.commandHistory.finishCommand()


	def _p_onCellChanged(self):
		queueEvent( self._p_refreshValue )

	def _p_refreshValue(self):
		if self._cell is not None:
			value = self._cell.getImmutableValue()
			if value is not None:
				if len( value ) != 1:
					text = self._pluralTargetLabelText  %  ( len( value ), )
				else:
					text = self._targetLabelText  %  ( len( value ), )
				self.numTargets.set_text( text )

