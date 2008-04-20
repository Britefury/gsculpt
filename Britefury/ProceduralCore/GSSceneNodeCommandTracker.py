##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.CommandHistory import CommandHistory

from Britefury.SheetGraph.SheetGraphCommandTracker import *

from Britefury.ProceduralCore.GSPlanCommandTracker import GSPlanCommandTracker
import GSSceneNode






class GSSceneNodeAddViewSettingsCommand (CommandHistory.Command):
	def __init__(self, sceneNode, viewSettings):
		super( GSSceneNodeAddViewSettingsCommand, self ).__init__()
		self._sceneNode = sceneNode
		self._viewSettings = viewSettings


	def execute(self):
		self._sceneNode._p_addNodeViewSettings( self._viewSettings )

	def unexecute(self):
		self._sceneNode._p_removeNodeViewSettings( self._viewSettings )




class GSSceneNodeRemoveViewSettingsCommand (CommandHistory.Command):
	def __init__(self, sceneNode, viewSettings):
		super( GSSceneNodeRemoveViewSettingsCommand, self ).__init__()
		self._sceneNode = sceneNode
		self._viewSettings = viewSettings



	def execute(self):
		self._sceneNode._p_removeNodeViewSettings( self._viewSettings )

	def unexecute(self):
		self._sceneNode._p_addNodeViewSettings( self._viewSettings )





class GSSceneNodeCommandTracker (SheetGraphNodeCommandTracker):
	def track(self, node):
		super( GSSceneNodeCommandTracker, self ).track( node )
		assert isinstance( node, GSSceneNode.GSSceneNode ), 'not a scene node'

		self._commandHistory.track( node.plan )


	def stopTracking(self, node):
		assert isinstance( node, GSSceneNode.GSSceneNode ), 'not a scene node'

		self._commandHistory.stopTracking( node.plan )

		super( GSSceneNodeCommandTracker, self ).stopTracking( node )



	def _f_onSceneNodeViewSettingsAdded(self, sceneNode, viewSettings):
		self._commandHistory.track( viewSettings )
		self._commandHistory.addCommand( GSSceneNodeAddViewSettingsCommand( sceneNode, viewSettings ) )


	def _f_onSceneNodeViewSettingsRemoved(self, sceneNode, viewSettings):
		self._commandHistory.addCommand( GSSceneNodeRemoveViewSettingsCommand( sceneNode, viewSettings ) )
		self._commandHistory.stopTracking( viewSettings )



