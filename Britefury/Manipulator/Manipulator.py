##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Math.Math import Point3, Quaternion

from Britefury.CellEdit.CellEdit import CellEdit

from Britefury.Event.QueuedEvent import queueEvent



class Manipulator (CellEdit):
	def __init__(self):
		super( Manipulator, self ).__init__()
		self.commandHistory = None
		self._view = None


	def manipAttachToView(self, view):
		assert self._view is None, 'already attached to a view'
		self._view = view

	def manipDetachFromView(self):
		assert self._view is not None, 'not attached to a view'
		self._view = None


	def _o_viewPostRedraw(self):
		if self._view is not None:
			self._view.postRedraw()


	def _p_commandHistoryBreak(self):
		if self.commandHistory is not None:
			self.commandHistory.finishCommand()



