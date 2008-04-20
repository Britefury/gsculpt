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

import BackgroundImage



class BackgroundImageCommandTracker (SheetCommandTracker):
	pass





class BackgroundImageListAppendCommand (CommandHistory.Command):
	def __init__(self, imageList, image):
		super( BackgroundImageListAppendCommand, self ).__init__()
		self._imageList = imageList
		self._image = image


	def execute(self):
		self._imageList.appendImage( self._image )

	def unexecute(self):
		self._imageList.removeImage( self._image )



class BackgroundImageListInsertCommand (CommandHistory.Command):
	def __init__(self, imageList, index, image):
		super( BackgroundImageListInsertCommand, self ).__init__()
		self._imageList = imageList
		self._index = index
		self._image = image


	def execute(self):
		self._imageList.insertImage( self._index, self._image )

	def unexecute(self):
		self._imageList.removeImage( self._image )



class BackgroundImageListRemoveCommand (CommandHistory.Command):
	def __init__(self, imageList, index, image):
		super( BackgroundImageListRemoveCommand, self ).__init__()
		self._imageList = imageList
		self._index = index
		self._image = image


	def execute(self):
		self._imageList.removeImage( self._image )

	def unexecute(self):
		self._imageList.insertImage( self._index, self._image )





class BackgroundImageListCommandTracker (CommandTracker):
	def track(self, imageList):
		super( BackgroundImageListCommandTracker, self ).track( imageList )
		assert isinstance( imageList, BackgroundImage.BackgroundImageList )


	def stopTracking(self, imageList):
		assert isinstance( imageList, BackgroundImage.BackgroundImageList )

		super( BackgroundImageListCommandTracker, self ).stopTracking( imageList )



	def _f_onImageListImageAppended(self, imageList, image):
		self._commandHistory.track( image )
		self._commandHistory.addCommand( BackgroundImageListAppendCommand( imageList, image ) )

	def _f_onImageListImageInserted(self, imageList, index, image):
		self._commandHistory.track( image )
		self._commandHistory.addCommand( BackgroundImageListInsertCommand( imageList, index, image ) )

	def _f_onImageListImageRemoved(self, imageList, image):
		index = imageList.index( image )
		self._commandHistory.addCommand( BackgroundImageListRemoveCommand( imageList, index, image ) )
		self._commandHistory.stopTracking( image )
