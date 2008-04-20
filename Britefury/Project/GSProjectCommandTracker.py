##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CommandHistory import CommandHistory
from Britefury.CommandHistory.CommandTracker import CommandTracker

from Britefury.ProceduralCore.GSSceneCommandTracker import GSSceneCommandTracker

import GSProject





class GSProjectCommandTracker (CommandTracker):
	def track(self, project):
		super( GSProjectCommandTracker, self ).track( project )
		assert isinstance( project, GSProject.GSProject )

		self._commandHistory.track( project.scene )
		self._commandHistory.track( project.drawingPlane )


	def stopTracking(self, project):
		assert isinstance( project, GSProject.GSProject )

		self._commandHistory.stopTracking( project.scene )
		self._commandHistory.stopTracking( project.drawingPlane )

		super( GSProjectCommandTracker, self ).stopTracking( project )



