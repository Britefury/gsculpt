##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

import pygtk
pygtk.require( '2.0' )
import gtk

from Britefury.FileIO.IOXml import *

from Britefury.Config import GSConfig

from Britefury.UI.UIAction import UIAction

from Britefury.Event.QueuedEvent import queueEvent

from Britefury.Sheet.Sheet import *



class Shortcut (Sheet):
	def _p_accelLabel(self):
		keyval, mods = gtk.accelerator_parse( self.accelString )
		return gtk.accelerator_get_label( keyval, mods )


	def _p_action(self):
		if self.path != '':
			try:
				action = UIAction._pathToActionTable[self.path]
			except KeyError:
				return None
			else:
				action.accelerator = self.accelString
				return action
		else:
			return None



	path = Field( str, '' )
	accelString = Field( str, '' )
	accelLabel = FunctionField( _p_accelLabel )
	action = FunctionRefField( _p_action )



	def __init__(self):
		super( Shortcut, self ).__init__()

		self.cells.accelString.changedSignal.connect( self._p_onAccelString )
		self.bModified = False
		self.defaultAccelString = self.accelString


	def reset(self):
		self.accelString = self.defaultAccelString
		self.bModified = False


	def _p_onAccelString(self):
		self.bModified = True
		queueEvent( self._p_refreshAction )

	def _p_refreshAction(self):
		self.action


	def _f_copyFrom(self, shortcut):
		self.accelString = shortcut.accelString





class ShortcutTable (object):
	def __init__(self):
		self._pathToShortcut = {}

		self._p_populate()



	def getShortcuts(self):
		return self._pathToShortcut.values()



	def _p_populate(self):
		for action in UIAction._actions:
			shortcut = self._p_newShortcut( action )
			self._pathToShortcut[shortcut.path] = shortcut

	def _p_clear(self):
		self._pathToShortcut = {}
		self._p_populate()



	def __readxml__(self, xmlNode):
		for node in xmlNode.childrenNamed( 'shortcut' ):
			shortcut = Shortcut()
			node  >>  shortcut

			try:
				existingShortcut = self._pathToShortcut[shortcut.path]
			except KeyError:
				pass
			else:
				existingShortcut._f_copyFrom( shortcut )


	def __writexml__(self, xmlNode):
		for shortcut in self._pathToShortcut.values():
			if shortcut.bModified:
				xmlNode.addChild( 'shortcut' )  <<  shortcut



	def _p_newShortcut(self, action):
		shortcut = Shortcut()
		shortcut.accelString = action.accelerator
		shortcut.defaultAccelString = shortcut.accelString
		shortcut.bModified = False
		shortcut.path = action.path
		return shortcut


ioObjectFactoryRegister( 'ShortcutTable', ShortcutTable )




def readShortcuts(shortcutTable, filename):
	if os.path.exists( filename ):
		f = open( filename, 'r' )
		if f is not None:
			doc = InputXmlDocument()
			doc.parseFile( f )
			contentNode = doc.getContentNode()
			if contentNode.isValid():
				contentNode  >>  shortcutTable
	else:
		raise IOError, 'Shortcuts file \'%s\' does not exist'  %  ( filename, )



def writeShortcuts(shortcutTable, filename):
	doc = OutputXmlDocument()
	doc.getContentNode()  <<  shortcutTable
	f = open( filename, 'w' )
	if f is not None:
		doc.writeFile( f )
		f.close()




def readSystemShortcuts(shortcutTable):
	try:
		readShortcuts( shortcutTable, GSConfig.shortcutsConfigPath )
	except IOError:
		print 'Could not read system shortcuts file'



def writeSystemShortcuts(shortcutTable):
	try:
		writeShortcuts( shortcutTable, GSConfig.shortcutsConfigPath )
	except IOError:
		print 'Could not write system shortcuts file'

