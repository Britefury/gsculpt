##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

from Britefury.FileIO.IOXml import ioObjectFactoryRegister

from Britefury.Util.SignalSlot import ClassSignal

from Britefury.Config import GSConfig

from Britefury.MainApp.Plugins.PluginPath import *




class PluginTable (object):
	appendedSignal = ClassSignal()
	removeSignal = ClassSignal()
	clearSignal = ClassSignal()

	def __init__(self):
		self._pluginPaths = []


	def __readxml__(self, xmlNode):
		self._pluginPaths = []

		pluginPathsNode = xmlNode.getChild( 'plugin_paths' )
		if pluginPathsNode.isValid():
			for pluginPathNode in pluginPathsNode.childrenNamed( 'plugin_path' ):
				if pluginPathNode.isValid():
					self.append( pluginPathNode.readObject() )


	def __writexml__(self, xmlNode):
		pluginPathsNode = xmlNode.addChild( 'plugin_paths' )
		for pluginPath in self._pluginPaths:
			pluginPathsNode.addChild( 'plugin_path' ).writeObject( pluginPath )



	def append(self, pluginPath):
		self._pluginPaths.append( pluginPath )
		self.appendedSignal.emit( pluginPath )

	def remove(self, pluginPath):
		self.removeSignal.emit( pluginPath )
		self._pluginPaths.remove( pluginPath )

	def clear(self):
		self.clearSignal.emit()
		self._pluginPaths = []

	def __len__(self):
		return len( self._pluginPaths )

	def __contains__(self, pluginPath):
		return pluginPath in self._pluginPaths

	def __iter__(self):
		return iter( self._pluginPaths )

	def __getitem__(self, index):
		return self._pluginPaths[index]

	def index(self, pluginPath):
		return self._pluginPaths.index( pluginPath )


	def loadAll(self):
		for pluginPath in self._pluginPaths:
			pluginPath.pluginInfo


ioObjectFactoryRegister( 'PluginTable', PluginTable )







def readPluginTable(pluginTable, filename):
	if os.path.exists( filename ):
		f = open( filename, 'r' )
		if f is not None:
			doc = InputXmlDocument()
			doc.parseFile( f )
			contentNode = doc.getContentNode()
			if contentNode.isValid():
				contentNode  >>  pluginTable
	else:
		raise IOError, 'Plugin table file \'%s\' does not exist'  %  ( filename, )



def writePluginTable(pluginTable, filename):
	doc = OutputXmlDocument()
	doc.getContentNode()  <<  pluginTable
	f = open( filename, 'w' )
	if f is not None:
		doc.writeFile( f )
		f.close()




def readSystemPluginTable(pluginTable):
	try:
		readPluginTable( pluginTable, GSConfig.pluginsConfigPath )
	except IOError:
		print 'Could not read system plugins file'



def writeSystemPluginTable(pluginTable):
	try:
		writePluginTable( pluginTable, GSConfig.pluginsConfigPath )
	except IOError:
		print 'Could not write system plugins file'
