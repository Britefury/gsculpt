##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Util.SignalSlot import *

from Britefury.Cell.LiteralCell import *

from Britefury.Sheet.Sheet import *
from Britefury.SheetEdit.SheetEditorTool import *

from Britefury.ProceduralCore.GSFunction import *

from Britefury.ProceduralTool.GSFunctionAdjustableInvoker import GSFunctionAdjustableInvoker








class FunctionToolProductView (object):
	def installEventHandlers(self, view):
		pass

	def uninstallEventHandlers(self, view):
		pass


	def installPainters(self, view):
		pass

	def uninstallPainters(self, view):
		pass


	def _p_getOutputProductCell(self):
		assert False, 'abstract; overload the "outputProductCell" property'

	def _p_getDisplayedProduct(self):
		assert False, 'abstract; overload the "displayedProduct" property'


	outputProductCell = property( _p_getOutputProductCell )
	displayedProduct = property( _p_getDisplayedProduct )





g_functionToolClassTable = {}




class GSFunctionToolClass (SheetEditorToolClass):
	def __init__(cls, clsName, bases, clsDict):
		super( GSFunctionToolClass, cls ).__init__( clsName, bases, clsDict )

		global g_functionToolClassTable
		if not clsDict.has_key( 'functionClass' ):
			raise AttributeError, 'functionClass not specified'

		if not ( isinstance( cls.functionClass, GSFunctionClass )  and  issubclass( cls.functionClass, GSFunction ) ):
			raise TypeError, 'functionClass must be a subclass of GSFunction and an instance of GSFunctionClass'


		if clsDict.has_key( 'adjustableInvoker' )  and  cls.adjustableInvoker is not None  and  not issubclass( cls.adjustableInvoker, GSFunctionAdjustableInvoker ):
			raise TypeError, 'adjustableInvoker must be a subclass of GSFunctionAdjustableInvoker'

		if clsDict.has_key( 'adjustableInvoker' )  and  cls.adjustableInvoker is not None  and  cls.adjustableInvoker.functionClass is not cls.functionClass:
			raise TypeError, 'adjustableInvoker functionClass does not match tool functionClass'


		# Class table
		g_functionToolClassTable[cls.functionClass] = cls







class GSFunctionTool (SheetEditorTool):
	"Function tool base class"
	__metaclass__ = GSFunctionToolClass

	functionClass = GSFunction
	pageTitle = _( 'tool' )
	titleText = 'GSFunctionTool'
	adjustableInvoker = None




	def __init__(self, view, editorSettings, parentWindow, commandHistory, painterFactory):
		"Constructor"
		self._productViewCell = LiteralRefCell()
		self._painterFactory = painterFactory

		super( GSFunctionTool, self ).__init__( view, editorSettings, parentWindow, commandHistory )

		productView = self._p_createProductView()
		self._productViewCell.literalValue = productView
		productView.installPainters( self._view )




	def shutdownTool(self):
		super( GSFunctionTool, self ).shutdownTool()
		productView = self._productViewCell.value
		if productView is not None:
			productView.uninstallPainters( self._view )
			self._p_shutdownProductView( productView )
			self._productViewCell.literalValue = None



	def onPageSwitchIn(self):
		productView = self._productViewCell.value
		if productView is not None:
			productView.installEventHandlers( self._view )
		super( GSFunctionTool, self ).onPageSwitchIn()


	def onPageSwitchOut(self):
		super( GSFunctionTool, self ).onPageSwitchOut()
		productView = self._productViewCell.value
		if productView is not None:
			productView.uninstallEventHandlers( self._view )



	def _p_createProductView(self):
		assert False, 'abstract'

	def _p_shutdownProductView(self, productView):
		assert False, 'abstract'



	def _f_setAdjustableInvoker(self, invoker):
		self._adjustableInvoker = invoker

	def _f_getAdjustableInvokerClass(self):
		return type( self ).adjustableInvoker



	def _p_getOutputProductCell(self):
		productView = self._productViewCell.value
		if productView is not None:
			return productView.outputProductCell
		else:
			return None

	def _p_getDisplayedProduct(self):
		productView = self._productViewCell.value
		if productView is not None:
			return productView.displayedProduct
		else:
			return None

	def _p_getPainterFactory(self):
		return self._painterFactory

	outputProductCell = property( _p_getOutputProductCell )
	displayedProduct = property( _p_getDisplayedProduct )
	painterFactory = property( _p_getPainterFactory )





#def createFunctionTool(function):
	#global g_functionToolClassTable
	#functionClass = function.__class__
	#toolClass = g_functionToolClassTable[functionClass]
	#return toolClass()


def getFunctionToolClass(function):
	return g_functionToolClassTable[function.__class__]




