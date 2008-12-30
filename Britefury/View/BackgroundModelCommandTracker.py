##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CommandHistory import CommandHistory

from Britefury.CommandHistory.CommandTracker import CommandTracker

from Britefury.Sheet.SheetCommandTracker import SheetCommandTracker

import BackgroundModel



class BackgroundModelCommandTracker (SheetCommandTracker):
	pass





class BackgroundModelListAppendCommand (CommandHistory.Command):
	def __init__(self, modelList, model):
		super( BackgroundModelListAppendCommand, self ).__init__()
		self._modelList = modelList
		self._model = model


	def execute(self):
		self._modelList.appendModel( self._model )

	def unexecute(self):
		self._modelList.removeModel( self._model )



class BackgroundModelListInsertCommand (CommandHistory.Command):
	def __init__(self, modelList, index, model):
		super( BackgroundModelListInsertCommand, self ).__init__()
		self._modelList = modelList
		self._index = index
		self._model = model


	def execute(self):
		self._modelList.insertModel( self._index, self._model )

	def unexecute(self):
		self._modelList.removeModel( self._model )



class BackgroundModelListRemoveCommand (CommandHistory.Command):
	def __init__(self, modelList, index, model):
		super( BackgroundModelListRemoveCommand, self ).__init__()
		self._modelList = modelList
		self._index = index
		self._model = model


	def execute(self):
		self._modelList.removeModel( self._model )

	def unexecute(self):
		self._modelList.insertModel( self._index, self._model )





class BackgroundModelListCommandTracker (CommandTracker):
	def track(self, modelList):
		super( BackgroundModelListCommandTracker, self ).track( modelList )
		assert isinstance( modelList, BackgroundModel.BackgroundModelList )


	def stopTracking(self, modelList):
		assert isinstance( modelList, BackgroundModel.BackgroundModelList )

		super( BackgroundModelListCommandTracker, self ).stopTracking( modelList )



	def _f_onModelListModelAppended(self, modelList, model):
		self._commandHistory.track( model )
		self._commandHistory.addCommand( BackgroundModelListAppendCommand( modelList, model ) )

	def _f_onModelListModelInserted(self, modelList, index, model):
		self._commandHistory.track( model )
		self._commandHistory.addCommand( BackgroundModelListInsertCommand( modelList, index, model ) )

	def _f_onModelListModelRemoved(self, modelList, model):
		index = modelList.index( model )
		self._commandHistory.addCommand( BackgroundModelListRemoveCommand( modelList, index, model ) )
		self._commandHistory.stopTracking( model )
