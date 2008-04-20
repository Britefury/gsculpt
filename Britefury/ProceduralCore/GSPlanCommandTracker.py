##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import copy

from Britefury.CommandHistory import CommandHistory
from Britefury.CommandHistory.CommandTracker import CommandTracker

from GSProcedureCommandTracker import GSProcedureCommandTracker

import GSPlan




class GSPlanAppendCommand (CommandHistory.Command):
	def __init__(self, plan, proc):
		super( GSPlanAppendCommand, self ).__init__()
		self._plan = plan
		self._proc = proc


	def execute(self):
		self._plan.append( self._proc )

	def unexecute(self):
		self._plan.remove( self._proc )



class GSPlanInsertCommand (CommandHistory.Command):
	def __init__(self, plan, index, proc):
		super( GSPlanInsertCommand, self ).__init__()
		self._plan = plan
		self._index = index
		self._proc = proc


	def execute(self):
		self._plan.insert( self._index, self._proc )

	def unexecute(self):
		self._plan.remove( self._proc )



class GSPlanSetCommand (CommandHistory.Command):
	def __init__(self, plan, index, oldProc, newProc):
		super( GSPlanSetCommand, self ).__init__()
		self._plan = plan
		self._index = index
		self._oldProc = oldProc
		self._newProc = newProc


	def execute(self):
		self._plan[self._index] = newProc

	def unexecute(self):
		self._plan[self._index] = oldProc



class GSPlanSetSliceCommand (CommandHistory.Command):
	def __init__(self, plan, i, j, oldProcs, newProcs):
		super( GSPlanSetSliceCommand, self ).__init__()
		self._plan = plan
		self._i = i
		self._beforeJ = j
		self._afterJ = self._i + len( newProcs )
		self._oldProcs = copy( oldProcs )
		self._newProcs = copy( newProcs )


	def execute(self):
		self._plan[self._i:self._beforeJ] = self._newProcs

	def unexecute(self):
		self._plan[self._i:self._afterJ] = self._oldProcs



class GSPlanRemoveCommand (CommandHistory.Command):
	def __init__(self, plan, index, proc):
		super( GSPlanRemoveCommand, self ).__init__()
		self._plan = plan
		self._index = index
		self._proc = proc


	def execute(self):
		del self._plan[self._index]

	def unexecute(self):
		self._plan.insert( self._index, self._proc )



class GSPlanSwapCommand (CommandHistory.Command):
	def __init__(self, plan, index1, index2):
		super( GSPlanSwapCommand, self ).__init__()
		self._plan = plan
		self._index1 = index1
		self._index2 = index2


	def execute(self):
		self._plan.swap( self._index1, self._index2 )

	def unexecute(self):
		self._plan.swap( self._index1, self._index2 )







class GSPlanCommandTracker (CommandTracker):
	def track(self, plan):
		super( GSPlanCommandTracker, self ).track( plan )
		assert isinstance( plan, GSPlan.GSPlan )

		for proc in plan:
			self._commandHistory.track( proc )



	def stopTracking(self, plan):
		assert isinstance( plan, GSPlan.GSPlan )

		for proc in plan:
			self._commandHistory.stopTracking( proc )

		super( GSPlanCommandTracker, self ).stopTracking( plan )




	def _f_onPlanProcAppended(self, plan, index, proc):
		self._commandHistory.track( proc )
		self._commandHistory.addCommand( GSPlanAppendCommand( plan, proc ) )

	def _f_onPlanProcInserted(self, plan, index, proc):
		self._commandHistory.track( proc )
		self._commandHistory.addCommand( GSPlanInsertCommand( plan, index, proc ) )

	def _f_onPlanProcSet(self, plan, index, oldProc, newProc):
		self._commandHistory.stopTracking( oldProc )
		self._commandHistory.track( newProc )
		self._commandHistory.addCommand( GSPlanSetCommand( plan, index, oldProc, newProc ) )

	def _f_onPlanProcSetSlice(self, plan, i, j, oldProcs, newProcs):
		for oldProc in oldProcs:
			self._commandHistory.stopTracking( oldProc )
		for newProc in newProcs:
			self._commandHistory.track( newProc )
		self._commandHistory.addCommand( GSPlanSetSliceCommand( plan, i, j, oldProcs, newProcs ) )

	def _f_onPlanProcRemoved(self, plan, index, proc):
		self._commandHistory.addCommand( GSPlanRemoveCommand( plan, index, proc ) )
		self._commandHistory.stopTracking( proc )

	def _f_onPlanProcsSwapped(self, plan, index1, index2):
		self._commandHistory.addCommand( GSPlanSwapCommand( plan, index1, index2 ) )


