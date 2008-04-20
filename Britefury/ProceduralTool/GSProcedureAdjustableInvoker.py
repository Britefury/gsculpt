##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.ProceduralTool.GSFunctionAdjustableInvoker import *


class GSProcedureAdjustableInvoker (GSFunctionAdjustableInvoker):
	functionClass = None

	def __init__(self, procCell, inputProductCell, errorHandler):
		super( GSProcedureAdjustableInvoker, self ).__init__( procCell )
		self._procCell = procCell
		self._inputProductCell = inputProductCell
		self._errorHandler = errorHandler
		self._inputObject = None


	def shutdownAdjustableInvoker(self):
		self._inputObject = None
		super( GSProcedureAdjustableInvoker, self ).shutdownAdjustableInvoker()



	def getInput(self):
		self._inputObject = self._inputProductCell.value



	def _getProc(self):
		return self._procCell.getValue()

	_proc = property( _getProc )