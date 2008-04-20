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

from Britefury.UI.FormLayout import FormLayout

from Britefury.Model.Model import MarkPredicate

from Britefury.SceneNodes.SceneProcedureNode import SceneProcedureNode

from Britefury.MeshTools.MeshEdit.Face.ToolMeshMarkFaces import ProcMeshMarkFaces
from Britefury.MeshTools.MeshEdit.Face.ToolMeshDeleteFaces import ProcMeshDeleteFaces



def meshDetachFaces(meshNode, scene, sceneEditorTools, nodeEditor, parentWindow, commandHistory):
	originalNodeName = scene.getUnusedNumberedNodeName( meshNode.name + ' pre detach' )
	newNodeName = meshNode.name
	detachedNodeName = scene.getUnusedNumberedNodeName( 'Detached faces' )

	originalNodeNameEntry = gtk.Entry()
	originalNodeNameEntry.set_text( originalNodeName )
	originalNodeNameEntry.show()


	newNodeNameEntry = gtk.Entry()
	newNodeNameEntry.set_text( newNodeName )
	newNodeNameEntry.show()

	detachedNodeNameEntry = gtk.Entry()
	detachedNodeNameEntry.set_text( detachedNodeName )
	detachedNodeNameEntry.show()


	titleLabel = gtk.Label()
	titleLabel.set_markup( '<b><span foreground="#000080">' + _( 'Detach faces' ) + '</span></b>' )
	titleLabel.show()


	layout = FormLayout()
	layout.textBlock( _( 'Detach faces will create two new nodes; one that contains the mesh with the selected faces removed, the other containing only the selected faces. The former will become the current node.' ) )
	layout.row()
	layout.row()  <<  _( 'Original node name:' )  <<  originalNodeNameEntry
	layout.row()
	layout.row()  <<  _( 'New node name:' )  <<  newNodeNameEntry
	layout.row()
	layout.row()  <<  _( 'Detached node name:' )  <<  detachedNodeNameEntry
	layout.finalise()
	layout.getWidget().show()
	layout.getWidget().set_border_width( 40 )


	dialog = gtk.Dialog( _( 'Detach faces' ), parentWindow, gtk.DIALOG_MODAL | gtk.DIALOG_DESTROY_WITH_PARENT,  ( gtk.STOCK_CANCEL, gtk.RESPONSE_REJECT, gtk.STOCK_OK, gtk.RESPONSE_ACCEPT ) )
	dialog.vbox.pack_start( titleLabel, False, False, 20 )
	dialog.vbox.pack_start( layout.getWidget(), True, False )
	dialog.show()
	response = dialog.run()
	dialog.destroy()
	if response == gtk.RESPONSE_ACCEPT:
		originalNodeName = originalNodeNameEntry.get_text()
		newNodeName = newNodeNameEntry.get_text()
		detachedNodeName = detachedNodeNameEntry.get_text()


		if meshNode in scene.nodes:
			commandHistory.freeze()

			originalNode = meshNode
			newNode = SceneProcedureNode()
			detachedNode = SceneProcedureNode()

			# Hide the old node
			originalNode.bVisible = False

			originalNode.name = originalNodeName
			newNode.name = newNodeName
			detachedNode.name = detachedNodeName

			scene.nodes.append( newNode )
			scene.nodes.append( detachedNode )

			newNode.inputModel.append( originalNode.out )
			detachedNode.inputModel.append( originalNode.out )

			newNodeDeleteProc = ProcMeshDeleteFaces()
			newNode.plan.append( newNodeDeleteProc )

			detachedNodeInvertProc = ProcMeshMarkFaces()
			detachedNodeInvertProc.predicate = MarkPredicate.INVERT
			detachedNodeDeleteProc = ProcMeshDeleteFaces()
			detachedNodeMarkProc = ProcMeshMarkFaces()
			detachedNodeMarkProc.predicate = MarkPredicate.MARK
			detachedNode.plan.append( detachedNodeInvertProc )
			detachedNode.plan.append( detachedNodeDeleteProc )
			detachedNode.plan.append( detachedNodeMarkProc )

			sceneEditorTools.currentNode = newNode
			nodeEditor.currentFunction = newNodeDeleteProc

			commandHistory.thaw()


