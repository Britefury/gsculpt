##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.UI import GSImage
from Britefury.UI.PopupWindow import PopupWindow

from Britefury.Tool.Toolbox import Toolbox




class GSSceneNodeToolbox (Toolbox):
	def __init__(self, pageName, acceleratorManager, view, editorSettings, parentWindow, commandHistory):
		super( GSSceneNodeToolbox, self ).__init__( pageName, acceleratorManager, view )

		self._sceneNode = None



	def attachSceneNode(self, sceneNode):
		assert self._sceneNode is None, 'scene node already attached'
		self._sceneNode = sceneNode


	def detachSceneNode(self):
		assert self._sceneNode is not None, 'no scene node attached'
		self._sceneNode = None





class GSSceneNodeToolboxWithNodeViewEditor (GSSceneNodeToolbox):
	nodeViewSettingsClass = None
	nodeViewSettingsEditorClass = None

	def __init__(self, pageName, acceleratorManager, view, editorSettings, parentWindow, commandHistory):
		super( GSSceneNodeToolboxWithNodeViewEditor, self ).__init__( pageName, acceleratorManager, view, editorSettings, parentWindow, commandHistory )

		assert self.nodeViewSettingsClass is not None
		assert self.nodeViewSettingsEditorClass is not None

		tooltips = gtk.Tooltips()

		self._nodeViewSettingsButton = gtk.Button()
		image = GSImage.getGSImageWidget( 'node_view_settings.png' )
		self._nodeViewSettingsButton.add( image )
		self._nodeViewSettingsButton.show()
		self._nodeViewSettingsButton.connect( 'clicked', self._p_onNodeViewSettingsButton )
		tooltips.set_tip( self._nodeViewSettingsButton, _( 'Node view settings' ), _( 'Node view settings' ) )

		self._nodeViewSettingsButtonBox = gtk.HBox()
		self._nodeViewSettingsButtonBox.pack_start( self._nodeViewSettingsButton, False, False, 5 )
		self._nodeViewSettingsButtonBox.show()

		self._o_packMiscControl( self._nodeViewSettingsButtonBox )


		self._nodeViewSettingsEditor = self.nodeViewSettingsEditorClass( view, editorSettings, parentWindow, commandHistory )

		self._nodeViewSettingsPopup = PopupWindow()
		self._nodeViewSettingsPopup.contents = self._nodeViewSettingsEditor.getSheetEditorWidget()

		self._commandHistory = commandHistory



	def attachSceneNode(self, sceneNode):
		super( GSSceneNodeToolboxWithNodeViewEditor, self ).attachSceneNode( sceneNode )
		nodeViewSettings = self._sceneNode.getNodeViewSettingsForClass( self.nodeViewSettingsClass )
		if nodeViewSettings is None:
			nodeViewSettings = self.nodeViewSettingsClass()
			self._commandHistory.freezeJoin()
			self._sceneNode.setNodeViewSettings( nodeViewSettings )
			self._commandHistory.thaw()
		self._nodeViewSettingsEditor.sheet = nodeViewSettings


	def detachSceneNode(self):
		assert self._sceneNode is not None, 'no scene node attached'
		self._nodeViewSettingsEditor.sheet = None

		super( GSSceneNodeToolboxWithNodeViewEditor, self ).detachSceneNode()



	def _p_onNodeViewSettingsButton(self, widget):
		self._nodeViewSettingsPopup.popupAdjacentTo( widget, gtk.DIR_DOWN, PopupWindow.CLICK )




