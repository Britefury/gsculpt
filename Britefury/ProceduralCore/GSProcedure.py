##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import inspect

from Britefury.Sheet.Sheet import *

from Britefury.ProceduralCore.Product import GSProduct
from Britefury.ProceduralCore.GSFunction import *
from Britefury.ProceduralCore.GSProcedureCommandTracker import GSProcedureCommandTracker





class GSProcedureClass (SheetClass, GSFunctionClass):
	def __init__(cls, name, bases, clsDict):
		super( GSProcedureClass, cls ).__init__( name, bases, clsDict )

		try:
			inputClass = cls.inputClass
		except AttributeError:
		   	raise TypeError, 'GSProcedure subclass %s does not have an \'inputClass\' attribute'  %  ( name, )







class GSProcedure (Sheet, GSFunction):
	"GSProcedure base class"
	__metaclass__ = GSProcedureClass
	trackerClass = GSProcedureCommandTracker
	description = _( 'Abstract procedure' )


	inputClass = GSProduct


	def __init__(self, src=None):
		"""Constructor:
		@src - source object to copy"""
		super( GSProcedure, self ).__init__( src )
		self.r_plan = None
		self.r_indexInPlan = -1




	def setPlan(self, plan, indexInPlan):
		"Set the plan which owns this procedure, along with the index of the procedure in the plan"
		self.r_plan = plan
		self.r_indexInPlan = indexInPlan

	def setIndexInPlan(self, plan, indexInPlan):
		"Set the index of this procedure in the plan that owns it"
		assert plan == self.r_plan, 'plans do not match'
		self.r_indexInPlan = indexInPlan


	def _f_onChildCellModified(self):
		"Notify of field value change"
		super( GSProcedure, self )._f_onChildCellModified()
		if self.r_plan is not None:
			self.r_plan._p_onProcedureModify( self )



	def canOperateOn(self, inputObject):
		"Determine if the object @inputObject can be operated on"
		return self.canOperateOnClass( type( inputObject ) )


	def canOperateOnClass(self, inputClass):
		"Determine if an object of class @inputClass can be operated on"
		if isinstance( self.inputClass, list ):
			for cls in self.inputClass:
				if issubclass( inputClass, cls ):
					return True
			return False
		elif isinstance( self.inputClass, type ):
			return issubclass( inputClass, self.inputClass )
		else:
			raise TypeError, 'Could not process %s.inputClass' % ( type( self ).__name__, )



	def getOutputClass(self, inputClass):
		"Determine the class of the output object given the class of the input object"
		if self.canOperateOnClass( inputClass ):
			try:
				outputClass = self.outputClass
			except AttributeError:
				return inputClass
			else:
				if isinstance( outputClass, dict ):
					return outputClass[inputClass]
				elif isinstance( outputClass, type ):
					return outputClass
				else:
					raise TypeError, 'Could not process %s.outputClass' % ( type( self ).__name__, )
		else:
			return None


	def invoke(self, inputObject, errorHandler):
		"Invoke the procedure on the given input object"
		if not self.canOperateOn( inputObject ):
			return None
		else:
			return self.procedureInvoke( inputObject, errorHandler )









if __name__ == '__main__':
	class SimpeProcedure (GSProcedure):
		a = Field( int, 1 )

		def procedureInvoke(self, inputObject, errorHandler):
			return inputObject + self.a

	proc = SimpeProcedure()
	proc.a = 42
	print proc.invoke( 10, None )




	class BiggerProcedure (GSProcedure):
		a = Field( int, 1 )
		b = Field( int, 1 )
		c = Field( int, 1 )
		d = Field( int, 1 )
		e = Field( int, 1 )

		def invoke(inputObject, errorHandler):
			return inputObject




	import time



	proc2 = BiggerProcedure()

	numIter = 10240
	startTime = time.time()
	for i in xrange( 0, numIter ):
		proc2.canOperateOnClass( int )
		proc2.getOutputClass( int )
		proc2.invoke( 10, None )
	endTime = time.time()
	timeElapsed = endTime - startTime

	print '%d iterations took %f seconds; %f iterations per second' % ( numIter, timeElapsed, float(numIter)/timeElapsed )

	for i in xrange( 10, 0, -1 ):
		print i
		time.sleep( 1.0 )

	numProcs = 10240
	procs = []
	for i in xrange( 0, numProcs ):
		procs.append( BiggerProcedure() )

	for i in xrange( 10, 0, -1 ):
		print i
		time.sleep( 1.0 )





