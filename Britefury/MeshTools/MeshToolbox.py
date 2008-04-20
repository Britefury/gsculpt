##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.ProceduralTool.GSSceneNodeToolbox import GSSceneNodeToolboxWithNodeViewEditor

from Britefury.MeshTools.MeshEditToolkit import MeshEditToolkit
from Britefury.Mesh.GSSceneNodeMeshViewSettings import GSSceneNodeMeshViewSettings, GSSceneNodeMeshViewSettingsEditor



class MeshToolbox (GSSceneNodeToolboxWithNodeViewEditor):
	nodeViewSettingsClass = GSSceneNodeMeshViewSettings
	nodeViewSettingsEditorClass = GSSceneNodeMeshViewSettingsEditor

	def __init__(self, acceleratorManager, view, nodeEditor, sceneEditorTools, backgroundLayerPainter, toolPanel, editorSettings, parentWindow, commandHistory):
		super( MeshToolbox, self ).__init__( _( 'Mesh' ), acceleratorManager, view, editorSettings, parentWindow, commandHistory )
		meshEditToolkit = MeshEditToolkit( nodeEditor, sceneEditorTools, backgroundLayerPainter, view, parentWindow, toolPanel, commandHistory )
		self.addToolkit( meshEditToolkit, _( 'Mesh edit' ) )

