##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import ClassSignal

from Britefury.WorkArea.ModellerView import VPType


class ViewLayout (object):
	viewportAddSignal = ClassSignal()
	viewportRemoveSignal = ClassSignal()
	viewportSelectSignal = ClassSignal()


	def __init__(self, view, viewportTypeToFactoryAndName):
		self._view = view
		self._viewportTypeToFactoryAndName = viewportTypeToFactoryAndName



	def getWidget(self):
		assert False, 'abstract'


	def setCurrentViewport(self, viewport):
		assert False, 'abstract'

	def getCurrentWrappedViewport(self):
		assert False, 'abstract'


	def initialise(self, currentWrappedViewport):
		assert False, 'abstract'

	def shutdown(self):
		pass

	def getViewports(self):
		assert False, 'abstract'



	def _p_createViewport(self, viewportType = VPType.PERSPECTIVE):
		return self._viewportTypeToFactoryAndName[viewportType][0]( self._view.viewSettings );





