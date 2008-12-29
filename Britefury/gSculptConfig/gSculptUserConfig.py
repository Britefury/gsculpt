##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
import os

import ConfigParser

from Britefury.Config import GSConfig



class ConfigProperty (object):
	def __init__(self, section, option, valueType, defaultValue):
		self._section = section
		self._option = option
		self._valueType = valueType
		self._defaultValue = defaultValue


	def __get__(self, obj, objType):
		if obj._config.has_section( self._section )  and  obj._config.has_option( self._section, self._option ):
			return self._valueType( obj._config.get( self._section, self._option ) )
		else:
			return self._defaultValue

	def __set__(self, obj, value):
		if not obj._config.has_section( self._section ):
			obj._config.add_section( self._section )
		return obj._config.set( self._section, self._option, value )

	def __delete__(self, obj):
		obj._config.remove_option( self._section, self._option )




class GSculptUserConfig (object):
	def __init__(self):
		self._config = ConfigParser.RawConfigParser()


	def load(self):
		if os.path.exists( GSConfig.userConfigFilePath ):
			try:
				f = open( GSConfig.userConfigFilePath, 'r' )
			except IOError:
				print 'Could not open user config file for reading'
			else:
				self._config.readfp( f )


	def save(self):
		GSConfig.makeSettingsDir()
		try:
			f = open( GSConfig.userConfigFilePath, 'w' )
		except IOError:
			print 'Could not open user config file for writing'
		else:
			self._config.write( f )


	texturePath = ConfigProperty( 'Paths', 'texturePath', str, os.getcwd() )
	backgroundModelPath = ConfigProperty( 'Paths', 'backgroundModelPath', str, os.getcwd() )
	uiToolPanelWidth = ConfigProperty( 'UI', 'toolPanelWidth', int, 230 )
	projectPath = ConfigProperty( 'Paths', 'projectPath', str, os.getcwd() )
	importPath = ConfigProperty( 'Paths', 'importPath', str, os.getcwd() )
	exportPath = ConfigProperty( 'Paths', 'exportPath', str, os.getcwd() )
	scriptPath = ConfigProperty( 'Paths', 'scriptPath', str, os.getcwd() )
	lastVersionUsed = ConfigProperty( 'Version', 'lastVersionUsed', str, '0.0' )




userConfig = GSculptUserConfig()
