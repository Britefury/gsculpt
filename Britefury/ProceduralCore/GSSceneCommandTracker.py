##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CommandHistory import CommandHistory
from Britefury.CommandHistory.CommandTracker import CommandTracker

from GSSceneNodeCommandTracker import GSSceneNodeCommandTracker
from Britefury.SheetGraph.SheetGraphCommandTracker import SheetGraphCommandTracker

from Britefury.View.BackgroundImageCommandTracker import BackgroundImageListCommandTracker

import GSScene





class GSSceneCommandTracker (SheetGraphCommandTracker):
	def track(self, scene):
		super( GSSceneCommandTracker, self ).track( scene )
		assert isinstance( scene, GSScene.GSScene )

		self._commandHistory.track( scene.backgroundModels )
		self._commandHistory.track( scene.backgroundImages )
		self._commandHistory.track( scene.layerTable )
		self._commandHistory.track( scene._graphViewDisplayTable )


	def stopTracking(self, scene):
		assert isinstance( scene, GSScene.GSScene )

		self._commandHistory.stopTracking( scene._graphViewDisplayTable )
		self._commandHistory.stopTracking( scene.layerTable )
		self._commandHistory.stopTracking( scene.backgroundImages )
		self._commandHistory.stopTracking( scene.backgroundModels )

		super( GSSceneCommandTracker, self ).stopTracking( scene )



