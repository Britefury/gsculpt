##-*************************
##-* This program is free software; you can use it, redistribute it and/or modify it
##-* under the terms of the GNU General Public License version 2 as published by the
##-* Free Software Foundation. The full text of the GNU General Public License
##-* version 2 can be found in the file named 'COPYING' that accompanies this
##-* program. This source code is (C)copyright Geoffrey French 1999-2007.
##-*************************
from Britefury.Sheet.Sheet import *



class PluginPath (Sheet):
	def _p_pluginInfo(self):
		pluginInfo = None
		try:
			_locals = {}
			execfile( self.path, {}, _locals )
			pluginInfo = _locals['pluginInfo']
			self.bLoadedSuccessfully = True
		except Exception, e:
			print e
			self.bLoadedSuccessfully = False
		return pluginInfo


	path = Field( str, '' )
	bLoadedSuccessfully = Field( bool, False )
	pluginInfo = FunctionRefField( _p_pluginInfo )


