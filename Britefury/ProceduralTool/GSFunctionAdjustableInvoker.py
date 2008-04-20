##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Sheet.Sheet import *




class GSFunctionAdjustableInvokerClass (SheetClass):
	def __init__(cls, clsName, bases, clsDict):
		super( GSFunctionAdjustableInvokerClass, cls ).__init__( clsName, bases, clsDict )

		try:
			functionClass = clsDict['functionClass']
		except KeyError:
			raise TypeError, 'functionClass not specified'




class GSFunctionAdjustableInvoker (Sheet):
	"""
	Steps:
	1
		Initialisation			initInvoke()
	2
		Main					main()
		Save				save()
	3
		Restore				restore()
		Adjust				adjust()
	"""

	__metaclass__ = GSFunctionAdjustableInvokerClass
	functionClass = None


	def _inputStage(self):
		if self._functionCell.getValue() is not None:
			# Subclass.getInput() will get and store input(s)
			self.getInput()

	def _initialisationStage(self):
		if self._functionCell.getValue() is not None:
			# input stage is a pre-requisite
			self.inputStage
			# initialisation
			self.initInvoke()

	def _mainStage(self):
		if self._functionCell.getValue() is not None:
			# initialisation stage is a pre-requisite
			self.initialisationStage
			# main
			self.main()
			# save state (adjustment)
			self.save()

	def _adjustmentStage(self):
		if self._functionCell.getValue() is not None:
			# main stage is a pre-requisite
			self.mainStage
			# restore saved state
			self.restore()
			# adjust state
			self.adjust()

	def _resultStage(self):
		if self._functionCell.getValue() is not None:
			# adjustment stage is a pre-requisite
			self.adjustmentStage
			# return the result
			return self.getResult()
		else:
			return None


	inputStage = FunctionRefField( _inputStage )
	initialisationStage = FunctionRefField( _initialisationStage )
	mainStage = FunctionRefField( _mainStage )
	adjustmentStage = FunctionRefField( _adjustmentStage )
	result = FunctionRefField( _resultStage )




	def getInput(self):
		pass

	def initInvoke(self):
		pass

	def main(self):
		pass

	def save(self):
		pass

	def restore(self):
		pass

	def adjust(self):
		pass

	def getResult(self):
		return None



	def __init__(self, functionCell):
		super( GSFunctionAdjustableInvoker, self ).__init__()
		self._functionCell = functionCell


	def shutdownAdjustableInvoker(self):
		pass









if __name__ == '__main__':
	import unittest


	class TestSheet (Sheet):
		a = Field( int, 1 )
		b = Field( int, 2 )
		c = Field( int, 3 )
		d = Field( int, 5 )



	class TestFunctionAdjustableInvoker (GSFunctionAdjustableInvoker):
		functionClass = TestSheet

		def __init__(self, function, tester):
			super( TestFunctionAdjustableInvoker, self ).__init__( function )
			self._tester = tester


		def getInput(self):
			self._tester.message( 'getInput' )
			self._input = [ self._function.a ]

		def initInvoke(self):
			self._tester.message( 'initInvoke' )
			self._a = self._input + [ self._function.b ]

		def main(self):
			self._tester.message( 'main' )
			self._b = self._a + [ self._function.c ]

		def save(self):
			self._tester.message( 'save' )
			self._x = self._b[2]

		def restore(self):
			self._tester.message( 'restore' )
			self._b[2] = self._x

		def adjust(self):
			self._tester.message( 'adjust' )
			self._b[2] *= self._function.d

		def getResult(self):
			self._tester.message( 'getResult' )
			return self._b




	class FunctionAdjustableInvokerTest (unittest.TestCase):
		def setUp(self):
			self._sigs = {}

		def tearDown(self):
			self._sigs = {}

		def message(self, name):
			self._sigs[name] = self._sigs.get( name, 0 ) + 1

		def received(self, name):
			return self._sigs.get( name, 0 )

		def receivedAndReset(self, name):
			return self._sigs.pop( name, 0 )

		def wasReceivedAndReset(self, name):
			return self._sigs.pop( name, 0 ) != 0

		def reset(self, name):
			self._sigs.pop( name, 0 )




		def testResult(self):
			sheet = TestSheet()
			invoker = TestFunctionAdjustableInvoker( sheet, self )

			self.assert_( invoker.result == [ 1, 2, 15 ] )
			self.assert_( self.receivedAndReset( 'getInput' ) )
			self.assert_( self.receivedAndReset( 'initInvoke' ) )
			self.assert_( self.receivedAndReset( 'main' ) )
			self.assert_( self.receivedAndReset( 'save' ) )
			self.assert_( self.receivedAndReset( 'restore' ) )
			self.assert_( self.receivedAndReset( 'adjust' ) )
			self.assert_( self.receivedAndReset( 'getResult' ) )



		def testStages(self):
			sheet = TestSheet()
			invoker = TestFunctionAdjustableInvoker( sheet, self )

			self.assert_( invoker.result == [ 1, 2, 15 ] )
			self.assert_( self.receivedAndReset( 'getInput' ) )
			self.assert_( self.receivedAndReset( 'initInvoke' ) )
			self.assert_( self.receivedAndReset( 'main' ) )
			self.assert_( self.receivedAndReset( 'save' ) )
			self.assert_( self.receivedAndReset( 'restore' ) )
			self.assert_( self.receivedAndReset( 'adjust' ) )
			self.assert_( self.receivedAndReset( 'getResult' ) )


			sheet.a = 27
			self.assert_( invoker.result == [ 27, 2, 15 ] )
			self.assert_( self.receivedAndReset( 'getInput' ) )
			self.assert_( self.receivedAndReset( 'initInvoke' ) )
			self.assert_( self.receivedAndReset( 'main' ) )
			self.assert_( self.receivedAndReset( 'save' ) )
			self.assert_( self.receivedAndReset( 'restore' ) )
			self.assert_( self.receivedAndReset( 'adjust' ) )
			self.assert_( self.receivedAndReset( 'getResult' ) )



			sheet.b = 4
			self.assert_( invoker.result == [ 27, 4, 15 ] )
			self.assert_( not self.receivedAndReset( 'getInput' ) )
			self.assert_( self.receivedAndReset( 'initInvoke' ) )
			self.assert_( self.receivedAndReset( 'main' ) )
			self.assert_( self.receivedAndReset( 'save' ) )
			self.assert_( self.receivedAndReset( 'restore' ) )
			self.assert_( self.receivedAndReset( 'adjust' ) )
			self.assert_( self.receivedAndReset( 'getResult' ) )


			sheet.c = 6
			self.assert_( invoker.result == [ 27, 4, 30 ] )
			self.assert_( not self.receivedAndReset( 'getInput' ) )
			self.assert_( not self.receivedAndReset( 'initInvoke' ) )
			self.assert_( self.receivedAndReset( 'main' ) )
			self.assert_( self.receivedAndReset( 'save' ) )
			self.assert_( self.receivedAndReset( 'restore' ) )
			self.assert_( self.receivedAndReset( 'adjust' ) )
			self.assert_( self.receivedAndReset( 'getResult' ) )


			sheet.d = 8
			self.assert_( invoker.result == [ 27, 4, 48 ] )
			self.assert_( not self.receivedAndReset( 'getInput' ) )
			self.assert_( not self.receivedAndReset( 'initInvoke' ) )
			self.assert_( not self.receivedAndReset( 'main' ) )
			self.assert_( not self.receivedAndReset( 'save' ) )
			self.assert_( self.receivedAndReset( 'restore' ) )
			self.assert_( self.receivedAndReset( 'adjust' ) )
			self.assert_( self.receivedAndReset( 'getResult' ) )


			self.assert_( invoker.result == [ 27, 4, 48 ] )
			self.assert_( not self.receivedAndReset( 'getInput' ) )
			self.assert_( not self.receivedAndReset( 'initInvoke' ) )
			self.assert_( not self.receivedAndReset( 'main' ) )
			self.assert_( not self.receivedAndReset( 'save' ) )
			self.assert_( not self.receivedAndReset( 'restore' ) )
			self.assert_( not self.receivedAndReset( 'adjust' ) )
			self.assert_( not self.receivedAndReset( 'getResult' ) )


	unittest.main()