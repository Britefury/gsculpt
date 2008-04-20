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


from Britefury.UI.NameDialog import nameDialog

from Britefury.SceneNodes.SceneModelNode import SceneModelNode
from Britefury.SceneNodes.SceneProcedureNode import SceneProcedureNode
from Britefury.SceneNodes.SceneGroupNode import SceneGroupNode



class NewNodeMenu (object):
	def __init__(self):
		self._parentWindow = None
		self._commandHistory = None
		self._scene = None

		self._menu = gtk.Menu()
		self._menu.show()
		self._itemToEntry = {}
		self._items = []

		self._p_addItem( _( 'Blank model' ), _( 'Create blank model node' ), _( 'Create a blank model node' ), SceneModelNode )
		self._p_addItem( _( 'Procedure' ), _( 'Create procedure node' ), _( 'Create a procedure node' ), SceneProcedureNode )
		self._p_addItem( _( 'Group' ), _( 'Create group node' ), _( 'Create a group node' ), SceneGroupNode )



	def popup(self, parentWindow, commandHistory, scene):
		self._parentWindow = parentWindow
		self._commandHistory = commandHistory
		self._scene = scene

		self._menu.popup( None, None, None, 1, 0 )


	def _p_addItem(self, itemText, title, message, factory):
		item = gtk.MenuItem( itemText )
		item.connect( 'activate', self._p_onItem )
		item.show()
		self._menu.append( item )
		self._itemToEntry[item] = ( title, message, factory )
		self._items.append( item )


	def _p_onItem(self, item):
		# Get the init function, and name prefix
		title, message, factory = self._itemToEntry[item]

		name = nameDialog( title, message, _( 'Name:' ), '', True, self._parentWindow )
		if name is not None:
			self._commandHistory.freeze()
			node = factory()
			node.name = name
			self._scene.nodes.append( node )
			self._commandHistory.thaw()

