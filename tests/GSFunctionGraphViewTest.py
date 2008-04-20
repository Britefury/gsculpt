##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from copy import *

from Britefury.FileIO.IOXml import *

from Britefury.GraphView.GraphView import *
from Britefury.GSGraph import GSGraph
from Britefury.GSGraph.GSGraph import *

from Britefury.ProceduralCore.GSFunctionGraph import *

from Britefury.CommandHistory.CommandHistory import CommandHistory
from Britefury.GSGraph.GSGraphCommandTracker import GSGraphCommandTracker

import random
import math



class NodeA (GSFunctionGraphNode):
	description = 'Node; A'

	a = NodeSinkMultipleColumn()
	b = NodeSinkMultipleColumn()
	out = NodeSourceColumn()




def buildGraph(graph, bRandom):
	numNodes = 5
	numLinks = int( numNodes * 1.5 )
	nodes = []
	width = NODE_WIDTH + NODE_SPACING
	height = NODE_WIDTH * 0.7  +  NODE_SPACING
	xRange = width * math.sqrt( numNodes )
	yRange = height * math.sqrt( numNodes )
	nodeClasses = [ NodeA ]
	for i in xrange( 0, numNodes ):
		nodeClass = random.choice( nodeClasses )
		node = nodeClass()
		gsnode = node.graphNode
		gsnode.viewPosition = Point2( random.uniform( 0.0, xRange ), random.uniform( 0.0, yRange ) )
		gsnode.bHasViewPosition = True
		nodes.append( ( node, node.sources[0], copy( node.sinks ) ) )
		graph.nodes.append( node )

	for i in xrange( 0, numLinks ):
		bLinked = False
		while not bLinked:
			srcNode, srcSource, srcSinks = random.choice( nodes )
			dstNode, dstSource, dstSinks = random.choice( nodes )
			source = srcSource
			sink = random.choice( dstSinks )

			if sink.graphSink.canAddInput()  and  source.graphSource not in sink.graphSink  and  not sink.graphSink.checkAddInputForCycles( source.graphSource ):
				sink.graphSink.addInput( source.graphSource )
				bLinked = True


def onOpen(widget):
	global graph, commandHistory

	xmlFilter = gtk.FileFilter()
	xmlFilter.set_name( 'XML (*.xml)' )
	xmlFilter.add_pattern( '*.xml' )

	openDialog = gtk.FileChooserDialog( 'Open', window, gtk.FILE_CHOOSER_ACTION_OPEN,
									( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
	openDialog.add_filter( xmlFilter )
	openDialog.show()
	response = openDialog.run()
	filename = openDialog.get_filename()
	openDialog.destroy()
	if response == gtk.RESPONSE_OK:
		if filename is not None:
			f = open( filename, 'r' )
			if f is not None:
				doc = InputXmlDocument()
				doc.parseFile( f )
				contentNode = doc.getContentNode()
				if contentNode.isValid():
					commandHistory.clear()
					commandHistory.stopTracking( graph )
					view.detachGraph()
					graph = GSFunctionGraph()
					contentNode  >>  graph
					commandHistory.track( graph )
					view.attachGraph( graph.graph )


def onSave(widget):
	global graph, commandHistory

	xmlFilter = gtk.FileFilter()
	xmlFilter.set_name( 'XML (*.xml)' )
	xmlFilter.add_pattern( '*.xml' )

	saveAsDialog = gtk.FileChooserDialog( 'Save As', window, gtk.FILE_CHOOSER_ACTION_SAVE,
								( gtk.STOCK_CANCEL, gtk.RESPONSE_CANCEL, gtk.STOCK_OK, gtk.RESPONSE_OK ) )
	saveAsDialog.add_filter( xmlFilter )
	saveAsDialog.show()
	response = saveAsDialog.run()
	filename = saveAsDialog.get_filename()
	saveAsDialog.destroy()
	if response == gtk.RESPONSE_OK:
		if filename is not None:
			doc = OutputXmlDocument()
			doc.getContentNode()  <<  graph
			f = open( filename, 'w' )
			if f is not None:
				doc.writeFile( f )
				f.close()


def onAdd(widget):
	global graph, commandHistory
	commandHistory.freeze()
	node = NodeA()
	node.addSink( GSFunctionGraphNodeSinkMultiple( node, 'q' ) )
	graph.nodes.append( node )
	commandHistory.thaw()


def onRemove(widget):
	global graph, commandHistory
	commandHistory.freeze()
	selectedNodes = list( view.selection )
	for node in selectedNodes:
		graph.nodes.remove( GSFunctionGraphNode.getFunctionGraphNodeForGSGraphNode( node ) )
	commandHistory.thaw()


def onUndo(widget):
	global graph, commandHistory
	if commandHistory.canUndo():
		commandHistory.undo()


def onRedo(widget):
	global graph, commandHistory
	if commandHistory.canRedo():
		commandHistory.redo()



def createLink(sourcePin, sinkPin):
	global graph, commandHistory
	commandHistory.freeze()
	sinkPin.addInput( sourcePin )
	commandHistory.thaw()

def eraseLink(link):
	global graph, commandHistory
	commandHistory.freeze()
	link.erase()
	commandHistory.thaw()




def onDeleteEvent(widget, event, data=None):
	return False

def onDestroy(widget, data=None):
	gtk.main_quit()



window = gtk.Window( gtk.WINDOW_TOPLEVEL );
window.connect( 'delete-event', onDeleteEvent )
window.connect( 'destroy', onDestroy )
window.set_border_width( 10 )
window.set_size_request( 800, 600 )

commandHistory = CommandHistory()

graph = GSFunctionGraph()
buildGraph( graph, True )
commandHistory.track( graph )
print 'built graph'


view = GraphView( createLink, eraseLink )
view.attachGraph( graph.graph )
#view.dndSinkToSourceSignal.connect( onDndSinkToSource )
#view.dndSourceToSinkSignal.connect( onDndSourceToSink )
view.show()

openButton = gtk.Button( stock=gtk.STOCK_OPEN )
openButton.connect( 'clicked', onOpen )

saveButton = gtk.Button( stock=gtk.STOCK_SAVE )
saveButton.connect( 'clicked', onSave )

addNodeButton = gtk.Button( stock=gtk.STOCK_ADD )
addNodeButton.connect( 'clicked', onAdd )

removeNodeButton = gtk.Button( stock=gtk.STOCK_REMOVE )
removeNodeButton.connect( 'clicked', onRemove )

undoButton = gtk.Button( stock=gtk.STOCK_UNDO )
undoButton.connect( 'clicked', onUndo )

redoButton = gtk.Button( stock=gtk.STOCK_REDO )
redoButton.connect( 'clicked', onRedo )

buttonBox = gtk.HBox( True )
buttonBox.pack_start( openButton, True, True, 20 )
buttonBox.pack_start( saveButton, True, True, 20 )
buttonBox.pack_start( addNodeButton, True, True, 20 )
buttonBox.pack_start( removeNodeButton, True, True, 20 )
buttonBox.pack_start( gtk.VSeparator(), False, False, 20 )
buttonBox.pack_start( undoButton, True, True, 20 )
buttonBox.pack_start( redoButton, True, True, 20 )
buttonBox.show_all()

box = gtk.VBox()
box.pack_start( view )
box.pack_start( gtk.HSeparator(), False, False, 10 )
box.pack_start( buttonBox, False, False, 10 )
box.show_all()

window.add( box )
window.show()

gtk.main()
